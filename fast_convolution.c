#include "fast_convolution.h"

#include <stdlib.h>
#include <math.h>
/*--------------------------1�������еĿ��پ�������ÿ���״̬�Ĵ�����-----------------------------------------
 *
 *
 * 		ʱ���������У��϶̵�ĩβ��Ҫ������ϳ����еĳ���һ�£���Բ�ܾ�� = ������ DFT(FFT) �����ĳ˻�
 *
 *      ******************************��Բ�ܾ���� �� �����Ծ���� ֮��Ĺ�ϵ******************************
 *      * 1���ڲ��Խϳ����� x ���������£�                                                                                                                                                  *
 * 		* 	�������x�ĳ�����N1������w�ĳ�����N2����Բ�ܾ�������ĩβN1-N2+1����Ľ�������Ծ�������һ�µ�            *
 * 		*
 * 		* 2���ڶ�����x������wĩβ������Ϊ��ͬ����L�������£�                                                                                                                    *
 * 		* 	�������в����ĳ���L>=len(x)+len(w)-1ʱ��Բ�ܾ����ǰlen(x)+len(w)-1����������Ծ�������ȫһ��  *
 * 		*                                                                                        *
 * 		*	Ϊ���ܹ��������㲹������е�DFT�����L����Ҫ����Ϊ2���������ݣ��Ϳ���ʹ��FFT                       *
 *		******************************************************************************************
 *
 *   ʹ�� DFT(FFT) ������������ x(n),n=[0,1,2,...,N-1] �� w(n),n=[0,1,2,...,M-1]�����Ծ������:
 *
 *		0�����㲹�㳤��, L >= N+M-1, �� L = 2����������
 *		1������������ĩβ����Ϊ����L
 *		2���Բ������ź���fft
 *	    3������������fft�Ľ�����е��
 *		4���Ե�˵Ľ����ifft
 *		5��ifft�����ǰ��N+M-1����������Ծ���Ľ������ʵ����Ľ�������㡣
 *
 *     /-------------------/                                       /---------------------/
 *    /       x   0(����)  / ---> FFT ---> ( ��� ) ---> IFFT --->  /  ȡoutput[0:N+M-1]   /
 *   /-------------------/				     ^                   /---------------------/
 *										     |
 *										     |
 *     /-------------------/			     |
 *    /       w   0(����)  / ---> FFT ---------
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
	 * ������ڻ����N��2��������������N_2pow
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
	 * ��input��weightĩβ����Ϊ����N_2pow
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
	 * ����input_zeropad �� weight_zeropad��FFT
	 */

	fft_real(input_zeropad, input_zeropad_fft, N_2pow);
	fft_real(weight_zeropad, weight_zeropad_fft, N_2pow);
	/*
	 * input_zeropad_fft ��� weight_zeropad_fft
	 */

	for(i = 0; i < N_2pow; i++)
	{
		output_cascade_fft[i] = complex_product(input_zeropad_fft[i], weight_zeropad_fft[i]);
	}

	/*
	 * ��output_cascade_fft��ifft
	 */
	ifft_real(output_cascade_fft, input_zeropad, N_2pow);
	/*
	 * ȡifft�����ǰlen(x) + len(w) - 1�������Ծ���Ľ��
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
/*--------------------------2�������еĿ��پ������Ҫ����״̬�Ĵ�����-----------------------------------------
 *
 *
 *      ******************************��Բ�ܾ���� �� �����Ծ���� ֮��Ĺ�ϵ******************************
 *      * 1���ڲ��Խϳ����� x ���������£�                                                                                                                                                  *
 * 		* 	�������x�ĳ�����N1������w�ĳ�����N2����Բ�ܾ�������ĩβN1-N2+1����Ľ�������Ծ�������һ�µ�            *
 * 		*
 * 		* 2���ڶ�����x������wĩβ������Ϊ��ͬ����L�������£�                                                                                                                    *
 * 		* 	�������в����ĳ���L>=len(x)+len(w)-1ʱ��Բ�ܾ����ǰlen(x)+len(w)-1����������Ծ�������ȫһ��  *
 * 		*                                                                                        *
 * 		*	Ϊ���ܹ��������㲹������е�DFT�����L����Ҫ����Ϊ2���������ݣ��Ϳ���ʹ��FFT                       *
 *		******************************************************************************************
 *
 * 		���1�����Խϳ����� x ������
 * 		----------------------
 * 		|         x		     |   �����볤��ΪN1�Ľϳ�����x,(���Խϳ�����x������)
 * 		----------------------
 *      ----------------------
 * 		|      w	|    0   |   �����볤��ΪN2���˲���w,(ĩβ����󳤶�ΪN1)
 * 		----------------------
 *      ----------------------
 * 		|    ......	|N1-N2+1 |	  ��Բ�ܾ������ĺ�N1-N2+1��� ���������Ծ������Ķ�Ӧ����һС�ν��һ��
 * 		----------------------
 * 		                v
 * 						v����һС�Σ�Բ�ܾ���Ľ�������Ծ���Ľ����һ���ģ�
 * 						v
 * 		-----------------------------------
 * 		|    ---	|N1-N2+1 |   ---      |  ��x��w�����Ծ���Ľ��
 * 		-----------------------------------
 *
 *
 * ����ʵʱ�źŴ�����ԣ������ź� x ��һ֡����һ֡�ĳ������У���һ֡�źŵĳ���ͨ����FIR�˲���w�ĳ���һ�£�len(x_old)=len(x_new)=len(w).
 * ���������ķֶξ������������ֿ��پ�����㹫ʽ��
 *                                      overlap-add
 *                                      overlap-save
 *
 * �����ּ��㷽ʽ�����ϲ�û��������Ϊ�ֶξ���Ĺؼ����ǽ����α���ǰһ֡������״̬�Ĵ���
 *
 * ����overlap-save������ǰ����֡һ֡һ����Ϊ������о�����㣬����ͼ��ʾ��
 *
 * 		-------------------------
 * 		|   x_old   |   x_new   | :(������������[x_old,x_new]����)
 * 		-------------------------
 *      -------------------------
 * 		|     w     |    0      |
 * 		-------------------------
 * 		-------------------------
 * 		|   ---     |   y_new   | :Բ�ܾ���ĺ�N��Ľ���պþ��ǵ�ǰ֡x_new��w���˲������
 * 		-------------------------
 * 		                  v
 * 						  v��ֻ����һС�Σ�Բ�ܾ���Ľ�������Ծ���Ľ����һ���ģ�
 * 						  v
 *      -------------------------------------
 * 		|   y_old   |   y_new   |	----	| �����Ծ�����
 * 		-------------------------------------
 *
 *
 *
 * 	���ǣ�����[x_old,x_new]�ĳ��Ȳ���2����������ʱ��ҲҪ��ĩβ���㣬����ʹ��FFT�Ա���ټ������ʹ��FFT����ô�п������㡣
 *
 *
 *     /-------------------/                                       /-------------------/
 *    /  x_old  x_new  0  / ---> FFT ---> ( ��� ) ---> IFFT --->  / ֻȡoutput[N:2N]   /
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
 * �ܽ�(�ǳ���Ҫ������)��
 * 		���ְ�֡����Ŀ��پ�����㣬�ٶȿ졢��ʱ�̣�����һ��ȱ�㣬���Ƿǳ��ĺ��ڴ棬���ѵ����֡�
 *
 * 		����DSPоƬ���ڴ��С��������ڴ�С��������ð������㴦��ķ�ʽ,����ʵʵ�����г˼����㣬�ڴ�����С��������Ӧ������������ˡ�
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
	 * cascede�ĳ���ΪN = 2 * input_num�����ǿ��ܲ���2���ݴη����������N�������2���ݴη�: N_2pow,��ʹ��fft���㡣
	 */
	do
	{
		t = t / 2;
		i = i + 1;		
	}while(1 != t);
	
	N_2pow = 1 << i;
	
	/*
	 * ע�⣺N_2pow̫�˻ᱬ��,Ҳ����malloc���ٵĿռ䳬���˶����Ĵ�С��������쳣��ֹ������
	 */
	input_cascade 	= (float *)malloc(N_2pow * sizeof(float));
	weigth_zeropad 	= (float *)malloc(N_2pow * sizeof(float));
	output_cascade 	= (float *)malloc(N_2pow * sizeof(float));
	
	input_cascade_fft 	= (complex_t *)malloc(N_2pow * sizeof(complex_t));
	weigth_zeropad_fft 	= (complex_t *)malloc(N_2pow * sizeof(complex_t));
	output_cascade_fft	= (complex_t *)malloc(N_2pow * sizeof(complex_t));

	/* 
	 * ��input_new��input_oldƴ��Ϊһ�����������(input_old��ǰ��input_new�ں�)������ĩβ����Ϊ�ܳ�Ϊ N_2pow,�Ա���fft���㡣
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
	 * weight��ĩβ����ΪN_2pow,�Ա���fft���㡣
	 */ 
	for(n = 0; n < N_2pow; n++)
	{
		if(n < input_num) 
			weigth_zeropad[n] = weight[n];
		else 
			weigth_zeropad[n] = 0;
	} 
		
	/*
	 * ���� input_cascade ��  weigth_zeropad ��fft 
	 */	
	fft_real(input_cascade, input_cascade_fft, N_2pow);
	fft_real(weigth_zeropad, weigth_zeropad_fft, N_2pow);

	/*
	 * ���� input_cascade_fft �� weigth_zeropad_fft �ĳ˻� out_cascade_fft
	 */
	
	for(n = 0; n < N_2pow; n++)
	{
		output_cascade_fft[n] = complex_product(input_cascade_fft[n], weigth_zeropad_fft[n]);
	} 
	
	/*
	 * ��output_cascade_fft��IFFT,��ȡoutput_cascade[n = input_num,...,2*input_num],
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
 * overlap-add��ԭ����Ƿֱ����ÿһ���ֶ����� x_i �� w �Ķ����о�����
 *
 * ����ǰһ���ֶ�����x�ľ������ĺ�len(w)�� �� ��һ���ֶ�����x��ǰlen(w)��ʱ����ʵ���غϵ�
 * ����غϵ���len(w)Ӧ��������������Ǹö�ʱ�������ǵ����ֵ������ͼ��ʾ��
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
 *                | �� |     |   |     |   |     |   |
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
	static float overlap_buff[weight_num - 1]; /* �ص������ */
	float conv_buff[frame_num + weight_num - 1];
	/* ���㵥֡�Ŀ��پ�� */
	fast_convolution(in_frame_ptr, frame_num, weight, weight_num, conv_buff);

	/* �����֡ = ��һ���ص��� + ���ξ����� */
	for(int i = 0; i < frame_num; i++)
		if(i <= weight_num - 1)
			out_frame_ptr[i] += conv_buff[i] + overlap_buff[i];
		else
			out_frame_ptr[i] = conv_buff[i];

	/* �����ص�֡����: ����֡���پ���ĺ� weight - 1 ���㱣����������Ϊ����ʵ����һ֡�����״̬���� */
	memcpy(overlap_buff, conv_buff + weight, sizeof(float));
}

