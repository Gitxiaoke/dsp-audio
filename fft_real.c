#include "fft_real.h" 

complex_t complex_product(complex_t a, complex_t b)      
{
    complex_t out; 
    out.real = a.real * b.real - a.imag * b.imag;
    out.imag = a.real * b.imag + a.imag * b.real;
    return out;
}

/* 
 * 基2 fft的原理很简单：1、只要知道蝶形运算的图，然后从左至右，一列就是一级，依次计算每一级蝶形运算的所有输出，最后一级蝶形运算就是 fft结果
 *					 2、输入数据需要按照倒序位码的顺序重新排列
 *					 3、输入数据必须是2的整数次幂
 * 
 *
 *
 * DFT formula: 
 *		X(k) = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1]
 *				Divide x(n) into odd sequences and even sequences： 
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*2t/N) + x(2t+1)e^(-j*2*pi*k*(2t+1)/N)
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*2t/N) + e^(-j*2*pi*k/N) * x(2t+1)e^(-j*2*pi*k*(2t)/N)
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*t/(N/2)) + e^(-j*2*pi*k/N) * x(2t+1)e^(-j*2*pi*k*t/(N/2))
 *			 = SUM~t=[0,1,2,...,N/2 -1] x1(t)e^(-j*2*pi*k*t/(N/2)) + e^(-j*2*pi*k/N) * SUM~t=[0,1,2,...,N/2 -1] x2(t)e^(-j*2*pi*k*t/(N/2))
 *		because:		  
 *			SUM~t=[0,1,2,...,N/2 -1] x1(t)e^(-j*2*pi*k*t/(N/2)) = X1(k)为偶序列 x1(t)的 DFT
 *			SUM~t=[0,1,2,...,N/2 -1] x2(t)e^(-j*2*pi*k*t/(N/2)) = X2(k)为奇序列 x2(t)的 DFT
 *		so:		
 *			X(k) 		= X1(k) + e^(-j*2*pi*k/N) * X2(k) , k = [0,1,2,..., N/2 - 1]  	//X(k)的前半部分 
 *			X(k + N/2) 	= X1(k) - e^(-j*2*pi*k/N) * X2(k) , k = [0,1,2,..., N/2 - 1]	//X(k)的后半部分 
 *			
 *			x1(n)、x2(n)同样可以分别分解为各自的偶序列DFT加上奇序列DFT乘以旋转因子的和；
 *			
 *			这样的分解可以一直持续下去，直到子序列的长度为2，可以得到一个FFT的蝶形信号流图；
 *			
 *			因此，只有到输入数据是2的整数次幂的情况下，基2 FFT算法才成立。 
 *
 *	
 *		FFT的计算流程：(参考FFT的蝶形运算信号流图，从左到右，按列依次计算每一级蝶形运算的输出)
 *						---> 首先按位码倒序重新排序输入信号，
 *						---> 将位码倒序后的输入，与第一级的旋转因子，计算第一级蝶形运算输出； 
 *						---> 再由第一级碟形运算的输出作为第二级蝶形运算的输入，计算第二级蝶形运算的输出；
 *						---> 再由第二级碟形运算的输出作为第三级蝶形运算的输入，计算第三级蝶形运算的输出；
 *						---> 大致如下图所示： 
 *	     	
 *		 输入:x[n] 				第一级输出:X(k)            第二级输出:X(k) 
 *      --------				---------					---------
 *       x[0] 		 			  	.    						.
 *       x[4] 		    			.							.
 *		 x[2]						.							.
 * 		 x[6]						.							.
 *		 x[1]	      第 1 级旋转因子		.		 第 2 级旋转因子	        .            ...直到最后一级蝶形运算
 *		 x[5]						.							.
 *	     x[3]						.							.
 *		 x[7]						.							.
 *							
 *							
 *
 *			旋转因子： 
 *			1、一共有 M = log2(N) 级旋转因子 
 *			2、第m级的旋转因子一共有 2^(m-1)个：W_m(k) = e^(-j*pi*k/(2^(1-m))), k=[1,...,2^(m-1)]
 *			3、旋转因子是等比数列，首项 W_m(k=0) = 1 + 0*j;  
 *			
 *			举例，对于N=2，则一共有 M=log2(2) = 1 级旋转因子， 有 1个旋转因子 W_m(k=1) = e^(-j*pi) = 1 + 0*j
 *				  X(k=0) = x(0) + W_m(k=1) * x(1) = x(0) + x(1)
 *				  X(k=1) = x(0) - W_m(k=1) * x(1) = x(0) - x(1)
 *			
 *			
 *			fft伪代码：
 *				fft(input,N)
 *				{
 *					1、输入倒序排列
 *					2、for(从左到右，计算每一级蝶形运算的输出)
 *					   {
 *							for(遍历一级蝶形运算中不同的旋转因子)
 *							{
 *								for(遍历同一旋转因子参与的所有蝶形运算)
 *								{
 *									蝶形运算 
 *								} 
 *							} 
 *						}
 *				} 
 *				 
 */				  

