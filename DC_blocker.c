#include "DC_blocker.h"

/*
 * DC blocker filter formula:
 *
 * 		y(n) = x(n) - x(n-1) + R * y(n-1)
 *
 * 		where:	n = current in/out value
 * 				(n-1) = previous in/out value
 *
 *				(-3dB @ 40Hz): R = 1-(250/samplerate)
 *				(-3dB @ 30Hz): R = 1-(190/samplerate)
 * 				(-3dB @ 20Hz): R = 1-(126/samplerate)
 */

void DC_blocker(	float	*in_frame_ptr,
					float	*out_frame_ptr,
					int 	frame_num,
					float 	Fs)
{
	static float R = 1 - (126 / Fs);
	static float x_status = 0, y_status = 0;

	for(int n = 0; n < frame_num; n++)
	{
		out_frame_ptr[n] = in_frame_ptr[n] - x_status + R * y_status;

		x_status = in_frame_ptr[n];
		y_status = out_frame_ptr[n];
	}
}
