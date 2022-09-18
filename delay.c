#include "delay.h"

delay_t delay;

/*
 *  ѭ������ʵ��
 *
 *     --------------------------------------------------------------------------------------
 *     |                                                                                    /|\
 *     |         -------------------------------------------------------------------         |
 *     |         |	                                                               |         |
 *     |       \ |     |---------------|   |---------------|   |---------------|   |         |
 *     --------- | ... |    ...        |   |  last_sample  |   |   new_sample  |...| ---------
 *             / |     |---------------|   |---------------|   |---------------|   |
 *               |                                                                 |
 *               -------------------------------------------------------------------
 */

void delay_push_sampleBase(float in_data)
{
	delay.delay_buffer[delay.current_index] = in_data;

	delay.current_index = delay.current_index + 1;
	if(delay.current_index > MAX_BUFFER_NUM)
		delay.current_index = delay.current_index - MAX_BUFFER_NUM;
}
/*
 *  ���ݻ��ݣ�ֻ��ȡ��delay_buffer�е����ݣ�����push�µ�����
 */
float delay_goback_sampleBase(	int goback_index)
{
	int out_index = delay.current_index - goback_index;
	if(out_index < 0)
		out_index = MAX_BUFFER_NUM + out_index;

	return delay.delay_buffer[out_index];
}

void delay_blockBase(	float 	*in_frame_ptr,
						float 	*out_frame_ptr,
						int 	frame_len,
						int		Fs,
						int 	delay_time_ms)
{

	/* push new_frame */
	if(MAX_BUFFER_NUM - delay.current_index >= frame_len)
	{
		memcpy(delay.delay_buffer + delay.current_index, in_frame_ptr, frame_len * sizeof(float));
		delay.current_index = delay.current_index + frame_len;
	}
	else if(MAX_BUFFER_NUM - delay.current_index < frame_len)
	{
		memcpy(delay.delay_buffer + delay.current_index, in_frame_ptr, (MAX_BUFFER_NUM - delay.current_index) * sizeof(float));
		memcpy(delay.delay_buffer , in_frame_ptr + (MAX_BUFFER_NUM - delay.current_index), (frame_len - (MAX_BUFFER_NUM - delay.current_index)) * sizeof(float));
		delay.current_index = frame_len - (MAX_BUFFER_NUM - delay.current_index);
	}

	/* goback old_frame */
	int goback_index;
	if((delay.current_index - frame_len - Fs * delay_time_ms / 1000 ) < 0)
	{
		goback_index = delay.current_index - frame_len - Fs * delay_time_ms / 1000 + MAX_BUFFER_NUM;
		if(goback_index + frame_len <= MAX_BUFFER_NUM)
		{
			memcpy(out_frame_ptr, &delay.delay_buffer[goback_index], frame_len * sizeof(float));
		}
		else
		{
			memcpy(out_frame_ptr, &delay.delay_buffer[goback_index], (MAX_BUFFER_NUM - goback_index) * sizeof(float));
			memcpy(out_frame_ptr + (MAX_BUFFER_NUM - goback_index), delay.delay_buffer, (frame_len - (MAX_BUFFER_NUM - goback_index)) * sizeof(float));
		}
	}
	else if((delay.current_index - frame_len - Fs * delay_time_ms / 1000 ) >= 0)
	{
		goback_index = delay.current_index - frame_len - Fs * delay_time_ms / 1000;
		memcpy(out_frame_ptr, delay.delay_buffer + goback_index, frame_len * sizeof(float));
	}
}

/*
 *   ʵ���ӳٵ�ԭ�������һ��������Ϊ����������¼��ǰһ�ε�����ֵ��
 *   ���µ�һ֡�������ݴ������β�����룬����������׶˵����ݣ�Ҳ���ǻ������ܼ�¼����������ݣ�
 *
 *   	�ӳ����֡����� = MAX_BUFFER_NUM - (delay_time * Fs + frame_len)
 *
 *   ����ͼ��ʾ�������ȳ��Ķ���ʵ��
 *
 *      /          ----------------------------------------------------------------     /
 *     /           |	                                                          |    /
 *    /------------|     |---------------|   |---------------|   |---------------||   /----------
 *   / frame_data  | ... |    ...        |   |    last_frame |   |   new_frame   ||  / frame_data
 *   \     out     |     |---------------|   |---------------|   |---------------||  \     in
 *    \------------|                                             |                |   \----------
 *     \           ----------------------------------------------|-----------------    \
 *                                                               |
 *                                     |                         |
 *                                     |<--- delay_time * Fs --->|
 *                                     |                         |
 *
 *                                     ^
 *                                     ||---------------|
 *                                     ||delay_out_frame|
 *                                     ||---------------|
 *                                     |
 *
 *   ���Կ���:
 *   	����ӳ�ʱ��  = (���������� - ֡��) / Fs
 *
 */

void delay_process(		float 	*in_frame_ptr,
						float 	*out_frame_ptr,
						int 	frame_len,
						int		Fs,
						int 	delay_time_ms)
{
	/* ����ǰ��λ(memmove)�ڳ���һ֡�ռ� */
	memmove(delay.delay_buffer, delay.delay_buffer + frame_len, (MAX_BUFFER_NUM - frame_len) * sizeof(float));
	/* �µ�һ֡ѹ��(push)������ */
	memcpy(delay.delay_buffer+(MAX_BUFFER_NUM - frame_len), in_frame_ptr,frame_len * sizeof(float));
	/* �����ӳ�ʱ��������ָ��λ�� */
	if((Fs * delay_time_ms / 1000) > (MAX_BUFFER_NUM - frame_len)) /* �����ӳٻ�����֧�ֵ�����ӳ�ʱ�䣬�����ӳٴ��� */
		out_frame_ptr = in_frame_ptr;
	else
		memcpy(out_frame_ptr, delay.delay_buffer + MAX_BUFFER_NUM - frame_len-(int)(Fs * delay_time_ms / 1000), frame_len * sizeof(float));
}
