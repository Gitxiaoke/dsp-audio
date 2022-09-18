#include "dct.h"

/*
 * DCT formula:
 *		X(k) = a(k) * SUM~n=[0,1,2,...,N-1] x(n)*cos(pi*(2n+1)k/(2N)) ,k=[0,1,2,...,N-1]
 * 			
 *			   a(k) = sqrt(1/N), for k = 0;
 *			   a(k) = sqrt(2/N), for k = [1,2,...,N-1] 
 *
 */

void dct(float *input, float *output, int inputNum)
{
	int N = inputNum;
	int n,k;
	for(k = 0; k < N; k++)
	{
		output[k] = 0;
		for(n = 0; n < N; n++)
		{
			output[k] += input[n] * cos(PI*(n+0.5)*k/N);
		}
		
		if(0 == k)
		{ 
			output[k] = sqrt(1. / N) * output[k]; 
		}
		else
		{
			output[k] = sqrt(2. / N) * output[k]; 
		} 
	}
}
