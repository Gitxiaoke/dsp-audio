#ifndef FADE_IN_OUT_H_
#define FADE_IN_OUT_H_

typedef enum{
	UN_MUTE = 0,
	MUTE
}mute_t;

void fade_process(	float *in_frame_ptr,
					float *out_frame_ptr,
					mute_t mute_type);

#endif /* FADE_IN_OUT_H_ */
