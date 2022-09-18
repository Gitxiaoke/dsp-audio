#ifndef _AUDIO_SPECTRAL_FEATURE_EXTRACTION_H_
#define _AUDIO_SPECTRAL_FEATURE_EXTRACTION_H_

#include "fft_real.h"
#include <math.h>


void Hz_to_Mel(float Hz_freq , float Mel_freq);
void Mel_to_Hz(float Mel_freq, float Hz_freq);


#endif

