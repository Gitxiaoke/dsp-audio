#include "windows_function.h"

/*
 *
 * enum windows_type{
 *		Rectangle = 0,
 *		Hanning,
 *		Hamming,
 *		Blackman
 * };
 *
 */
void windows_initial(float *win_buf, int windows_length, windows_type_t windows_type)
{
	int N = windows_length;
	int n;
	switch (windows_type){
		case Rectangle:{
			for(n = 0; n <= N - 1; n++) win_buf[n] = 1;
			break;		
		}
		case Hanning:{
			for(n = 0; n <= N - 1; n++) win_buf[n] = 0.5 - 0.5 * cos(2.*PI*n/(N-1));
			break;	
		}
		case Hamming:{
			for(n = 0; n <= N - 1; n++) win_buf[n] = 0.54 - 0.46 * cos(2.*PI*n/(N-1));
			break;
		}
		case Blackman:{
			for(n = 0; n <= N - 1; n++) win_buf[n] = 0.42 - 0.5 * cos(2.*PI*n/(N-1)) + 0.08 * cos(4*PI*n/(N-1)); 
			break;
		}
	}
}
/*
 * 加一帧信号乘上窗函数 
 */
void add_windows(float *input, float *win_buf, int input_num)
{
	int n, N = input_num;
	for(n = 0; n < N; n++)
	{
		input[n] = input[n] * win_buf[n];
	}
}
