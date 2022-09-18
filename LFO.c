#include "lfo.h"

lfo_t lfo_param;

/*
 *
 *       Max_delya_time * sin(2 * pi * lfo_freq * t)   ---> ��ɢ��
 *
 *       Max_delya_time * sin(2 * pi * lfo_freq * n / Fs)   ---> ��ɢ��
 *
 *       Max_delay_time *Fs * sin(2 * pi * lfo_freq * n / Fs)
 *
 *
 *
 */

void lfo_init(float lfo_freq, int sample_freq, float width_ms)
{
	lfo_param.phase = 0;
	lfo_param.phase_step = lfo_freq / sample_freq;
	lfo_param.width = sample_freq * width_ms; /* width: ��Ƶ�����İڷ��������Ӧ����ʵ���������ӳ�samples���� */
}

float lfo_tick(void)
{
	float tap;

	tap = lfo_param.width * sinf(2 * PI * lfo_param.phase);

	lfo_param.phase = lfo_param.phase + lfo_param.phase_step;
	if(lfo_param.phase > 1.0)
		lfo_param.phase = lfo_param.phase - 1.0;

	return tap;
}
