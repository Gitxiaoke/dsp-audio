#include "vibrato.h"
/*
 * vibratio����������ԭ��
 *
 *
 *     in_samples   |------------|   out_samples
 *   -------------> |delay_buffer|--------------->
 *                  |------------|
 *                         ^
 *                         |
 *                         |  ��LFO��Ƶ��������һ��ʱ��������ӳ�
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
		delay = avg_delay_ms * Fs + lfo_tick();  /* �ӳٵ��������û�������������ϣ���ȡǰ��������ľ�ֵ */
		delay_int = (int) delay;

		frac = delay - delay_int;

		candidate_1 = delay_goback_sampleBase(delay_int); 		/* ��֡���ӳٵ��Ӧ���������ӳٵ� */
		candidate_2 = delay_goback_sampleBase(delay_int + 1); 	/* ��֡���ӳٵ��Ӧ���������ӳٵ� */

		out_frame_ptr[n] = frac * candidate_1 + (1 - frac) * candidate_2;

		delay_push_sampleBase(in_frame_ptr[n]);
	}
}
