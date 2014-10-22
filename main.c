#include "def.h"

#define BUF_SIZE 40000

void cos_table_init();
void process_test();

int f[2][2][256];
double sqrt_2;

uint_32 pi, pj;
char data[256];

FILE *fout;

extern int STD_HUFFMAN_LEN[2][2][17];
extern int STD_HUFFMAN_CODE[2][2][256];

void revert(void *src, void *dst, size_t t)
{
	int i;
	for (i = 0 ; i < t ; i++)
		((char *)dst)[i] = ((char *)src)[t - i - 1];
}

uint_32 calc_bmp_size(int pv, int ph)
{
	uint_32 t = 0;
	int i, j;
	
	t = ph * 3;
	t = (t + 3) & 0xfffffffc;
	t *= pv;
	
	printf("size: %d\n", t);
	return t;
}

void convert_bgr_to_ycc(uint_8 data[], int pv, int ph)
{
	uint_8 *c = data;
	int i, j, k;
	uint_32 t = 0;


	for (i = 0 ; i < pv ; i++)
	{
		for (j = 0 ; j < ph ; j++)
		{
			uint_8 B = c[0];
			uint_8 G = c[1];
			uint_8 R = c[2];

			double Y = 0.299 * R + 0.587 * G + 0.114 * B;
			double Cb = -0.1687 * R - 0.3313 * G + 0.5 * B + 128;
			double Cr = 0.5 * R - 0.4187 * G - 0.0813 * B + 128;

			*(c++) = (uint_8) Y;
			*(c++) = (uint_8) Cb;
			*(c++) = (uint_8) Cr;
		}
		
		t = c - data;
		t %= 4;
		if (t != 0) c += 4 - t;
	}
	
	printf("size: %d\n", (uint_32)(c - data));
}

void convert_ycc_to_bgr(uint_8 data[], int size)
{
	uint_8 *c = data;
	int i;

	for (i = 0 ; i < size ; i++)
	{
		uint_8 Y = c[0];
		uint_8 Cb = c[1];
		uint_8 Cr = c[2];

		double R = Y + 1.402 * (Cr - 128);
		double G = Y - 0.34414 * (Cb - 128) -0.71414 * (Cr - 128);
		double B = Y + 1.772 * (Cb - 128);

		*(c++) = (uint_8) B;
		*(c++) = (uint_8) G;
		*(c++) = (uint_8) R;
	}

}

void init_all()
{
	cos_table_init();
	
	printf("Init frequency table......\n");
	memset(f, 0, sizeof(f));
	
	sqrt_2 = sqrt(2);
}

void out_8_8(int data[8][8])
{
	int i, j;
	for (i = 0 ; i < 8 ; i++)
	{
		for (j = 0 ; j < 8 ; j++)
			printf("%3d", data[i][j]);
		printf("\n");
	}
	
	printf("\n");
}

