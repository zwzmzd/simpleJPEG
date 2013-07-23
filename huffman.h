#ifndef _HUFFMAN_H
#define _HUFFMAN_H

struct heap_node
{
	int val;
	struct leaf_node *list;
	struct leaf_node **last_ref;
};

struct leaf_node
{
	int pos;
	struct leaf_node *next;
};

struct huffman_storage
{
	uint_8 len;
	uint_16 code;
};

uint_8 get_huffman_len(int a);
void get_huffman_code(int a, uint_8 *len, uint_16 *code);
extern struct huffman_storage huffman[][2][256];

#endif
