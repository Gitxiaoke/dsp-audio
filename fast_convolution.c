#include "fast_convolution.h"

#include <stdlib.h>
#include <math.h>
/*--------------------------1、短序列的快速卷积（不用考虑状态寄存器）-----------------------------------------
 *
 *
 * 		时域内两序列（较短的末尾需要补零与较长序列的长度一致）的圆周卷积 = 两序列 DFT(FFT) 后结果的乘积
 *
 *      ******************************《圆周卷积》 与 《线性卷积》 之间的关系******************************
 *      * 1、在不对较长序列 x 补零的情况下：                                                                                                                                                  *
 * 		* 	如果序列x的长度是N1，序列w的长度是N2，则圆周卷积结果的末尾N1-N2+1个点的结果与线性卷积结果是一致的            *
 * 		*
 * 		* 2、在对序列x和序列w末尾都补零为相同长度L的情形下：                                                                                                                    *
 * 		* 	当两序列补零后的长度L>=len(x)+len(w)-1时，圆周卷积的前len(x)+len(w)-1个点就与线性卷积结果完全一致  *
 * 		*                                                                                        *
 * 		*	为了能够加速运算补零后序列的DFT结果，L就需要补零为2的整数次幂，就可以使用FFT                       *
 *		******************************************************************************************
 *
 *   使用 DFT(FFT) 计算两个序列 x(n),n=[0,1,2,...,N-1] 与 w(n),n=[0,1,2,...,M-1]的线性卷积步骤:
 *
 *		0、计算补零长度, L >= N+M-1, 且 L = 2的整数次幂
 *		1、对两个序列末尾补零为长度L
 *		2、对补零后的信号做fft
 *	    3、对两个序列fft的结果进行点乘
 *		4、对点乘的结果做ifft
 *		5、ifft结果的前面N+M-1个点就是线性卷积的结果，其实后面的结果都是零。
 *
 *     /-------------------/                                       /---------------------/
 *    /       x   0(补零)  / ---> FFT ---> ( 点乘 ) ---> IFFT --->  /  取output[0:N+M-1]   /
 *   /-------------------/				     ^                   /---------------------/
 *										     |
 *										     |
 *     /-------------------/			     |
 *    /       w   0(补零)  / ---> FFT ---------
 *   /-------------------/
 *
 *
 */
