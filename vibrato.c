#include "vibrato.h"
/*
 * vibratio（颤音）的原理：
 *
 *
 *     in_samples   |------------|   out_samples
 *   -------------> |delay_buffer|--------------->
 *                  |------------|
 *                         ^
 *                         |
 *                         |  由LFO低频振荡器产生一个时变的周期延迟
 *                         |
 *                       (LFO)
 */

void vibrato(	float 	*in_frame_ptr,
				float 	*out_frame_ptr,
				int 	frame_num,
				int 	Fs,
				float 	avg_delay_ms,
				float 	lfo_width_ms)
{
	float delay;
	int delay_int;
	float frac;

	float candidate_1, candidate_2;

	if (avg_delay_ms < lfo_width_ms)
		avg_delay_ms = lfo_width_ms;

	for(int n = 0; n < frame_num; n++)
	{
		delay = avg_delay_ms * Fs + lfo_tick();  /* 延迟点数：如果没有落在整数点上，则取前后两个点的均值 */
		delay_int = (int) delay;

		frac = delay - delay_int;

		candidate_1 = delay_goback_sampleBase(delay_int); 		/* 非帧数延迟点对应的左整数延迟点 */
		candidate_2 = delay_goback_sampleBase(delay_int + 1); 	/* 非帧数延迟点对应的右整数延迟点 */

		out_frame_ptr[n] = frac * candidate_1 + (1 - frac) * candidate_2;

		delay_push_sampleBase(in_frame_ptr[n]);
	}
}
