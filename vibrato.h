#ifndef SOURCE_VIBRATO_H_
#define SOURCE_VIBRATO_H_

#include "LFO.h"
#include "delay.h"

void vibrato(	float 	*in_frame_ptr,
				float 	*out_frame_ptr,
				int 	frame_num,
				int 	Fs,
				float 	avg_delay_ms,
				float 	lfo_width_ms);

#endif /* SOURCE_VIBRATO_H_ */
