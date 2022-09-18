#ifndef SOURCE_DRC_H_
#define SOURCE_DRC_H_

#include <math.h>

/* Ñ¹ËõÆ÷ */
void compressor(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* À©Õ¹Æ÷ */
void expander(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* ÏŞÖÆÆ÷ */
void lms_limiter(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
void peak_limiter(float * in_frame_ptr, float *out_frame_ptr, int frame_num);
/* ÔëÉùÃÅ */
void noise_gate(float * in_frame_ptr, float *out_frame_ptr, int frame_num);

#endif /* SOURCE_DRC_H_ */
