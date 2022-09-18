#include "audio_spectral_feature_extraction.h"

/*
 * Mel to Hz
 */
void Mel_to_Hz(float Mel_freq, float Hz_freq)
{
	Mel_freq = 2595*log10(1 + Hz_freq/700.0);
}
/*
 *Hz to MEL
 */
void Hz_to_Mel(float Hz_freq , float Mel_freq)
{
	Hz_freq = 700*pow(10, Mel_freq/2595 - 1);
}
/*
 *
 */
void spectrogram(float *input, int input_num, float *output_fft_mag)
{

}

/*
 *
 */
void fbank(float *input,   int input_num)
{

}

/*
 * ---> 0、输入信号x[n]：采样率默认48KHz，假如2048个点做FFT，频率分辨率为Fs/N = 48000/2048 ≈ 23 Hz
 * ---> 1、预加重
 * ---> 2、分帧：对于嵌入式实时运行而言，语音输入就是按DMA中断分帧获取得并存储到乒乓buffer
 * ---> 3、加窗 
 * ---> 4、FFT：2048 samples的时间为42.6ms
 * ---> 5、幅值平方（功率谱） 
 * ---> 6、MEL滤波器 
 * ---> 7、对数功率
 * ---> 8、离散余弦变换
 * ---> 9、MFCC 
 */
void mfcc(float *input_ptr, int input_num, int Fs, int mel_num, float *mel_ouput_ptr[input_num])
{
	complex_t input_fft[input_num];
	float input_freq_mag[input_num / 2 + 1];

	fft_real(input_ptr, input_fft, input_num);

	for(int i = 0; i <= input_num / 2 + 1; i++)
	{
		input_freq_mag[i] = sqrt(input_fft[i].real * input_fft[i].real + input_fft[i].imag * input_fft[i].imag);
	}



}