void fft_real(float *input, complex_t *output, int input_num)
{
	int FFT_N = input_num;
	
	/*
	 * 输入数据按 位码倒序 的顺序重新排列 
	 * The input data is rearranged in reverse bit-code order
	 */
	float temp;
	int i = 0, j = 0, k = 0;
	for(i = 0; i < FFT_N - 1; i++)
	{
		if(i < j)
		{
			temp = input[i];
			input[i] = input[j];
			input[j] = temp;
		}
		
		k = FFT_N /2;
		while(j >= k)
		{
			j = j - k;
			k = k / 2;
		}
		j = j + k;	
	} 
	
	for(i = 0; i < FFT_N; i++)
	{
		output[i].real = input[i];
		output[i].imag = 0.0;
	}

    /*
     * 计算蝶形运算级数,M=log2(N)
	 */ 
	int m, M;
	int t = FFT_N;
	for(M = 1; (t /= 2) != 1; M++);
	
	int p = 0, q = 0, p_start;
	int dist;  /* 蝶形运算时两个输入数据之间的间隔 dist = p - q */
	int rotation_factor_num, factor_Num;
	
	complex_t W; /* 旋转因子 */
	complex_t W_ratio; /* 旋转因子的等比系数  */
	
	complex_t output_p, output_q; 
	
	for(m = 1; m <= M; m++)  /* 从左到右依次计算各级蝶形运算的输出，最后一级蝶形运算的输出就是FFT的结果 */ 
	{
		dist = 1<<(m-1);  /* 第m级蝶形运算两个输入的间隔 dist=2^(m-1) */ 
		factor_Num = 1<<(m-1);
		/* 
		 * 第m级的旋转因子：e^(-j*pi*k/(2^(1-m))), k=[0,1,...,2^(m-1)]
		 * 可以看出，旋转因子是等比数列 
		 * 每一级的第一个旋转因子是：1+0*j
		 * 旋转因子的公比为：e^(-j*pi/(2^(1-m))) 
		 */
		W.real = 1.0;	
        W.imag = 0.0;
        W_ratio.real = cos(PI / dist); 
        W_ratio.imag = -sin(PI / dist);
		 
		for(rotation_factor_num = 0; rotation_factor_num <= factor_Num - 1; rotation_factor_num++)/* 遍历同一级中的所有旋转因子，其中，第m级蝶形运算的旋转因子数量为2^(m-1) */ 
		{
			/* 
			 * p为蝶形运算的第一个输入，q为蝶形运算的第二个输入
			 * 第 rotation_factor_num 个旋转因子的第一个输入的下标 q 为 rotation_factor_num
			 */
			p_start = rotation_factor_num;
			for(p = p_start; p <= FFT_N-1; p = p + 2*dist) /* 遍历同一蝶形因子参与的所有运算，其中，同一蝶形因子的第一个输入之间的间隔为 2*dist */
			{
				q = p + dist;
			    
				output_p= output[p];
			    output_q= output[q];
                       
				output[p].real = output_p.real + complex_product(output_q, W).real;  // 蝶形运算公式
                output[p].imag = output_p.imag + complex_product(output_q, W).imag;
				output[q].real = output_p.real - complex_product(output_q, W).real;
                output[q].imag = output_p.imag - complex_product(output_q, W).imag;
			}
			W = complex_product(W_ratio, W);
		}		
	}	
}
/*
 * DFT formula：
 *		DFT的矩阵形式：
 *		X = W * x
 *		X = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1] 
 *
 *		(   X(0)   )  		( w^-0*0   	  w^-1*0      w^-2*0   	 ...   w^-(N-1)*0     	)	( x(0)   )
 *		(   X(1)   )		( w^-0*1   	  w^-1*1      w^-2*1   	 ...   w^-(N-1)*1		)	( x(1)   )
 *		(   X(2)   )		( w^-0*2   	  w^-1*2      w^-2*2   	 ...   w^-(N-1)*2		)	( x(2)   )
 *		(   .      )   = 	(   .   		.   	   .		  .       .				) *	(  .  	 )
 *		(   .      )		(   .   		. 	   	   .  		  . 	  .				)	(  .	 )
 *		(   .      )		(   .   		.		   .		  .		  .				)	(  .	 )
 *		(   x(N-1) )		( w^-0*(N-1)  w^-1*(N-1)  w^-2*(N-1) ...   w^-(N-1)*(N-1)  	)	( x(N-1) )
 *
 * IDFT formula:
 *		x = inverse(W) * X 
 * 		神奇的地方在于： conjugate(W) * W = N * I, 其中 I为单位阵 
 *		也就是说：		 inverse(W) = 1/N * conjugate(W)  
 * 		
 * 		IDFT: x=1/N * conjugate(W)* X ,等式两边同时求共轭，可得 
 *			  conjugate(x) = x = 1/N * W * conjugate(X) 
 *			  
 */ 
