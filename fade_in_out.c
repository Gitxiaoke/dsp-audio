#include "fade_in_out.h"

void fade_process(	float 	*in_frame_ptr,
					float 	*out_frame_ptr,
					int 	frame_len,
					int		Fs,
					int		fade_time_ms,
					mute_t 	mute_type)
{
	static int fade_length = Fs * fade_time_ms;
	static int fade_in_count, fade_out_count;
	static int fade_in_end = 0, fade_out_end = 0;

	if(MUTE == mute_type)
	{
		for(int n = 0; n < frame_len; n++)
		{
			out_frame_ptr[n] = in_frame_ptr[n] * (fade_length - fade_out_count) / fade_length;
			if(fade_out_count < fade_length)
				fade_out_count = fade_out_count + 1;
			else
				fade_out_end = 1;
		}
	}
	else if(UN_MUTE == mute_type)
	{
		if(fade_in_end != 1)
		{
			for(int n = 0; n < frame_len; n++)
			{
				if(fade_in_count < fade_length)
				{
					out_frame_ptr[n] = in_frame_ptr[n] * (fade_in_count / fade_length);
					fade_in_count = fade_in_count + 1;
				}
				else
				{
					out_frame_ptr[n] = in_frame_ptr[n];
					fade_in_end = 1;
				}
			}
		}
		else
		{
			for(int n = 0; n < frame_len; n++)
			{
				if(fade_in_count < fade_length)
				{
					out_frame_ptr[n] = in_frame_ptr[n] * (fade_in_count / fade_length);
					fade_in_count = fade_in_count + 1;
				}
				else
				{
					out_frame_ptr[n] = in_frame_ptr[n];
					fade_in_end = 1;
				}
			}
		}

	}
}