void encode(uint_8 data[], uint_32 ph, uint_32 pv)
{
	uint_32 H_block_count = (ph + 7) / 8;
	uint_32 V_block_count = (pv + 7) / 8;
	uint_32 total = H_block_count * V_block_count;
	
	static int (*Y_data)[8][8];
	static int (*Cb_data)[8][8];
	static int (*Cr_data)[8][8];
	static char buf[25];

	int i, j, k;
	int pre_DC_Y, pre_DC_Cb, pre_DC_Cr;
	uint_8 *now = data;
	uint_32 total_pixel = H_block_count * V_block_count * 64;
	
	Y_data = (int (*)[8][8])malloc(total_pixel * sizeof(int));
	Cb_data = (int (*)[8][8])malloc(total_pixel * sizeof(int));
	Cr_data = (int (*)[8][8])malloc(total_pixel * sizeof(int));
	
	memset(Y_data, 0, total_pixel);
	memset(Cb_data, 0, total_pixel);
	memset(Cr_data, 0, total_pixel);

	for ( i = pv - 1 ; i >= 0; i--)
	{
		uint_32 t;
		
		for (j = 0 ; j < ph ; j++)
		{
			int x, y, z;
			
			z = (i / 8) * H_block_count + (j / 8);
			x = i % 8;
			y = j % 8;

			Y_data[z][x][y] = now[0];
			Cb_data[z][x][y] = now[1];
			Cr_data[z][x][y] = now[2];
			
			now += 3;
			
		}
		
		t = now - data;
		t %= 4;
		if (t != 0) now += 4 - t;
	}
	
	pre_DC_Y = 0;
	pre_DC_Cb = 0;
	pre_DC_Cr = 0;
	for (k = 0 ; k < total ; k++)
	{
		process_unit_1st_stage(Y_data[k], Y_data[k], 0, pre_DC_Y);
		process_unit_1st_stage(Cb_data[k], Cb_data[k], 1, pre_DC_Cb);
		process_unit_1st_stage(Cr_data[k], Cr_data[k], 1, pre_DC_Cr);

		pre_DC_Y += Y_data[k][0][0];
		pre_DC_Cb += Cb_data[k][0][0];
		pre_DC_Cr += Cr_data[k][0][0];
	}
	
	{
#ifdef USER_DEFINE_HUFFMAN
		static int len[17];
		static int seq[256];
		
		for (i = 0 ; i < 2 ; i++)
			for (j = 0 ; j < 2 ; j++)
			{
				get_canonical_huffman(f[i][j], len, seq, 256);
				
				process_write_canonical_huffman_table(len, seq, (j << 4) + i);
				
				process_construct_huffman_table(len, seq, huffman[i][j]);
			}
#else
		for (i = 0 ; i < 2 ; i++)
			for (j = 0 ; j < 2 ; j++)
			{
				process_write_canonical_huffman_table(STD_HUFFMAN_LEN[i][j], STD_HUFFMAN_CODE[i][j], (j << 4) + i);
				
				process_construct_huffman_table(STD_HUFFMAN_LEN[i][j], STD_HUFFMAN_CODE[i][j], huffman[i][j]);
			}
#endif
	}
	
	buf[0] = 0xFF;
	buf[1] = 0xDA;
	buf[2] = 0;
	buf[3] = 0x0C;
	buf[4] = 0x03;
	buf[5] = 0x01;
	buf[6] = 0x00;
	buf[7] = 0x02;
	buf[8] = 0x11;
	buf[9] = 0x03;
	buf[10] = 0x11;
	buf[11] = 0;
	buf[12] = 0x3F;
	buf[13] = 0;
	
	fwrite(buf, 1, 14, fout);
	
	for (k = 0 ; k < total ; k++)
	{
		process_unit_2nd_stage(Y_data[k], 0);
		process_unit_2nd_stage(Cb_data[k], 1);
		process_unit_2nd_stage(Cr_data[k], 1);
		
		pre_DC_Y += Y_data[k][0][0];
		pre_DC_Cb += Cb_data[k][0][0];
		pre_DC_Cr += Cr_data[k][0][0];
	}
	
	free(Y_data);
	free(Cb_data);
	free(Cr_data);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	uint_8 *p_8;
	uint_16 *p_16;
	uint_32 *p_32;
	uint_32 size;
	uint_8 *frame;
	static char buf[100];
	int i, j;
	
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s input.bmp output.jpg\n", argv[0]);
		return -1;
	}

	init_all();
	
	fp = fopen(argv[1], "rb");
	fout = fopen(argv[2], "wb");

	fread(data, 1, 0x36, fp);
	p_32 = (uint_32 *)&data[2];

	size = *p_32;

	size -= 0x36;

	p_32 = (uint_32 *)&data[0x12];
	pj = *p_32;

	p_32 = (uint_32 *)&data[0x16];
	pi = *p_32;


	//frame = data + 0x36;
	
	assert(size == calc_bmp_size(pi, pj));
	frame = (unsigned char *)malloc(size);
	fread(frame, 1, size, fp);

	printf("%d: %d * %d\n", size, pj, pi);

	
	//write header
	buf[0] = 0xFF;
	buf[1] = 0xD8;
	fwrite(buf, 1, 2, fout);

	//write APP0
	buf[0] = 0xFF;
	buf[1] = 0xE0;
	buf[2] = 0;
	buf[3] = 0x10;
	buf[4] = 'J';
	buf[5] = 'F';
	buf[6] = 'I';
	buf[7] = 'F';
	buf[8] = 0;
	buf[9] = 0x01;
	buf[10] = 0x01;
	buf[11] = 0x01;
	buf[12] = 0;
	buf[13] = 0x60;
	buf[14] = 0;
	buf[15] = 0x60;
	buf[16] = 0;
	buf[17] = 0;
	fwrite(buf, 1, 18, fout);
	
	//write DQT for Y
	buf[0] = 0xFF;
	buf[1] = 0xDB;
	buf[2] = 0;
	buf[3] = 0x43;
	buf[4] = 0;
	fwrite(buf, 1, 5, fout);
	process_write_z_shape(quant[0]);
	
	//write DQT for Cb, Cr
	buf[0] = 0xFF;
	buf[1] = 0xDB;
	buf[2] = 0;
	buf[3] = 0x43;
	buf[4] = 01;
	fwrite(buf, 1, 5, fout);
	process_write_z_shape(quant[1]);
	
	//write SOI
	buf[0] = 0xFF;
	buf[1] = 0xC0;
	buf[2] = 0;
	buf[3] = 0x11;
	buf[4] = 0x08;
	
	revert(&pi, buf + 5, 2);
	revert(&pj, buf + 7, 2);
	buf[9] = 0x03;
	
	fwrite(buf, 1, 10, fout);
	
	buf[0] = 0x01;
	buf[1] = 0x11;
	buf[2] = 0x00;
	fwrite(buf, 1, 3, fout);
	
	buf[0] = 0x02;
	buf[1] = 0x11;
	buf[2] = 0x01;
	fwrite(buf, 1, 3, fout);
	
	buf[0] = 0x03;
	buf[1] = 0x11;
	buf[2] = 0x01;
	fwrite(buf, 1, 3, fout);

	convert_bgr_to_ycc(frame, pi, pj);
	encode(frame, pj, pi);
	//convert_ycc_to_bgr(frame, pi * pj);
	
	process_buf_output();
	//write EOI
	buf[0] = 0xFF;
	buf[1] = 0xD9;
	fwrite(buf, 1, 2, fout);
	//process_test();

	return 0;
}
