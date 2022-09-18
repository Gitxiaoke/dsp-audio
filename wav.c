#include "wav.h"

uint32_t head_buff[11];  /* char head[11] 实际上有44字节 */
void wav_print_head(char *wav_file)
{
	wav_head_t wav_head;
	FILE *file_ptr = fopen(wav_file, "rb");
	fread(head_buff, 11 * sizeof(uint32_t), 1, file_ptr);

	wav_head.chunk_id[0] = (head_buff[0] & 0x000000FF)>>0;
	wav_head.chunk_id[1] = (head_buff[0] & 0x0000FF00)>>8;
	wav_head.chunk_id[2] = (head_buff[0] & 0x00FF0000)>>16;
	wav_head.chunk_id[3] = (head_buff[0] & 0xFF000000)>>24;

	wav_head.chunk_size  = (uint32_t)head_buff[1];

	wav_head.format[0] = (head_buff[2] & 0x000000FF)>>0;
	wav_head.format[1] = (head_buff[2] & 0x0000FF00)>>8;
	wav_head.format[2] = (head_buff[2] & 0x00FF0000)>>16;
	wav_head.format[3] = (head_buff[2] & 0xFF000000)>>24;

	wav_head.sub_chunk_id[0] = (head_buff[3] & 0x000000FF)>>0;
	wav_head.sub_chunk_id[1] = (head_buff[3] & 0x0000FF00)>>8;
	wav_head.sub_chunk_id[2] = (head_buff[3] & 0x00FF0000)>>16;
	wav_head.sub_chunk_id[3] = (head_buff[3] & 0xFF000000)>>24;

	wav_head.sub_chunk_size  = (uint32_t)head_buff[4];

	wav_head.audio_format = (uint16_t)((head_buff[5] &0x0000FFFF)>>0);

	wav_head.num_channels = (uint16_t)((head_buff[5] &0xFFFF0000)>>16);

	wav_head.sample_rate = (uint32_t)head_buff[6];

	wav_head.byte_rate = (uint32_t)head_buff[7];

	wav_head.block_align = (uint16_t)((head_buff[8] &0x0000FFFF)>>0);

	wav_head.bits_per_sample = (uint16_t)((head_buff[8] &0xFFFF0000)>>16);

	wav_head.data_chunk_id[0] = (head_buff[9] & 0x000000FF)>>0;
	wav_head.data_chunk_id[1] = (head_buff[9] & 0x0000FF00)>>8;
	wav_head.data_chunk_id[2] = (head_buff[9] & 0x00FF0000)>>16;
	wav_head.data_chunk_id[3] = (head_buff[9] & 0xFF000000)>>24;

	wav_head.data_chunk_size = (uint32_t)head_buff[10];

	printf("chunk_id: %c%c%c%c\n", wav_head.chunk_id[0],wav_head.chunk_id[1],wav_head.chunk_id[2],wav_head.chunk_id[3]);
	printf("chunk_size: %d\n", wav_head.chunk_size + 8);
	printf("format: %c%c%c%c\n", wav_head.format[0], wav_head.format[1], wav_head.format[2], wav_head.format[3]);
	printf("sub_chunk_id: %c%c%c%c\n", wav_head.sub_chunk_id[0],wav_head.sub_chunk_id[1],wav_head.sub_chunk_id[2],wav_head.sub_chunk_id[3]);
	printf("sub_chunk_size: %d\n", wav_head.sub_chunk_size);
	printf("audio_format: %d\n", wav_head.audio_format);
	printf("num_channels: %d\n", wav_head.num_channels);
	printf("sample_rate: %d\n", wav_head.sample_rate);
	printf("byte_rate: %d\n", wav_head.byte_rate);
	printf("block_align: %d\n", wav_head.block_align);
	printf("bits_per_sample: %d\n", wav_head.bits_per_sample);
	printf("data_chunk_id: %c%c%c%c\n", wav_head.data_chunk_id[0],wav_head.data_chunk_id[1],wav_head.data_chunk_id[2],wav_head.data_chunk_id[3]);
	printf("data_chunk_size: %d\n", wav_head.data_chunk_size);
}