void fast_convolution(float 	*input,
						int 	input_len,
						float 	*weight,
						int 	w_len,
						float 	*output)
{
	float *input_zeropad;
	float *weight_zeropad;

	complex_t *input_zeropad_fft;
	complex_t *weight_zeropad_fft;
	complex_t *output_cascade_fft;

	int N = input_len + w_len - 1;
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

	input_zeropad  = (float *)malloc(N_2pow * sizeof(float));
	weight_zeropad = (float *)malloc(N_2pow * sizeof(float));
	input_zeropad_fft  = (complex_t *)malloc(N_2pow * sizeof(complex_t));
	weight_zeropad_fft = (complex_t *)malloc(N_2pow * sizeof(complex_t));
	output_cascade_fft = (complex_t *)malloc(N_2pow * sizeof(complex_t));

	/*
	 * 对input、weight末尾补零为长度N_2pow
	 */

	for(i = 0; i < N_2pow; i++)
	{
		if(i < input_len)
			input_zeropad[i] = input[i];
		else
			input_zeropad[i] = 0;
	}
	for(i = 0; i < N_2pow; i++)
	{
		if(i < w_len)
			weight_zeropad[i] = weight[i];
		else
			weight_zeropad[i] = 0;
	}

	/*
	 * 计算input_zeropad 、 weight_zeropad的FFT
	 */

	fft_real(input_zeropad, input_zeropad_fft, N_2pow);
	fft_real(weight_zeropad, weight_zeropad_fft, N_2pow);
	/*
	 * input_zeropad_fft 点乘 weight_zeropad_fft
	 */

	for(i = 0; i < N_2pow; i++)
	{
		output_cascade_fft[i] = complex_product(input_zeropad_fft[i], weight_zeropad_fft[i]);
	}

	/*
	 * 对output_cascade_fft做ifft
	 */
	ifft_real(output_cascade_fft, input_zeropad, N_2pow);
	/*
	 * 取ifft结果的前len(x) + len(w) - 1就是线性卷积的结果
	 */
	for(i = 0; i <= input_len + w_len - 1; i++)
	{
		output[i] = input_zeropad[i];
	}

	free(input_zeropad);
	free(weight_zeropad);
	free(input_zeropad_fft);
	free(weight_zeropad_fft);
	free(output_cascade_fft);
}
/*--------------------------2、长序列的快速卷积（需要考虑状态寄存器）-----------------------------------------
 *
 *
 *      ******************************《圆周卷积》 与 《线性卷积》 之间的关系******************************
 *      * 1、在不对较长序列 x 补零的情况下：                                                                                                                                                  *
 * 		* 	如果序列x的长度是N1，序列w的长度是N2，则圆周卷积结果的末尾N1-N2+1个点的结果与线性卷积结果是一致的            *
 * 		*
 * 		* 2、在对序列x和序列w末尾都补零为相同长度L的情形下：                                                                                                                    *
 * 		* 	当两序列补零后的长度L>=len(x)+len(w)-1时，圆周卷积的前len(x)+len(w)-1个点就与线性卷积结果完全一致  *
 * 		*                                                                                        *
 * 		*	为了能够加速运算补零后序列的DFT结果，L就需要补零为2的整数次幂，就可以使用FFT                       *
 *		******************************************************************************************
 *
 * 		情况1：不对较长序列 x 做补零
 * 		----------------------
 * 		|         x		     |   ：输入长度为N1的较长序列x,(不对较长序列x做补零)
 * 		----------------------
 *      ----------------------
 * 		|      w	|    0   |   ：输入长度为N2的滤波器w,(末尾补零后长度为N1)
 * 		----------------------
 *      ----------------------
 * 		|    ......	|N1-N2+1 |	  ：圆周卷积结果的后N1-N2+1结果 与下面线性卷积结果的对应的那一小段结果一致
 * 		----------------------
 * 		                v
 * 						v（这一小段，圆周卷积的结果与线性卷积的结果是一样的）
 * 						v
 * 		-----------------------------------
 * 		|    ---	|N1-N2+1 |   ---      |  ：x与w的线性卷积的结果
 * 		-----------------------------------
 *
 *
 * 对于实时信号处理而言，输入信号 x 是一帧接着一帧的超长序列，且一帧信号的长度通常与FIR滤波器w的长度一致：len(x_old)=len(x_new)=len(w).
 * 对于上述的分段卷积情况，有两种快速卷积计算公式：
 *                                      overlap-add
 *                                      overlap-save
 *
 * 这两种计算方式本质上并没有区别，因为分段卷积的关键就是解决如何保留前一帧卷积后的状态寄存器
 *
 * 对于overlap-save方法：前后两帧一帧一起作为输入进行卷积运算，如下图所示：
 *
 * 		-------------------------
 * 		|   x_old   |   x_new   | :(不对输入序列[x_old,x_new]补零)
 * 		-------------------------
 *      -------------------------
 * 		|     w     |    0      |
 * 		-------------------------
 * 		-------------------------
 * 		|   ---     |   y_new   | :圆周卷积的后N点的结果刚好就是当前帧x_new与w的滤波结果！
 * 		-------------------------
 * 		                  v
 * 						  v（只有这一小段，圆周卷积的结果与线性卷积的结果是一样的）
 * 						  v
 *      -------------------------------------
 * 		|   y_old   |   y_new   |	----	| ：线性卷积结果
 * 		-------------------------------------
 *
 *
 *
 * 	但是，序列[x_old,x_new]的长度不是2的整数次幂时，也要在末尾补零，才能使用FFT以便快速计算嘛，不使用FFT，怎么叫快速运算。
 *
 *
 *     /-------------------/                                       /-------------------/
 *    /  x_old  x_new  0  / ---> FFT ---> ( 点乘 ) ---> IFFT --->  / 只取output[N:2N]   /
 *   /-------------------/				     ^                   /-------------------/
 *										     |        
 *										     |
 *     /-------------------/			     |
 *    /       w        0  / ---> FFT ---------
 *   /-------------------/
 *
 *
 *
 *
 * 总结(非常重要！！！)：
 * 		这种按帧处理的快速卷积运算，速度快、耗时短，但有一个缺点，就是非常的耗内存，爆堆的那种。
 *
 * 		对于DSP芯片的内存较小的情况，内存小，如果采用按采样点处理的方式,老老实实的运行乘加运算，内存消耗小，但是相应的运算量变多了。
 */
 
