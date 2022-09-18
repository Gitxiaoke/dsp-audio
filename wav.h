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
 * ��Ҫ�ر�ע����ǣ�21489��char������4�ֽڣ�������1�ֽڣ�����
 * �� 21489�Ǵ�˸�ʽ�洢
 *
 * wav_head��44���ֽ�
 *
 * ��ˣ���21489ƽ̨�£���charָ�����intָ�붼�ǿ��Եģ���ȡ������
 * 11��char(int)����
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
	uint32_t chunk_id[4];			// 4 bytes: char[4]			�ĵ���ʶ��		��д�ַ���"RIFF", �������ļ�Ϊ��Ч�� RIFF ��ʽ�ĵ�
	uint32_t chunk_size;		// 4 bytes: unsigned int	�ļ����ݳ��ȣ�	����һ���ֽڿ�ʼ����ģ����ʵ�ʳ�����Ҫ�� 8 ���ֽ�
	char format[4];				// 4 bytes: char[4]			�ļ���ʽ���ͣ�	��д�ַ���"wave", �������ļ���wav�ļ�
	char sub_chunk_id[4];		// 4 bytes: char[4]			��ʽ���ʶ��		Сд�ַ���"fmt "
	uint32_t sub_chunk_size;	// 4 bytes: unsigned int	��ʽ�鳤�ȣ�
	uint16_t audio_format;		// 2 bytes: unsigned short	�����ʽ����
	uint16_t num_channels;		// 2 bytes: unsigned short	������Ŀ
	uint32_t sample_rate;		// 4 bytes: unsigned int	������
	uint32_t byte_rate;			// 4 bytes: unsigned int	���ݴ�������
	uint16_t block_align;		// 2 bytes: unsigned short	������洢ʱ��ռ�ֽ���    ǰ�����ֽ������������������ֽ���������
	uint16_t bits_per_sample;	// 2 bytes: unsigned int	������ʵ����Чλ��
	char data_chunk_id[4];	// 4 bytes: char[4]			���ݱ�ʶ		Сд�ַ���"data"
	uint32_t data_chunk_size;	// 4 bytes: unsigned int	PCM �����ܳ���, ��λ�ֽ�
}wav_head_t;

void wav_print_head(char *wav_file);

#endif /* WAV_H_ */
