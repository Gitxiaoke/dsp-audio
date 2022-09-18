#include "delay.h"

delay_t delay;

/*
 *  循环缓冲实现
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
 *  数据回溯：只是取出delay_buffer中的数据，并不push新的数据
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
 *   实现延迟的原理就是用一个数组作为缓冲区，记录此前一段的输入值，
 *   最新的一帧输入数据从数组的尾端输入，挤出数组的首端的数据（也就是缓冲区能记录的最早的数据）
 *
 *   	延迟输出帧的起点 = MAX_BUFFER_NUM - (delay_time * Fs + frame_len)
 *
 *   如下图所示：先入先出的队列实现
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
 *   可以看出:
 *   	最大延迟时间  = (缓冲区长度 - 帧长) / Fs
 *
 */

void delay_process(		float 	*in_frame_ptr,
						float 	*out_frame_ptr,
						int 	frame_len,
						int		Fs,
						int 	delay_time_ms)
{
	/* 先向前移位(memmove)腾出第一帧空间 */
	memmove(delay.delay_buffer, delay.delay_buffer + frame_len, (MAX_BUFFER_NUM - frame_len) * sizeof(float));
	/* 新的一帧压入(push)缓冲区 */
	memcpy(delay.delay_buffer+(MAX_BUFFER_NUM - frame_len), in_frame_ptr,frame_len * sizeof(float));
	/* 根据延迟时间觉得输出指针位置 */
	if((Fs * delay_time_ms / 1000) > (MAX_BUFFER_NUM - frame_len)) /* 超过延迟缓冲区支持的最大延迟时间，不做延迟处理 */
		out_frame_ptr = in_frame_ptr;
	else
		memcpy(out_frame_ptr, delay.delay_buffer + MAX_BUFFER_NUM - frame_len-(int)(Fs * delay_time_ms / 1000), frame_len * sizeof(float));
}
