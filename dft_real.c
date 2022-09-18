#include "dft_real.h"

/*
 * ����һ����������������ʵ���� DFT�� IDFT C����ʵ�֡� 
 * Here is a DFT and IDFT C function with the input data is real numbers.
 * 
 * 
 * DFT formula: 
 *			X(k) = SUM~n=[0,1,2,...,N-1] x(n)*e^(-j*2*pi*k*n/N), K=[0,1,2,...,N-1]
 * IDFT formula: 
 *			x(n) = 1/N * SUM~k=[0,1,2,...,N-1] X(k)*e^(j*2*pi*k*n/N), n=[0,1,2,...,N-1]
 * 
 * ����DFT�Ľ��ͣ�
 * 		1��������Ϊ Fs�� N���������� x(n),n=0,1,2,...,N-1��DFT������Ϊ N��ĸ��� X(k)=a_k + b_k*j, k=0,1,2,...,N-1.
 *		2���� k��ĸ������ X(k)=a_k + b_k*j����������������Ƶ��Ϊ f=Fs*k/N�� a_k*cos(2*pi*f*n) ��  b_k*sin(2*pi*f*n)���������Һϲ��� 
 *		   a_k*cos(2*pi*f*n) + b_k*sin(2*pi*f*n) = A*cos(2*pi*f*n + ��), ��ֵ A = sqrt(a_k^2 + b_k^2); ��λ ��= arctan(b_k / a_k)
 *      3����x(n)Ϊʵ�����У����У�
 *				3.1 X(k) = X*(N-k), k ��0���ҵ�NΪż��ʱ���м��k=N/2û�й���ԳƵ㣬��ʱ��X(k=N/2)��X(k=0)һ����ʵ���������Ǹ��� 
 *				3.2 X(k=0)����ֱ��������С 
 *				3.3 ʵ����DFT���X(k),k=0,1,2,...,N-1��ȥ��k=0ֱ�������󣬺�ʣ��N-1���ǹ������ĳʹ���Գƹ�ϵ:
 *		   				---���N������,ȥ��k=0��ʣ��N-1��Ϊż��,��X(k=1,2,...,N-1)ȫ�������������ĳʹ���Գ�; 
 *		    			---���N��ż��,ȥ��k=0, ʣ��N-1��Ϊ����,��ȥ��k=0����м�� k=N/2 ����X(k=N/2)Ϊʵ������Ϊ�õ�û�й���ԳƵ�
 * 
 *		4��DFT����FFT����Ƶ�׵Ĳ��裺 
 *				ʵ�źŵ�DFT��FFT���Ľ��Ϊ˫���ף�ֱ�� + ��Ƶ +(�е�Ƶ�㣬NΪż��ʱ)+ ��Ƶ 
 *				ʵ����Ҫ����Ƶ�ʷ�Χ�ǣ�ֱ�� + ��Ƶ +(�е�Ƶ�㣬NΪż��ʱ),Ҳ���ǣ�k=0��1,2,...,N/2 - 1,N/2�����жԳ�Ƶ����ֻΪ��ʵ��ֵ��һ�� 
 *					4.1 ����DFT(����FFT)��Y = fft(x);
 *					4.2 ��N��P2 = abs(Y/N);
 *					4.3 ȡ��Ƶ�ʲ��֣�Ҳ���ǵ����ף�P1 = P2(0:L/2);
 *					4.4 �Գ�Ƶ�ʲ��ֳ�2��P1(1:end-1) = 2*P1(1:end-1); Ҳ����k = 1,2,...,N/2 - 1
 *					
 *					4.5 ��λ��(k) = arctan(imag(X(k)) / real(X(k))) * 180 / PI
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
 * 			(a + b*j)(c + d*j) = (ac-bd) + (bc + ad)*j, ��Ϊx(n)��ʵ�����ȼ۵�˵���ǣ�X(N)���� Fs/2����Գƣ������ֻ��Ҫ����ʵ�����鲿�ǲ���Ҫ����ġ� 
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

