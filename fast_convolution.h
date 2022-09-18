#ifndef _FAST_CONVOLUTION_
#define _FAST_CONVOLUTION_

#include "fft_real.h"

#ifndef PI
	#define PI 3.141592653589793238462
#endif 

/* ������ x ������� w �Ŀ��پ�� */
void fast_convolution(float *input,int input_len, float *weight, int w_len, float *output);

/* ������ x ������� w �ķֶο��پ�� */
void fast_convolution_overlap_save(	float 	*input_old,
										float 	*input_new,
										float 	*weight,
										float 	*output,
										int 	frame_num);

void fast_convolution_overlap_add(	float 	*in_frame_ptr,
										float	*out_frame_ptr,
										int		frame_num,
										float 	*weight,
										int		weight_num);
/* ������ x �� ������ w �ķֶο��پ�� */
void fast_convolution_partitioned_fast_block(	float 	*in_frame_ptr,
													float 	*out_frame_ptr,
													int 	frame_num,
													float 	*weight_ptr,
													int 	weight_num,
													int 	block_size);

#endif
