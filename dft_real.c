#include "dft_real.h"

/*
 * 这是一个关于输入数据是实数的 DFT和 IDFT C语言实现。 
 * Here is a DFT and IDFT C function with the input data is real numbers.
 * 
 * 
 * DFT formula: 
 *			X(k) = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1]
 * IDFT formula: 
 *			x(n) = 1/N * SUM~k=[0,1,2,...,N-1] X(k)*e^(j*2*pi*k*n/N), n=[0,1,2,...,N-1]
 * 
 * 关于DFT的解释：
 * 		1、采样率为 Fs的 N点输入数据 x(n),n=0,1,2,...,N-1的DFT，其结果为 N点的复数 X(k)=a_k + b_k*j, k=0,1,2,...,N-1.
 *		2、第 k点的复数结果 X(k)=a_k + b_k*j，代表输入数据中频率为 f=Fs*k/N的 a_k*cos(2*pi*f*n) 与  b_k*sin(2*pi*f*n)，正弦余弦合并： 
 *		   a_k*cos(2*pi*f*n) + b_k*sin(2*pi*f*n) = A*cos(2*pi*f*n + θ), 幅值 A = sqrt(a_k^2 + b_k^2); 相位 θ= arctan(b_k / a_k)
 *      3、当x(n)为实数序列，则有：
 *				3.1 X(k) = X*(N-k), k ≠0，且当N为偶数时，中间点k=N/2没有共轭对称点，此时的X(k=N/2)与X(k=0)一样是实数，而不是复数 
 *				3.2 X(k=0)代表直流分量大小 
 *				3.3 实数的DFT结果X(k),k=0,1,2,...,N-1在去除k=0直流分量后，后剩下N-1点是关于中心呈共轭对称关系:
 *		   				---如果N是奇数,去除k=0，剩下N-1点为偶数,则X(k=1,2,...,N-1)全部两两关于中心呈共轭对称; 
 *		    			---如果N是偶数,去除k=0, 剩下N-1点为奇数,则去除k=0后的中间点 k=N/2 处的X(k=N/2)为实数，因为该点没有共轭对称点
 * 
 *		4、DFT或者FFT计算频谱的步骤： 
 *				实信号的DFT（FFT）的结果为双边谱：直流 + 正频 +(中点频点，N为偶数时)+ 负频 
 *				实际需要的正频率范围是：直流 + 正频 +(中点频点，N为偶数时),也就是：k=0，1,2,...,N/2 - 1,N/2。其中对称频部分只为真实幅值的一半 
 *					4.1 计算DFT(或者FFT)：Y = fft(x);
 *					4.2 除N：P2 = abs(Y/N);
 *					4.3 取正频率部分，也就是单边谱：P1 = P2(0:L/2);
 *					4.4 对称频率部分乘2：P1(1:end-1) = 2*P1(1:end-1); 也就是k = 1,2,...,N/2 - 1
 *					
 *					4.5 相位θ(k) = arctan(imag(X(k)) / real(X(k))) * 180 / PI
 *
 * 																	  
 */ 



/*
 * DFT formula: 
 *			X(k) = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1]
 * 		
 * because: 
 		   	e^(jA)  = cos(A) + j*sin(A) 
 *         	e^(-jA) = cos(A) - j*sin(A) 
 * so:
 *			real(X(k)) = SUM~n=[0,1,2,...,N-1] x(n)*cos(2*pi*k*n/N), K=[0,1,2,...,N-1]
 *     		imag(X(k)) = SUM~n=[0,1,2,...,N-1] -1*x(n)*sin(2*pi*k*n/N), K=[0,1,2,...,N-1]
 *
 */ 

void dft(float *input, complex_t *output, int inputNum)
{
	int N = inputNum;
	
	int k,n;
	for(k = 0; k <= N - 1; k++)
	{
		output[k].real = 0;
		output[k].imag = 0;
		for(n = 0; n <= N - 1; n++)
		{
			output[k].real += input[n] * cos(2 * PI * k * n / N);
			output[k].imag -= input[n] * sin(2 * PI * k * n / N);
		}
	}
}

/*
 * IDFT formula: 
 *			x(n) = 1/N * SUM~k=[0,1,2,...,N-1] X(k)*e^(j*2*pi*k*n/N), n=[0,1,2,...,N-1]
 * 		
 * because: 
 *		   	e^(jA)  = cos(A) + j*sin(A) 
 * 			(a + b*j)(c + d*j) = (ac-bd) + (bc + ad)*j, 因为x(n)是实数（等价的说法是：X(N)关于 Fs/2共轭对称），因此只需要计算实部，虚部是不需要计算的。 
 * so:
 * 			x(n) = 1/N * { SUM~k=[0,1,2,...,N-1] real(X(k))*cos(2*pi*k*n/N) - imag(X(k))*sin(2*pi*k*n/N) }
 *											    
 *
 */ 
void idft(complex_t *input, float *output, int inputNum)
{
	int N = inputNum;
	
	int k,n;
	for(n = 0; n <= N - 1; n++)
	{
		output[n] = 0; 
		for(k = 0; k <= N - 1; k++)
		{
			output[n] += input[k].real * cos(2 * PI * k * n / N) - \
			 			 input[k].imag * sin(2 * PI * k * n / N);
		}
		output[n] = output[n] / N;
	}
}

