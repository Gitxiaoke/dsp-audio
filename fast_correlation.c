#include "fast_correlation.h"

/* 注意： xcorr(x,y) ≠ xcorr(y,x)
 *
 * 圆周相关与线性相关之间的关系是：
 *
 *      **********************************圆周相关与线性相关之间的关系*******************************
 *      * 1、在不对序列x和序列y补零的情况下：                                                                                                                      *
 * 		* 	如果序列x的长度是N1，序列y的长度是N2，则圆周相关结果的开头N1-N2+1个点的结果与线性相关结果是一致的*
 * 		* 2、在对序列x只末尾补零为L;序列y的前补len(x)零、后面补L-len(y)-len(x)零为相同长度L的情形下：           *
 * 		* 	只有补零后的长度L>=len(x)+len(y)-1时，圆周卷积的后len(x)+len(w)-1个点就与线性卷积结果完全一致 *
 *		*****************************************************************************************
 *
 *
 *  计算xcorr(x,y)的快速卷积步骤(情况2，也就是要补零)：
 *		1、对序列x末尾补零为总长为L; (注意：L为2的整数次幂，以便可以使用FFT，不然就得使用DFT到频域)
 *		2、对序列y的前面补len(x)个零，后面再补零使其总长也为L;
 *		3、对补零后的信号做fft;
 *		4、对y对应的fft结果取共轭;
 *		3、fft的结果点乘;
 *		4、对点乘的结果做ifft;
 *		5、ifft结果的最后N+M-1个点就是线性卷积的结果(其实前面的结果都是零)。
 *
 *
 *	   /-------------------/                                               /------------------------/
 *    /       x      0    /        ---> FFT ---> ( 点乘 ) ---> IFFT --->   /  取output[L-(M+N-1):L]  /
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
	 * 计算大于或等于N的2的整数次幂整数N_2pow
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
	 * 1、对x末尾补零为长度N_2pow
	 * 2、对y做前端做长度为len(x)的补零且后面再做补零使其总长度为N_2pow
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
	 * 计算zeropad 、 weight_zero的FFT
	 */
	fft_real(x_zeropad, x_zeropad_fft, N_2pow);
	fft_real(y_zeropad, y_zeropad_fft, N_2pow);

	/*
	 * 对y_fft取共轭
	 */
	for(i = 0; i < N_2pow; i++)
	{
		y_zeropad_fft->imag = -1 * y_zeropad_fft->imag;
	}

	/*
	 * input_zeropad_fft 点乘 weight_zeropad_fft
	 */
	for(i = 0; i < N_2pow; i++)
	{
		o_zeropad_fft[i] = complex_product(x_zeropad_fft[i], y_zeropad_fft[i]);
	}

	/*
	 * 对output_cascade_fft做ifft
	 */
	ifft_real(o_zeropad_fft, o_zeropad, N_2pow);

	/*
	 * 取ifft结果的前len(x) + len(w) - 1就是线性卷积的结果
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
