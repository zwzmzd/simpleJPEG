#ifndef _PROCESS_H
#define _PROCESS_H

extern uint_8 quant[2][8][8];

void process_write(uint_32 a, uint_32 s_bit);
void process_buf_output();

void process_unit_1st_stage(int in[8][8], int data[8][8], uint_8 type, int pre_DC);
void process_unit_2nd_stage(int data[8][8], uint_8 type);

void process_construct_huffman_table(int len[], int seq[], struct huffman_storage r[]);
void process_write_z_shape(uint_8 data[8][8]);
void process_write_canonical_huffman_table(int len[], int seq[], uint_8 type);

#endif