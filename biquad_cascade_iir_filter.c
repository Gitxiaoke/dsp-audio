#include "biquad_cascade_iir_filter.h"

# define df1
//# define df2
//# define df2T

struct biquad_df1 bq_df1[8] = {
	{.coefA = {1, -0.4705, 0.9957}, .coefB = {1, -0.4304, 0.2478}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -0.6558, 0.9405}, .coefB = {1, -0.4962, 0.9718}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -1.3870, 0.9207}, .coefB = {1, -1.1559, 0.8589}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -1.4487, 0.8673}, .coefB = {1, -1.3160, 0.9490}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -1.5889, 0.8551}, .coefB = {1, -1.6783, 0.8510}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -1.8644, 0.9748}, .coefB = {1, -1.8532, 0.9717}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -1.9712, 0.9732}, .coefB = {1, -1.9665, 0.9681}, .temp_x = {0, 0}, .temp_y = {0, 0}},
	{.coefA = {1, -0.7371, 0     }, .coefB = {1, -0.5956, 0     }, .temp_x = {0, 0}, .temp_y = {0, 0}}
};

struct biquad_df2 bq_df2[8] = {
	{.coefA = {1, -0.4705, 0.9957}, .coefB = {1, -0.4304, 0.2478}, .w={0, 0, 0}},
	{.coefA = {1, -0.6558, 0.9405}, .coefB = {1, -0.4962, 0.9718}, .w={0, 0, 0}},
	{.coefA = {1, -1.3870, 0.9207}, .coefB = {1, -1.1559, 0.8589}, .w={0, 0, 0}},
	{.coefA = {1, -1.4487, 0.8673}, .coefB = {1, -1.3160, 0.9490}, .w={0, 0, 0}},
	{.coefA = {1, -1.5889, 0.8551}, .coefB = {1, -1.6783, 0.8510}, .w={0, 0, 0}},
	{.coefA = {1, -1.8644, 0.9748}, .coefB = {1, -1.8532, 0.9717}, .w={0, 0, 0}},
	{.coefA = {1, -1.9712, 0.9732}, .coefB = {1, -1.9665, 0.9681}, .w={0, 0, 0}},
	{.coefA = {1, -0.7371, 0     }, .coefB = {1, -0.5956, 0     }, .w={0, 0, 0}}
};

struct biquad_df2T bq_df2T[8] = {
	{.coefA = {1, -0.4705, 0.9957}, .coefB = {1, -0.4304, 0.2478}, .w={0, 0}},
	{.coefA = {1, -0.6558, 0.9405}, .coefB = {1, -0.4962, 0.9718}, .w={0, 0}},
	{.coefA = {1, -1.3870, 0.9207}, .coefB = {1, -1.1559, 0.8589}, .w={0, 0}},
	{.coefA = {1, -1.4487, 0.8673}, .coefB = {1, -1.3160, 0.9490}, .w={0, 0}},
	{.coefA = {1, -1.5889, 0.8551}, .coefB = {1, -1.6783, 0.8510}, .w={0, 0}},
	{.coefA = {1, -1.8644, 0.9748}, .coefB = {1, -1.8532, 0.9717}, .w={0, 0}},
	{.coefA = {1, -1.9712, 0.9732}, .coefB = {1, -1.9665, 0.9681}, .w={0, 0}},
	{.coefA = {1, -0.7371, 0     }, .coefB = {1, -0.5956, 0     }, .w={0, 0}}
};

/*
 * 双二阶IIR滤波器直接I型
 * */
float biquad_iir_df_1(float input, struct biquad_df1 *bq)
{
	float output = 0;
	/* 计算输出 */
	output = bq->coefB[0] * input + bq->coefB[1] * bq->temp_x[0] + bq->coefB[2] * bq->temp_x[1] - \
			 bq->coefA[1] * bq->temp_y[0] - bq->coefA[2] * bq->temp_y[1];
	/* 更新历史值 */
	bq->temp_x[1] = bq->temp_x[0];
	bq->temp_x[0] = input;

	bq->temp_y[1] = bq->temp_y[0];
	bq->temp_y[0] = output;

	return output;
}

/*
 * 双二阶IIR滤波器直接II型
 * */
float biquad_iir_df_2(float input, struct biquad_df2 *bq)
{
	float output = 0;
	/* 计算输入值 */
	bq->w[0] = input - bq->coefA[1] * bq->w[1] - bq->coefA[2] * bq->w[2];
	/* 计算输出值 */
	output = bq->coefB[0] * bq->w[0] + bq->coefB[1] * bq->w[1] + bq->coefB[2] * bq->w[2];
	/* 更新历史值 */
	bq->w[2] = bq->w[1];
	bq->w[1] = bq->w[0];

	return output;
}

/*
 * 双二阶IIR滤波器直接II型转置
 * */
float biquad_iir_df_2T(float input, struct biquad_df2T *bq)
{
	float output = 0;
	float add0 = 0, add1 = 0;
	/* 计算输出值 */
	output = bq->coefB[0] *input + bq->w[0];

	add0 = bq->coefB[1] * input - bq->coefA[1] * output + bq->w[1];
	add1 = bq->coefB[2] * input - bq->coefA[2] * output;

	/* 更新历史值 */
	bq->w[0] = add0;
	bq->w[1] = add1;

	return output;
}

void biquad_cascade_filter(float* input, float* output, int framelen)
{
	float gain = 0.9726;
	float temp = 0;
	int i;
	for(i = 0; i < framelen; i++)
	{
		#ifdef df1
			temp = biquad_iir_df_1(input[i], &bq_df1[0]);
			temp = biquad_iir_df_1(temp, &bq_df1[1]);
			temp = biquad_iir_df_1(temp, &bq_df1[2]);
			temp = biquad_iir_df_1(temp, &bq_df1[3]);
			temp = biquad_iir_df_1(temp, &bq_df1[4]);
			temp = biquad_iir_df_1(temp, &bq_df1[5]);
			temp = biquad_iir_df_1(temp, &bq_df1[6]);
			temp = biquad_iir_df_1(temp, &bq_df1[7]);
		#endif
		#ifdef df2
			temp = biquad_iir_df_2(input[i], &bq_df2[0]);
			temp = biquad_iir_df_2(temp, &bq_df2[1]);
			temp = biquad_iir_df_2(temp, &bq_df2[2]);
			temp = biquad_iir_df_2(temp, &bq_df2[3]);
			temp = biquad_iir_df_2(temp, &bq_df2[4]);
			temp = biquad_iir_df_2(temp, &bq_df2[5]);
			temp = biquad_iir_df_2(temp, &bq_df2[6]);
			temp = biquad_iir_df_2(temp, &bq_df2[7]);
		#endif
		#ifdef df2T
			temp = biquad_iir_df_2T(input[i], &bq_df2T[0]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[1]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[2]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[3]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[4]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[5]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[6]);
			temp = biquad_iir_df_2T(temp, &bq_df2T[7]);
		#endif
		output[i] = temp;
	}
	
	for(i = 0; i < framelen; i++)
	{
		output[i] = gain * output[i];
	}
}
