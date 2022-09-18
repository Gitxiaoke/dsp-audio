#ifndef DELAY_H_
#define DELAY_H_

#include <string.h>

/*
 * �ӳ�ͨ����Ϊ1��
 * �ӳٻ���������Ϊ:MAX_BUFFER_NUM.
 * ������֧�ֵ�����ӳ�ʱ�� = (MAX_BUFFER_NUM - (frame_len)) / Fs
 */
#define MAX_BUFFER_NUM (2560*10)

typedef struct{
	int current_index;
	float delay_buffer[MAX_BUFFER_NUM];
}delay_t;


void delay_process(		float 	*in_frame_ptr,
						float 	*out_frame_ptr,
						int 	frame_len,
						int		Fs,
						int 	delay_time);

void delay_push_sampleBase(float in_data);

float delay_goback_sampleBase(	int goback_index);

void delay_blockBase(	float 	*in_frame_ptr,
						float 	*out_frame_ptr,
						int 	frame_len,
						int		Fs,
						int 	delay_time_ms);

#endif /* DELAY_H_ */