void ifft_real(complex_t *input, float *output, int input_num)
{
	int FFT_N = input_num;
	
	int i = 0, j = 0, k = 0;\
	
	/*
	 * 对输入X(k)取共轭 
	 */
	for(i = 0; i < FFT_N; i++)
	{
		input[i].imag = -1 * input[i].imag;
	}
	
	/*
	 * 输入数据按 位码倒序 的顺序重新排列 
	 * The input data is rearranged in reverse bit-code order
	 */
	complex_t temp;
	for(i = 0; i < FFT_N - 1; i++)
	{
		if(i < j)
		{
			temp = input[i];
			input[i] = input[j];
			input[j] = temp;
		}
		
		k = FFT_N /2;
		while(j >= k)
		{
			j = j - k;
			k = k / 2;
		}
		j = j + k;	
	} 
	
    /*
     * 计算蝶形运算级数,M=log2(N)
	 */ 
	int m, M;
	int t = FFT_N;
	for(M = 1; (t /= 2) != 1; M++);
	
	int p = 0, q = 0, p_start;
	int dist;  /* 蝶形运算时两个输入数据之间的间隔 dist = p - q */
	int rotation_factor_num, factor_Num;
	
	complex_t W; /* 旋转因子 */
	complex_t W_ratio; /* 旋转因子的等比系数  */
	
	complex_t input_p, input_q; 
	
	for(m = 1; m <= M; m++)  /* 从左到右依次计算各级蝶形运算的输出，最后一级蝶形运算的输出就是FFT的结果 */ 
	{
		dist = 1<<(m-1);  /* 第m级蝶形运算两个输入的间隔 dist=2^(m-1) */ 
		factor_Num = 1<<(m-1);
		/* 
		 * 第m级的旋转因子：e^(-j*pi*k/(2^(1-m))), k=[0,1,...,2^(m-1)]
		 * 可以看出，旋转因子是等比数列 
		 * 每一级的第一个旋转因子是：1+0*j
		 * 旋转因子的公比为：e^(-j*pi/(2^(1-m))) 
		 */
		W.real = 1.0;	
        W.imag = 0.0;
        W_ratio.real = cos(PI / dist); 
        W_ratio.imag = -sin(PI / dist);
		 
		for(rotation_factor_num = 0; rotation_factor_num <= factor_Num - 1; rotation_factor_num++)/* 遍历同一级中的所有旋转因子，其中，第m级蝶形运算的旋转因子数量为2^(m-1) */ 
		{
			/* 
			 * p为蝶形运算的第一个输入，q为蝶形运算的第二个输入
			 * 第 rotation_factor_num 个旋转因子的第一个输入的下标 q 为 rotation_factor_num
			 */
			p_start = rotation_factor_num;
			for(p = p_start; p <= FFT_N-1; p = p + 2*dist) /* 遍历同一蝶形因子参与的所有运算，其中，同一蝶形因子的第一个输入之间的间隔为 2*dist */
			{
				q = p + dist;
			    
				input_p= input[p];
			    input_q= input[q];
                       
				input[p].real = input_p.real + complex_product(input_q, W).real;  // 蝶形运算公式
                input[p].imag = input_p.imag + complex_product(input_q, W).imag;
				input[q].real = input_p.real - complex_product(input_q, W).real;
                input[q].imag = input_p.imag - complex_product(input_q, W).imag;
			}
			W = complex_product(W_ratio, W);
		}		
	}
	
	/*
	 * 输出 
	 */
	for(i = 0; i < FFT_N; i++)
	{
		output[i] = input[i].real / FFT_N;
	}	
}

/*
 * 当输入x(n),n = [0,1,2,...,N-1],N为偶数时，即偶数个输入点时：
 *
 *	fft变换的第一个点X(k=0)代表直流分量
 * 	剩下了N-1个技术点代表交流分量，其中一半为正频，而另一半是负频，
 * 	还剩一个中间点X(k = N/2)没有对称点，也就是正频的最后一个点，实际代表的频率为(N/2)*(Fs/N)=Fs/2。
 * 	spectrum = {fft(k=0)/N, 2*[fft(k=1)/N，fft(k=2)/N,...,fft(k=N/2-1)/N], fft(k=N/2)}， 一共N/2+1个点。
 */
void spectrum(float *input, int input_num, float *output_spectrum)
{
	int N = input_num;
	complex_t input_fft[N];

	fft_real(input, input_fft, N);

	for(int n = 0; n < N/2 + 1; n++)
	{
		output_spectrum[n] = sqrt(input_fft[n].real  * input_fft[n].real + input_fft[n].imag * input_fft[n].imag);
		output_spectrum[n] = output_spectrum[n] / N;
		if(n >= 0 && n <= N/2)
		{
			output_spectrum[n] = 2 * output_spectrum[n];
		}
	}
}
