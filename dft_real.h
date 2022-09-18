#ifndef _DFT_REAL_
#define _DFT_REAL_

#include <math.h>

#ifndef PI
	#define PI 3.141592653589793238
#endif

#ifndef COMPLEX_T
#define COMPLEX_T
	typedef struct
	{
		float real;
		float imag;
	}complex_t;
#endif

void dft(float *input, complex_t *output, int inputNum);
void idft(complex_t *input, float *output, int inputNum); 
#endif

