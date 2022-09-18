#include "fft_real.h" 

complex_t complex_product(complex_t a, complex_t b)      
{
    complex_t out; 
    out.real = a.real * b.real - a.imag * b.imag;
    out.imag = a.real * b.imag + a.imag * b.real;
    return out;
}

/* 
 * ��2 fft��ԭ��ܼ򵥣�1��ֻҪ֪�����������ͼ��Ȼ��������ң�һ�о���һ�������μ���ÿһ�����������������������һ������������� fft���
 *					 2������������Ҫ���յ���λ���˳����������
 *					 3���������ݱ�����2����������
 * 
 *
 *
 * DFT formula: 
 *		X(k) = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1]
 *				Divide x(n) into odd sequences and even sequences�� 
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*2t/N) + x(2t+1)e^(-j*2*pi*k*(2t+1)/N)
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*2t/N) + e^(-j*2*pi*k/N) * x(2t+1)e^(-j*2*pi*k*(2t)/N)
 *			 = SUM~t=[0,1,2,...,N/2 -1] x(2t)e^(-j*2*pi*k*t/(N/2)) + e^(-j*2*pi*k/N) * x(2t+1)e^(-j*2*pi*k*t/(N/2))
 *			 = SUM~t=[0,1,2,...,N/2 -1] x1(t)e^(-j*2*pi*k*t/(N/2)) + e^(-j*2*pi*k/N) * SUM~t=[0,1,2,...,N/2 -1] x2(t)e^(-j*2*pi*k*t/(N/2))
 *		because:		  
 *			SUM~t=[0,1,2,...,N/2 -1] x1(t)e^(-j*2*pi*k*t/(N/2)) = X1(k)Ϊż���� x1(t)�� DFT
 *			SUM~t=[0,1,2,...,N/2 -1] x2(t)e^(-j*2*pi*k*t/(N/2)) = X2(k)Ϊ������ x2(t)�� DFT
 *		so:		
 *			X(k) 		= X1(k) + e^(-j*2*pi*k/N) * X2(k) , k = [0,1,2,..., N/2 - 1]  	//X(k)��ǰ�벿�� 
 *			X(k + N/2) 	= X1(k) - e^(-j*2*pi*k/N) * X2(k) , k = [0,1,2,..., N/2 - 1]	//X(k)�ĺ�벿�� 
 *			
 *			x1(n)��x2(n)ͬ�����Էֱ�ֽ�Ϊ���Ե�ż����DFT����������DFT������ת���ӵĺͣ�
 *			
 *			�����ķֽ����һֱ������ȥ��ֱ�������еĳ���Ϊ2�����Եõ�һ��FFT�ĵ����ź���ͼ��
 *			
 *			��ˣ�ֻ�е�����������2���������ݵ�����£���2 FFT�㷨�ų����� 
 *
 *	
 *		FFT�ļ������̣�(�ο�FFT�ĵ��������ź���ͼ�������ң��������μ���ÿһ��������������)
 *						---> ���Ȱ�λ�뵹���������������źţ�
 *						---> ��λ�뵹�������룬���һ������ת���ӣ������һ��������������� 
 *						---> ���ɵ�һ����������������Ϊ�ڶ���������������룬����ڶ�����������������
 *						---> ���ɵڶ�����������������Ϊ������������������룬�����������������������
 *						---> ��������ͼ��ʾ�� 
 *	     	
 *		 ����:x[n] 				��һ�����:X(k)            �ڶ������:X(k) 
 *      --------				---------					---------
 *       x[0] 		 			  	.    						.
 *       x[4] 		    			.							.
 *		 x[2]						.							.
 * 		 x[6]						.							.
 *		 x[1]	      �� 1 ����ת����		.		 �� 2 ����ת����	        .            ...ֱ�����һ����������
 *		 x[5]						.							.
 *	     x[3]						.							.
 *		 x[7]						.							.
 *							
 *							
 *
 *			��ת���ӣ� 
 *			1��һ���� M = log2(N) ����ת���� 
 *			2����m������ת����һ���� 2^(m-1)����W_m(k) = e^(-j*pi*k/(2^(1-m))), k=[1,...,2^(m-1)]
 *			3����ת�����ǵȱ����У����� W_m(k=0) = 1 + 0*j;  
 *			
 *			����������N=2����һ���� M=log2(2) = 1 ����ת���ӣ� �� 1����ת���� W_m(k=1) = e^(-j*pi) = 1 + 0*j
 *				  X(k=0) = x(0) + W_m(k=1) * x(1) = x(0) + x(1)
 *				  X(k=1) = x(0) - W_m(k=1) * x(1) = x(0) - x(1)
 *			
 *			
 *			fftα���룺
 *				fft(input,N)
 *				{
 *					1�����뵹������
 *					2��for(�����ң�����ÿһ��������������)
 *					   {
 *							for(����һ�����������в�ͬ����ת����)
 *							{
 *								for(����ͬһ��ת���Ӳ�������е�������)
 *								{
 *									�������� 
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
	 * �������ݰ� λ�뵹�� ��˳���������� 
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
     * ����������㼶��,M=log2(N)
	 */ 
	int m, M;
	int t = FFT_N;
	for(M = 1; (t /= 2) != 1; M++);
	
	int p = 0, q = 0, p_start;
	int dist;  /* ��������ʱ������������֮��ļ�� dist = p - q */
	int rotation_factor_num, factor_Num;
	
	complex_t W; /* ��ת���� */
	complex_t W_ratio; /* ��ת���ӵĵȱ�ϵ��  */
	
	complex_t output_p, output_q; 
	
	for(m = 1; m <= M; m++)  /* ���������μ�����������������������һ������������������FFT�Ľ�� */ 
	{
		dist = 1<<(m-1);  /* ��m������������������ļ�� dist=2^(m-1) */ 
		factor_Num = 1<<(m-1);
		/* 
		 * ��m������ת���ӣ�e^(-j*pi*k/(2^(1-m))), k=[0,1,...,2^(m-1)]
		 * ���Կ�������ת�����ǵȱ����� 
		 * ÿһ���ĵ�һ����ת�����ǣ�1+0*j
		 * ��ת���ӵĹ���Ϊ��e^(-j*pi/(2^(1-m))) 
		 */
		W.real = 1.0;	
        W.imag = 0.0;
        W_ratio.real = cos(PI / dist); 
        W_ratio.imag = -sin(PI / dist);
		 
		for(rotation_factor_num = 0; rotation_factor_num <= factor_Num - 1; rotation_factor_num++)/* ����ͬһ���е�������ת���ӣ����У���m�������������ת��������Ϊ2^(m-1) */ 
		{
			/* 
			 * pΪ��������ĵ�һ�����룬qΪ��������ĵڶ�������
			 * �� rotation_factor_num ����ת���ӵĵ�һ��������±� q Ϊ rotation_factor_num
			 */
			p_start = rotation_factor_num;
			for(p = p_start; p <= FFT_N-1; p = p + 2*dist) /* ����ͬһ�������Ӳ�����������㣬���У�ͬһ�������ӵĵ�һ������֮��ļ��Ϊ 2*dist */
			{
				q = p + dist;
			    
				output_p= output[p];
			    output_q= output[q];
                       
				output[p].real = output_p.real + complex_product(output_q, W).real;  // �������㹫ʽ
                output[p].imag = output_p.imag + complex_product(output_q, W).imag;
				output[q].real = output_p.real - complex_product(output_q, W).real;
                output[q].imag = output_p.imag - complex_product(output_q, W).imag;
			}
			W = complex_product(W_ratio, W);
		}		
	}	
}
/*
 * DFT formula��
 *		DFT�ľ�����ʽ��
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
 * 		����ĵط����ڣ� conjugate(W) * W = N * I, ���� IΪ��λ�� 
 *		Ҳ����˵��		 inverse(W) = 1/N * conjugate(W)  
 * 		
 * 		IDFT: x=1/N * conjugate(W)* X ,��ʽ����ͬʱ����ɵ� 
 *			  conjugate(x) = x = 1/N * W * conjugate(X) 
 *			  
 */ 
