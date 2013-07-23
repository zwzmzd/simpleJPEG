#include "def.h"

#ifndef GOOD
uint_8 quant[2][8][8] = {
	{
		{16,11,10,16,24,40,51,61},
		{12,12,14,19,26,58,60,55},
		{14,13,16,24,40,57,69,56},
		{14,17,22,29,51,87,80,62},
		{18,22,37,56,68,109,103,77},
		{24,35,55,64,81,104,113,92},
		{49,64,78,87,103,121,120,101},
		{72,92,95,98,112,100,103,99},
	},
	{
		{17,18,24,47,99,99,99,99},
		{18,21,26,66,99,99,99,99},
		{24,26,56,99,99,99,99,99},
		{47,66,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
		{99,99,99,99,99,99,99,99},
	}
};
#else
uint_8 quant[2][8][8] = {
	{
		{ 6,  4,  4,  6,  9, 11, 12, 16 },
		{ 4,  5,  5,  6,  8, 10, 12, 12 },
		{ 4,  5,  5,  6, 10, 12, 14, 19 },
		{ 6,  6,  6, 11, 12, 15, 19, 28 },
		{ 9,  8, 10, 12, 16, 20, 27, 32 },
		{11, 10, 12, 15, 20, 27, 32, 32 },
		{12, 12, 14, 19, 27, 32, 32, 32 },
		{16, 12, 19, 28, 32, 32, 32, 32 }
	},
	{
		{ 7,  7, 13, 24, 26, 32, 32, 32 },
		{ 7, 12, 16, 21, 32, 32, 32, 32 },
		{13, 16, 17, 32, 32, 32, 32, 32 },
		{24, 21, 32, 32, 32, 32, 32, 32 },
		{26, 32, 32, 32, 32, 32, 32, 32 },
		{32, 32, 32, 32, 32, 32, 32, 32 },
		{32, 32, 32, 32, 32, 32, 32, 32 },
		{32, 32, 32, 32, 32, 32, 32, 32 }
	}
};
#endif

extern double cos_table[][8];
extern double sqrt_2;
extern int f[][2][256];

extern FILE *fout;


void process_FDCT(int in[8][8], int data[8][8])
{
	int u, v, i, j;
	static double buf[8][8];
	
	for (i = 0 ; i < 8 ; i++)
		for (j = 0 ; j < 8 ; j++)
			data[i][j] = in[i][j] - 128;
			
	for (v = 0 ; v < 8 ; v++)
		for (i = 0 ; i < 8 ; i++)
		{
			double d = 0;
			for (j = 0 ; j < 8 ; j++)
				d += data[i][j] * cos_table[v][j];
			d = d / 2;
			if (v == 0) d = d / sqrt_2;
			
			buf[v][i] = d;
		}
		
		
	for (u = 0 ; u < 8 ; u++)
		for (v = 0 ; v < 8 ; v++)
		{
			double d = 0;
			for (i = 0 ; i < 8 ; i++)
				d += buf[v][i] * cos_table[u][i];
			d = d / 2;
			if (u == 0) d = d / sqrt_2;
			
			data[u][v] = (int)d;
		}
}

void process_write_z_shape(uint_8 data[8][8])
{	
	uint_8 c;
	int i, j, k, di, dj;

	i = 0;
	j = 0;

	di = -1;
	dj = 1;
	
	for (k = 0 ; k < 64 ; k++)
	{
		c = data[i][j];
		fwrite(&c, 1, 1, fout);
		if (i == 0 && di == -1)
		{
			j++;
			di = 1;
			dj = -1;
		}
		else if (k < 35 && j == 0 && di == 1) 
		{
			i++;
			di = -1;
			dj = 1;
		}
		else if (i == 7 && di == 1)
		{
			j++;
			di = -1;
			dj = 1;
		}
		else if (j == 7 && di == -1)
		{
			i++;
			di = 1;
			dj = -1;
		}
		else
		{
			i += di;
			j += dj;
		}
	}
}

void process_unit_1st_stage(int in[8][8], int data[8][8], uint_8 type, int pre_DC)
{
	int i, j;
	int last = 0;
	
	process_FDCT(in, data);
	
	for (i = 0 ; i < 8 ; i++)
		for (j = 0 ; j < 8 ; j++)
			data[i][j] /= quant[type][i][j];
			
	data[0][0] -= pre_DC;
			
	f[type][0][get_huffman_len(data[0][0])]++;
	
	{
		int i, j, k, di, dj;

		i = 0;
		j = 0;

		di = -1;
		dj = 1;
		
		for (k = 0 ; k < 64 ; k++)
		{
			if (data[i][j] != 0) last = k;
			if (i == 0 && di == -1)
			{
				j++;
				di = 1;
				dj = -1;
			}
			else if (k < 35 && j == 0 && di == 1) 
			{
				i++;
				di = -1;
				dj = 1;
			}
			else if (i == 7 && di == 1)
			{
				j++;
				di = -1;
				dj = 1;
			}
			else if (j == 7 && di == -1)
			{
				i++;
				di = 1;
				dj = -1;
			}
			else
			{
				i += di;
				j += dj;
			}
		}
	}
	
	last++;
	
	{
		int i, j, k, di, dj;
		int n = 0;

		i = 0;
		j = 1;

		di = 1;
		dj = -1;
		
		for (k = 1 ; k < last ; k++)
		{
			if (data[i][j] == 0)
			{
				n++;
				
				if (n == 16)
				{
					f[type][1][15 << 4]++;
					n = 0;
				}
			}
			else
			{
				f[type][1][(n << 4) + get_huffman_len(data[i][j])]++;
				n = 0;
			}
			if (i == 0 && di == -1)
			{
				j++;
				di = 1;
				dj = -1;
			}
			else if (k < 35 && j == 0 && di == 1) 
			{
				i++;
				di = -1;
				dj = 1;
			}
			else if (i == 7 && di == 1)
			{
				j++;
				di = -1;
				dj = 1;
			}
			else if (j == 7 && di == -1)
			{
				i++;
				di = 1;
				dj = -1;
			}
			else
			{
				i += di;
				j += dj;
			}
		}
	}
	
	if (last < 64) f[type][1][0]++;
}

#define BUF_SIZE 10
static void process_write_bit(bool a)
{
	static uint_8 buf[BUF_SIZE];
	static int current_bit = 0;
	
	if (fout == 0)
		fout = fopen("test", "wb");
	
	if (a >> 1)
	{
		int i;
		for (i = 0 ; i <= 7 - (current_bit & 0x07) ; i++)
			buf[current_bit / 8] |= (1 << i);
			
		for (i = 0 ; i < (current_bit + 7) / 8 ; i++)
		{
			fwrite(&buf[i], 1, 1, fout);
			if (buf[i] == 0xff)
			{
				static uint_8 padding = 0;
				fwrite(&padding, 1, 1, fout);
			}
		}

		current_bit = 0;
		return;
	}
	
	if ((current_bit & 0x7) == 0) buf[current_bit / 8] = 0;
	
	buf[current_bit / 8] |= a << (7 - (current_bit & 0x07));
	current_bit++;
	
	if (current_bit == BUF_SIZE * 8)
	{
		int i;

		current_bit = 0;
		for (i = 0 ; i < BUF_SIZE ; i++)
		{
			fwrite(&buf[i], 1, 1, fout);
			if (buf[i] == 0xff)
			{
				static uint_8 padding = 0;
				fwrite(&padding, 1, 1, fout);
			}
		}
	}
}

void process_write(uint_32 a, uint_32 s_bit)
{
	static bool buf[32];
	int s = 0;
	
	while (s_bit > 0)
	{
		buf[s++] = a & 1;
		a = a >> 1;
		s_bit--;
	}
	
	while (s > 0)
		process_write_bit(buf[--s]);
}

void process_buf_output()
{
	process_write_bit(3);
}

void process_unit_2nd_stage(int data[8][8], uint_8 type)
{
	int i, j;
	int last = 0;
	
	uint_8 len;
	uint_16 code;
	
	get_huffman_code(data[0][0], &len, &code);
			
	{
		uint_8 code_len = huffman[type][0][len].len;
		uint_16 c = huffman[type][0][len].code;
		
		process_write(c, code_len);
	}
	
	process_write(code, len);
	
	{
		int i, j, k, di, dj;

		i = 0;
		j = 0;

		di = -1;
		dj = 1;
		
		for (k = 0 ; k < 64 ; k++)
		{
			if (data[i][j] != 0) last = k;
			if (i == 0 && di == -1)
			{
				j++;
				di = 1;
				dj = -1;
			}
			else if (k < 35 && j == 0 && di == 1) 
			{
				i++;
				di = -1;
				dj = 1;
			}
			else if (i == 7 && di == 1)
			{
				j++;
				di = -1;
				dj = 1;
			}
			else if (j == 7 && di == -1)
			{
				i++;
				di = 1;
				dj = -1;
			}
			else
			{
				i += di;
				j += dj;
			}
		}
	}
	
	last++;
	
	{
		int i, j, k, di, dj;
		int n = 0;

		i = 0;
		j = 1;

		di = 1;
		dj = -1;
		
		for (k = 1 ; k < last ; k++)
		{
			if (data[i][j] == 0)
			{
				n++;
				
				if (n == 16)
				{
					n = 0;	
					{
						uint_8 code_len = huffman[type][1][15 << 4].len;
						uint_16 c = huffman[type][1][15 << 4].code;
						
						process_write(c, code_len);
					}
				}
			}
			else
			{
				uint_8 val_len;
				uint_16 val_code;
				get_huffman_code(data[i][j], &val_len, &val_code);
				{
						uint_8 code_len = huffman[type][1][(n << 4) + val_len].len;
						uint_16 c = huffman[type][1][(n << 4) + val_len].code;
						
						process_write(c, code_len);
						process_write(val_code, val_len);
				}
				n = 0;
			}
			if (i == 0 && di == -1)
			{
				j++;
				di = 1;
				dj = -1;
			}
			else if (k < 35 && j == 0 && di == 1) 
			{
				i++;
				di = -1;
				dj = 1;
			}
			else if (i == 7 && di == 1)
			{
				j++;
				di = -1;
				dj = 1;
			}
			else if (j == 7 && di == -1)
			{
				i++;
				di = 1;
				dj = -1;
			}
			else
			{
				i += di;
				j += dj;
			}
		}
	}
	
	if (last < 64)
	{
		uint_8 len = huffman[type][1][0].len;
		uint_16 code = huffman[type][1][0].code;
		
		process_write(code, len);
	}
}

void process_construct_huffman_table(int len[], int seq[], struct huffman_storage r[])
{
	int t = 0;
	uint_16 code = 0;
	int i, j;
	
	for (i = 0 ; i <= 16 ; i++)
	{
		for (j = 0 ; j < len[i] ; j++)
		{
			uint_8 num = seq[t++];
			r[num].len = i;
			r[num].code = code++;
		}
		
		code = code << 1;
	}
}

void process_write_canonical_huffman_table(int len[], int seq[], uint_8 type)
{
	int i, j, t;
	static char buf[10];
	
	t = 0;
	for (i = 1 ; i <= 16 ; i++)
		t += len[i];
	
	buf[0] = 0xFF;
	buf[1] = 0xC4;
	t += 19;
	revert(&t, buf + 2, 2);
	t -= 19;
	
	buf[4] = type;
	
	fwrite(buf, 1, 5, fout);
		
	for (i = 1 ; i <= 16 ; i++)
	{
		uint_8 num = len[i];
		fwrite(&num, 1, 1, fout);
	}
	
	for (i = 0 ; i < t ; i++)
	{
		uint_8 num = seq[i];
		fwrite(&num, 1, 1, fout);
	}
}
void process_test()
{
	/*
	static int result[8][8];
	static uint_8 in[8][8] = {
		{139,144,149,153,155,155,155,155},
		{144,151,153,156,159,156,156,156},
		{150,155,160,163,158,156,156,156},
		{159,161,162,160,160,159,159,159},
		{159,160,161,162,162,155,155,155},
		{161,161,161,161,160,157,157,157},
		{162,162,161,163,162,157,157,157},
		{162,162,161,161,163,158,158,158}
	};
	
	int i, j;
	
	process_FDCT(in, result);
	
	for (i = 0 ; i < 8 ; i++)
	{
		for (j = 0 ; j < 8 ; j++)
			printf("%d\t", result[i][j]);
		printf("\n");
	}
	*/
	/*
	fout = fopen("test", "wb");
	printf("%d\n", get_huffman_len(0));
	process_write(0xccc, 20);
	process_buf_output();
	*/
	uint_8 len;
	uint_16 code;
	get_huffman_code(21, &len, &code);
	printf("21 : %d 0x%04x\n", len, code);
}
