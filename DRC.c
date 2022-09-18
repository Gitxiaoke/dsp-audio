#include "DRC.h"

/*
 * noise gate
 *
 *   x[n]                                                                                  y[n]
 * -------*--------------------------------------------------------------------------->(+)------>
 *        |                                                                             ^
 *        |              _________________________                                      |
 *        v     x_a[n]   |                        |    g_c[n]                    g_s[n] |
 *      (abs)------------|(static characteristic) |----------->(gain smoothing)----------
 *                       |                        |
 *                       |
 *
 *
 *
 *
 *		TA is the attack time period,  specified by the AttackTime property.
 *		TR is the release time period, specified by the ReleaseTime property.
 *		Fs is the input sampling rate, specified by the SampleRate property.
 *
 *		CA is the hold counter for attack.
 *		TH is determined by the HoldTime property.
 *
 */

void noise_gate(	float *in_frame_ptr,
					float *out_frame_ptr,
					int frame_num,
					int Fs,
					float threshold_dB,
					float attack_time_ms,
					float attack_counter,
					float release_time_ms,
					float hold_time)
{
	float g_c[frame_num], g_s[frame_num];

	float alpha_A, alpha_R;

	float threshold;

	threshold = pow(10, threshold_dB / 20.);

//	alpha_A = exp(-1 * log(9) / (Fs * attack_time_ms / 1000));
//	alpha_R = exp(-1 * log(9) / (Fs * release_time_ms / 1000));

//	log(9) = 2.19722457734;

	alpha_A = expf(-2197.22457734 / (Fs * attack_time_ms));
	alpha_R = expf(-2197.22457734 / (Fs * release_time_ms));

	/* [1] Convert Input Signal to Magnitude */
	for(int i = 0; i < frame_num; i++)
		in_frame_ptr[i] = (in_frame_ptr[i] < 0) ? (0-in_frame_ptr[i]) : in_frame_ptr[i];

	/* [2] Gain Computer*/
	for(int i = 0; i < frame_num; i++)
		g_c[i] = (in_frame_ptr[i] < threshold) ? 0 : 1;

	/* [3] Gain Smoothing */
	for(int i = 1; i < frame_num; i++)
	{
		if(g_c[i] <= g_s[i - 1])
		{
			if(attack_counter > hold_time)
				g_s[i] = alpha_A * g_s[i-1] + (1 - alpha_A) * g_c[i];   /* 错了，matlab那里的g_s[n]与g_s[n-1]指的是前后时刻！ */
			else
				g_s[i] = g_s[i-1];
		}
		else if(g_c[i] > g_s[i - 1])
			{
			g_s[i] = alpha_R * g_s[i - 1] + (1 - alpha_R) * g_c[i];
			attack_counter = 0.0f;
			}

		out_frame_ptr[i] = in_frame_ptr[i] * g_s[i];
	}


}
