#ifndef _WINDOWS_FUNCTION_
#define _WINDOWS_FUNCTION_

#include <math.h>


#ifndef PI
	#define PI 3.141592653589793238
#endif

typedef enum {
	Rectangle = 0,
	Hanning,
	Hamming,
	Blackman
}windows_type_t;

void windows_initial(float *win_buf, int windows_length, windows_type_t windows_type);
void add_windows(float *input, float *win_buf, int input_num);

#endif
