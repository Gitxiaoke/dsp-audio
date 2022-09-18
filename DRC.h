#ifndef SOURCE_DRC_H_
#define SOURCE_DRC_H_

#include <math.h>

/* ѹ���� */
void compressor(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* ��չ�� */
void expander(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* ������ */
void lms_limiter(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
void peak_limiter(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* ������ */
void noise_gate(float * in_frame_ptr, float *out_frame_ptr, int frame_num);

#endif /* SOURCE_DRC_H_ */
