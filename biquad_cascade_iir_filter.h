#ifndef __BIQUAD_CASCADE_IIR_FILTER_H_
#define __BIQUAD_CASCADE_IIR_FILTER_H_

struct biquad_df1{
	float coefA[3];
	float coefB[3];
	float temp_x[2];  /* 历史值 */
	float temp_y[2];  /* 历史值 */
};

struct biquad_df2{
	float coefA[3];
	float coefB[3];
	float w[3];  /* 历史值 */
};

struct biquad_df2T{
	float coefA[3];
	float coefB[3];
	float w[2];  /* 历史值 */
};

float biquad_iir_df_1(float input, struct biquad_df1 *bq);
float biquad_iir_df_2(float input, struct biquad_df2 *bq);
float biquad_iir_df_2T(float input, struct biquad_df2T *bq);

void biquad_cascade_filter(float* input, float* output, int framelen);

#endif

