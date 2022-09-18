/*****************************************************************************
 * DSP.c
 *****************************************************************************/

#include <sys/platform.h>
#include "adi_initialize.h"

#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

#include "fast_convolution.h"
#define FrameNum 160	/* 这个值太大了容易在fast_convolution函数里面爆堆 */

float test_a = 1.0;  /* 用于测试的全局变量，看test_a的地址是位于堆区还是栈区:结果是在栈区 */
float test_b = 2.0;  /* 只有malloc申请的内存才是在堆区 */

int main(int argc, char *argv[])
{

	int i;
	
	float Fs = 640;

//	for(i = 0; i < FrameNum; i++)
//	{
//		input[i] = 	10 + \
//					2*cos(2*PI*10*i/Fs + PI/3) + \
//					2*cos(2*PI*30*i/Fs - PI/3) + \
//					2*cos(2*PI*70*i/Fs + PI/3);
////		input[i] =   + 1;
//		printf("input %f \n", input[i]);
//	}
//	printf("------------------input-----------------\n");

//	dft(input, output, FrameNum);
//	for (i = 0; i < FrameNum; i++)
//	{
//		printf("%f + %f\n", output[i].real, output[i].imag);
//	}
//	printf("------------------dft-out-----------------\n");
//
//	idft(output, input, FrameNum);
//	for (i = 0; i < FrameNum; i++)
//	{
//		printf("%f \n", input[i]);
//	}
//	printf("------------------idft-out-----------------\n");
//
//	fft_real(input, output, FrameNum);
//	for(i = 0; i < FrameNum; i++)
//	{
//		printf("%f + %f\n", output[i].real, output[i].imag);
//	}
//	printf("------------------fft-----------------\n");
//
//	ifft_real(output, input, FrameNum);
//	for(i = 0; i < FrameNum; i++)
//	{
//		printf("-ifft %f \n", input[i]);
//	}
//	printf("------------------ifft-----------------\n");


//	dct(input, dct_output, FrameNum);
//	for(i = 0; i < FrameNum; i++)
//	{
//		printf("dct-out %f \n", dct_output[i]);
//	}
//	printf("------------------dct-output-----------------\n");
//
//
//	windows_initial(win_buf, FrameNum, Rectangle);
//	for(i = 0; i < FrameNum; i++)
//	{
//		printf("%d Rectangle %f \n", i, win_buf[i]);
//	}
//	printf("------------------Rectangle-windows-----------------\n");


//	float input_new[FrameNum];
//	float input_old[FrameNum];
//	float weight[FrameNum];
//	float conv_output[FrameNum];
//	for(i = 0; i < FrameNum; i++)
//	{
//		input_new[i] = 1;
//		input_old[i] = 2;
//		weight[i] = 3;
//	}
//
//
//	fast_convolution_overlapSave(input_old, input_new, weight, conv_output, FrameNum);
//	for(i = 0; i < FrameNum; i++)
//	{
//		printf("fast-Conv-out %d : %f \n", i, conv_output[i]);
//	}
//	printf("------------------fast-convolution-----------------\n");

	char *wav_file = "C:\\Users\\11451\\Desktop\\dev++\\wav_read_test\\leave.wav";
	wav_print_head(wav_file);

	return 0;
}

