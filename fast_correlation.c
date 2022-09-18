#include "fast_correlation.h"

/* ע�⣺ xcorr(x,y) �� xcorr(y,x)
 *
 * Բ��������������֮��Ĺ�ϵ�ǣ�
 *
 *      **********************************Բ��������������֮��Ĺ�ϵ*******************************
 *      * 1���ڲ�������x������y���������£�                                                                                                                      *
 * 		* 	�������x�ĳ�����N1������y�ĳ�����N2����Բ����ؽ���Ŀ�ͷN1-N2+1����Ľ����������ؽ����һ�µ�*
 * 		* 2���ڶ�����xֻĩβ����ΪL;����y��ǰ��len(x)�㡢���油L-len(y)-len(x)��Ϊ��ͬ����L�������£�           *
 * 		* 	ֻ�в����ĳ���L>=len(x)+len(y)-1ʱ��Բ�ܾ���ĺ�len(x)+len(w)-1����������Ծ�������ȫһ�� *
 *		*****************************************************************************************
 *
 *
 *  ����xcorr(x,y)�Ŀ��پ������(���2��Ҳ����Ҫ����)��
 *		1��������xĩβ����Ϊ�ܳ�ΪL; (ע�⣺LΪ2���������ݣ��Ա����ʹ��FFT����Ȼ�͵�ʹ��DFT��Ƶ��)
 *		2��������y��ǰ�油len(x)���㣬�����ٲ���ʹ���ܳ�ҲΪL;
 *		3���Բ������ź���fft;
 *		4����y��Ӧ��fft���ȡ����;
 *		3��fft�Ľ�����;
 *		4���Ե�˵Ľ����ifft;
 *		5��ifft��������N+M-1����������Ծ���Ľ��(��ʵǰ��Ľ��������)��
 *
 *
 *	   /-------------------/                                               /------------------------/
 *    /       x      0    /        ---> FFT ---> ( ��� ) ---> IFFT --->   /  ȡoutput[L-(M+N-1):L]  /
 *   /-------------------/				             ^                   /-------------------------/
 *										             |
 *										             |
 *     /-------------------/			             |
 *    / 0_len(x)   y   0  / ---> FFT --- conjugate ---
 *   /-------------------/
 *
 */
void fast_correlation(float *x_buff,int x_len, float *y_buff, int y_len, float *output, int o_len)
{
	float *x_zeropad;
	float *y_zeropad;
	float *o_zeropad;
	complex_t *x_zeropad_fft;
	complex_t *y_zeropad_fft;
	complex_t *o_zeropad_fft;

	int N = x_len + y_len - 1;
	int i = 1,t = N;
	int N_2pow;
	/*
	 * ������ڻ����N��2��������������N_2pow
	 */
	do
	{
		t = t / 2;
		i = i + 1;
	}while(1 != t);

	N_2pow = 1 << i;

	x_zeropad  = (float *)malloc(N_2pow * sizeof(float));
	y_zeropad  = (float *)malloc(N_2pow * sizeof(float));
	o_zeropad  = (float *)malloc(N_2pow * sizeof(float));
	x_zeropad_fft = (complex_t *)malloc(N_2pow * sizeof(complex_t));
	y_zeropad_fft = (complex_t *)malloc(N_2pow * sizeof(complex_t));
	o_zeropad_fft = (complex_t *)malloc(N_2pow * sizeof(complex_t));

	/*
	 * 1����xĩβ����Ϊ����N_2pow
	 * 2����y��ǰ��������Ϊlen(x)�Ĳ����Һ�����������ʹ���ܳ���ΪN_2pow
	 */

	for(i = 0; i < N_2pow; i++)
	{
		if(i < x_len)
			x_zeropad[i] = x_buff[i];
		else
			x_zeropad[i] = 0;
	}

	for(i = 0; i < N_2pow; i++)
	{
		if(i < x_len)
			y_zeropad[i] = 0;
		else if(i >= x_len && i <= (x_len + y_len))
			y_zeropad[i] = y_buff[i - x_len];
		else
			y_zeropad[i] = 0;
	}

	/*
	 * ����zeropad �� weight_zero��FFT
	 */
	fft_real(x_zeropad, x_zeropad_fft, N_2pow);
	fft_real(y_zeropad, y_zeropad_fft, N_2pow);

	/*
	 * ��y_fftȡ����
	 */
	for(i = 0; i < N_2pow; i++)
	{
		y_zeropad_fft->imag = -1 * y_zeropad_fft->imag;
	}

	/*
	 * input_zeropad_fft ��� weight_zeropad_fft
	 */
	for(i = 0; i < N_2pow; i++)
	{
		o_zeropad_fft[i] = complex_product(x_zeropad_fft[i], y_zeropad_fft[i]);
	}

	/*
	 * ��output_cascade_fft��ifft
	 */
	ifft_real(o_zeropad_fft, o_zeropad, N_2pow);

	/*
	 * ȡifft�����ǰlen(x) + len(w) - 1�������Ծ���Ľ��
	 */
	for(i = N_2pow - N; i <= N_2pow; i++)
	{
		output[i-(N_2pow - N)] = o_zeropad[i];
	}

	free(x_zeropad);
	free(y_zeropad);
	free(o_zeropad);
	free(x_zeropad_fft);
	free(y_zeropad_fft);
	free(o_zeropad_fft);
}