void ifft_real(complex_t *input, float *output, int input_num)
{
	int FFT_N = input_num;
	
	int i = 0, j = 0, k = 0;\
	
	/*
	 * ������X(k)ȡ���� 
	 */
	for(i = 0; i < FFT_N; i++)
	{
		input[i].imag = -1 * input[i].imag;
	}
	
	/*
	 * �������ݰ� λ�뵹�� ��˳���������� 
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
     * ����������㼶��,M=log2(N)
	 */ 
	int m, M;
	int t = FFT_N;
	for(M = 1; (t /= 2) != 1; M++);
	
	int p = 0, q = 0, p_start;
	int dist;  /* ��������ʱ������������֮��ļ�� dist = p - q */
	int rotation_factor_num, factor_Num;
	
	complex_t W; /* ��ת���� */
	complex_t W_ratio; /* ��ת���ӵĵȱ�ϵ��  */
	
	complex_t input_p, input_q; 
	
	for(m = 1; m <= M; m++)  /* ���������μ�����������������������һ������������������FFT�Ľ�� */ 
	{
		dist = 1<<(m-1);  /* ��m������������������ļ�� dist=2^(m-1) */ 
		factor_Num = 1<<(m-1);
		/* 
		 * ��m������ת���ӣ�e^(-j*pi*k/(2^(1-m))), k=[0,1,...,2^(m-1)]
		 * ���Կ�������ת�����ǵȱ����� 
		 * ÿһ���ĵ�һ����ת�����ǣ�1+0*j
		 * ��ת���ӵĹ���Ϊ��e^(-j*pi/(2^(1-m))) 
		 */
		W.real = 1.0;	
        W.imag = 0.0;
        W_ratio.real = cos(PI / dist); 
        W_ratio.imag = -sin(PI / dist);
		 
		for(rotation_factor_num = 0; rotation_factor_num <= factor_Num - 1; rotation_factor_num++)/* ����ͬһ���е�������ת���ӣ����У���m�������������ת��������Ϊ2^(m-1) */ 
		{
			/* 
			 * pΪ��������ĵ�һ�����룬qΪ��������ĵڶ�������
			 * �� rotation_factor_num ����ת���ӵĵ�һ��������±� q Ϊ rotation_factor_num
			 */
			p_start = rotation_factor_num;
			for(p = p_start; p <= FFT_N-1; p = p + 2*dist) /* ����ͬһ�������Ӳ�����������㣬���У�ͬһ�������ӵĵ�һ������֮��ļ��Ϊ 2*dist */
			{
				q = p + dist;
			    
				input_p= input[p];
			    input_q= input[q];
                       
				input[p].real = input_p.real + complex_product(input_q, W).real;  // �������㹫ʽ
                input[p].imag = input_p.imag + complex_product(input_q, W).imag;
				input[q].real = input_p.real - complex_product(input_q, W).real;
                input[q].imag = input_p.imag - complex_product(input_q, W).imag;
			}
			W = complex_product(W_ratio, W);
		}		
	}
	
	/*
	 * ��� 
	 */
	for(i = 0; i < FFT_N; i++)
	{
		output[i] = input[i].real / FFT_N;
	}	
}

/*
 * ������x(n),n = [0,1,2,...,N-1],NΪż��ʱ����ż���������ʱ��
 *
 *	fft�任�ĵ�һ����X(k=0)����ֱ������
 * 	ʣ����N-1�����������������������һ��Ϊ��Ƶ������һ���Ǹ�Ƶ��
 * 	��ʣһ���м��X(k = N/2)û�жԳƵ㣬Ҳ������Ƶ�����һ���㣬ʵ�ʴ����Ƶ��Ϊ(N/2)*(Fs/N)=Fs/2��
 * 	spectrum = {fft(k=0)/N, 2*[fft(k=1)/N��fft(k=2)/N,...,fft(k=N/2-1)/N], fft(k=N/2)}�� һ��N/2+1���㡣
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
