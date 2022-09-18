#ifndef _FFT_REAL_
#define _FFT_REAL_

#include<math.h>

#ifndef PI
	#define PI 3.141592653589793238462
#endif 

#ifndef COMPLEX_T
#define COMPLEX_T
	typedef struct
	{
		float real;
		float imag;
	}complex_t;
#endif

complex_t complex_product(complex_t a, complex_t b);

void fft_real(float *input, complex_t *output, int input_num);
void ifft_real(complex_t *input, float *output, int input_num);

void spectrum(float *input, int input_num, float *output_spectrum);


#endif
