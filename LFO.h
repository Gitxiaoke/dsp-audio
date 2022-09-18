#ifndef SOURCE_LFO_H_
#define SOURCE_LFO_H_

#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct
{
	float width;
	float phase;
	float phase_step;
}lfo_t;



void lfo_init(float lfo_freq, int sample_freq, float width_ms);
float lfo_tick(void);

#endif /* SOURCE_LFO_H_ */
