#ifndef WAV_H_
#define WAV_H_

#include <stdint.h>
#include <stdio.h>

#define _21489_
#ifdef _21489_
	#define uint8_t uint32_t
	#define uint16_t uint32_t
#endif
/*
 * 需要特别注意的是，21489的char类型是4字节，而不是1字节！！！
 * 且 21489是大端格式存储
 *
 * wav_head有44个字节
 *
 * 因此，在21489平台下，用char指针或者int指针都是可以的，读取连续的
 * 11个char(int)长度
 *
 * char * file = fileread("xxx.wav");
 * char * wav_head;
 * wav_head_ptr = (char *)malloc(11 * sizeof(char));
 * memcry(wav_head, file, 11);
 * wav_heat.xxx = wav_head_ptr[1] & 0xFF00
 * wav_heat.
 * wav_heat.
 *
 */


/*
 * wav_head
 */
typedef struct
{
	uint32_t chunk_id[4];			// 4 bytes: char[4]			文档标识：		大写字符串"RIFF", 标明该文件为有效的 RIFF 格式文档
	uint32_t chunk_size;		// 4 bytes: unsigned int	文件数据长度：	从下一个字节开始算起的，因此实际长度需要加 8 个字节
	char format[4];				// 4 bytes: char[4]			文件格式类型：	大写字符串"wave", 表明该文件是wav文件
	char sub_chunk_id[4];		// 4 bytes: char[4]			格式块标识：		小写字符串"fmt "
	uint32_t sub_chunk_size;	// 4 bytes: unsigned int	格式块长度：
	uint16_t audio_format;		// 2 bytes: unsigned short	编码格式代码
	uint16_t num_channels;		// 2 bytes: unsigned short	声道数目
	uint32_t sample_rate;		// 4 bytes: unsigned int	采样率
	uint32_t byte_rate;			// 4 bytes: unsigned int	数据传输速率
	uint16_t block_align;		// 2 bytes: unsigned short	采样点存储时所占字节数    前两个字节是左声道，后两个字节是右声道
	uint16_t bits_per_sample;	// 2 bytes: unsigned int	采样点实际有效位数
	char data_chunk_id[4];	// 4 bytes: char[4]			数据标识		小写字符串"data"
	uint32_t data_chunk_size;	// 4 bytes: unsigned int	PCM 数据总长度, 单位字节
}wav_head_t;

void wav_print_head(char *wav_file);

#endif /* WAV_H_ */