void fast_convolution_overlap_save(	float 	*input_old,
									float 	*input_new,
									float 	*weight,
									float 	*output,
									int 	input_num)
{
	float *input_cascade;
	float *weigth_zeropad; 
	float *output_cascade;
	
	complex_t *input_cascade_fft;
	complex_t *weigth_zeropad_fft; 
	complex_t *output_cascade_fft;
	
	int n, N_2pow, N = 2 * input_num;
	int i = 1, t = N;
	
	/* 
	 * cascede的长度为N = 2 * input_num，但是可能不是2的幂次方，计算大于N的最近的2的幂次方: N_2pow,以使用fft运算。
	 */
	do
	{
		t = t / 2;
		i = i + 1;		
	}while(1 != t);
	
	N_2pow = 1 << i;
	
	/*
	 * 注意：N_2pow太了会爆堆,也就是malloc开辟的空间超过了堆区的大小，程序会异常终止！！！
	 */
	input_cascade 	= (float *)malloc(N_2pow * sizeof(float));
	weigth_zeropad 	= (float *)malloc(N_2pow * sizeof(float));
	output_cascade 	= (float *)malloc(N_2pow * sizeof(float));
	
	input_cascade_fft 	= (complex_t *)malloc(N_2pow * sizeof(complex_t));
	weigth_zeropad_fft 	= (complex_t *)malloc(N_2pow * sizeof(complex_t));
	output_cascade_fft	= (complex_t *)malloc(N_2pow * sizeof(complex_t));

	/* 
	 * 将input_new和input_old拼接为一个整体的输入(input_old在前，input_new在后)，并在末尾补零为总长为 N_2pow,以便于fft运算。
	 */
	for(n = 0; n < N_2pow; n++)
	{
		if(n < N/2) 
			input_cascade[n] = input_old[n];
		else if(N/2 <= n < N) 
			input_cascade[n] = input_new[n - (N / 2)];
		else if(n < N_2pow)
			input_cascade[n] = 0;
	}
	
	/* 
	 * weight的末尾补零为N_2pow,以便于fft运算。
	 */ 
	for(n = 0; n < N_2pow; n++)
	{
		if(n < input_num) 
			weigth_zeropad[n] = weight[n];
		else 
			weigth_zeropad[n] = 0;
	} 
		
	/*
	 * 计算 input_cascade 与  weigth_zeropad 的fft 
	 */	
	fft_real(input_cascade, input_cascade_fft, N_2pow);
	fft_real(weigth_zeropad, weigth_zeropad_fft, N_2pow);

	/*
	 * 计算 input_cascade_fft 与 weigth_zeropad_fft 的乘积 out_cascade_fft
	 */
	
	for(n = 0; n < N_2pow; n++)
	{
		output_cascade_fft[n] = complex_product(input_cascade_fft[n], weigth_zeropad_fft[n]);
	} 
	
	/*
	 * 对output_cascade_fft做IFFT,并取output_cascade[n = input_num,...,2*input_num],
	 */
	ifft_real(output_cascade_fft, output_cascade, N_2pow);

	for(n = input_num; n < 2*input_num; n++)
	{
		output[n - input_num] = output_cascade[n];
	} 
	
	
	free(input_cascade);
	free(weigth_zeropad);
	free(output_cascade);
	
	free(input_cascade_fft);
	free(weigth_zeropad_fft);
	free(output_cascade_fft);
}

/*
 * overlap-add的原理就是分别计算每一个分段输入 x_i 与 w 的短序列卷积结果
 *
 * 其中前一个分段序列x的卷积结果的后len(w)点 与 后一个分段序列x的前len(w)在时间其实是重合的
 * 因此重合的这len(w)应该相加起来，才是该段时间内真是的输出值，如下图所示：
 *
 * 		--------------------------------------------------
 * 		|   x_1   |   x_2   |   x_3   |   x_4   |   x_4  | ...
 * 		--------------------------------------------------
 * 		-----     -----     -----     -----     -----
 *      | w |     | w |     | w |     | w |     | w | ...
 *      -----     -----     -----     -----     -----
 *                |   |     |   |     |   |     |   |
 *      ---------------     |   |     |   |     |   |
 *      |      y_1    |     |   |     |   |     |   |
 *      ---------------     |   |     |   |     |   |
 *                | ＋ |     |   |     |   |     |   |
 *                ---------------     |   |     |   |
 *                |      y_2    |     |   |     |   |
 *                ---------------     |   |     |   |
 *                          | + |     |   |     |   |
 *                          ---------------     |   |
 *                          |      y_3    |     |   |
 *                          ---------------     |   |
 *                                    | + |     |   |
 *                                    ---------------
 *                                    |      y_4    |
 *                                    ---------------
 *
 *      ---------------------------------------------
 *      |                   y                       |
 *      ---------------------------------------------
 *
 */

void fast_convolution_overlap_add(float 	*in_frame_ptr,
									float	*out_frame_ptr,
									int		frame_num,
									float 	*weight,
									int		weight_num)
{
	static float overlap_buff[weight_num - 1]; /* 重叠输出点 */
	float conv_buff[frame_num + weight_num - 1];
	/* 计算单帧的快速卷积 */
	fast_convolution(in_frame_ptr, frame_num, weight, weight_num, conv_buff);

	/* 当输出帧 = 上一针重叠点 + 本次卷积输出 */
	for(int i = 0; i < frame_num; i++)
		if(i <= weight_num - 1)
			out_frame_ptr[i] += conv_buff[i] + overlap_buff[i];
		else
			out_frame_ptr[i] = conv_buff[i];

	/* 更新重叠帧部分: 将单帧快速卷积的后 weight - 1 个点保留下来，因为这其实是下一帧卷积的状态变量 */
	memcpy(overlap_buff, conv_buff + weight, sizeof(float));
}

