#include "def.h"

struct huffman_storage huffman[2][2][256];

uint_8 get_huffman_len(int a)
{
	uint_32 m = abs(a);
	uint_8 count = 0;

	while (m > 0)
	{
		m /= 2;
		count++;
	}
	return count;
}

void get_huffman_code(int a, uint_8 *len, uint_16 *code)
{
	int start;
	uint_32 dis;
	*len = get_huffman_len(a);

	if (a == 0)
	{
		*code = 0;
	}
	else if (a < 0)
	{
		start = -((1 << *len) - 1);
		*code = a - start;
	}
	else
	{
		*code = a;
	}
}

static void top_to_down(struct heap_node *heap[], int n, int pos)
{
	while (1)
	{
		int j = pos * 2;
		if (j > n) break;

		if (j < n && heap[j+1]->val < heap[j]->val) j++;

		if (heap[pos]->val <= heap[j]->val) break;

		{
			struct heap_node *tmp = heap[pos];
			heap[pos] = heap[j];
			heap[j] = tmp;
		}
		pos = j; 
	}
}

static int construct_deep_array(int f[], int N, int deep[])
{
	int n = N;
	static struct heap_node *heap[500];
	int i;
	int total = 0;

	for (i = 0 ; i < n ; i++) 
		deep[i] = 0;

	for (i = 0 ; i < n ; i++)
	{
		if (f[i] > 0)
		{
			struct heap_node *p = (struct heap_node *)malloc(sizeof(struct heap_node));
			struct leaf_node *q = (struct leaf_node *)malloc(sizeof(struct leaf_node));

			q->pos = i;
			q->next = 0;

			p->val = f[i];
			p->list = q;
			p->last_ref = &q->next;

			heap[++total] = p;
		
		}
	}

	n = total;

	for (i = n / 2 ; i > 0 ; i--)
		top_to_down(heap, n, i);

	while (n > 1)
	{
		struct heap_node *a, *b;
		struct leaf_node *p;

		a = heap[1];

		heap[1] = heap[n];
		n--;
		top_to_down(heap, n, 1);
		b = heap[1];

		a->val += b->val;


		for (p = a->list ; p != 0 ; p = p->next)
			deep[p->pos]++;

		for (p = b->list ; p != 0 ; p = p->next)
			deep[p->pos]++;

		*a->last_ref = b->list;
		a->last_ref = b->last_ref;

		free(b);
		heap[1] = a;
		top_to_down(heap, n, 1);
	}

	{
		struct leaf_node *p;

		for (p = heap[1]->list ; p != 0 ;)
		{
			struct leaf_node *next = p->next;
			free(p);
			p = next;
		}
	}
	
	return total;
}

int get_canonical_huffman(int f[], int len[], int seq[], int n)
{
	int *deep = (int *)malloc(sizeof(int) * n);
	int i, j;
	int seq_i = 0;

	construct_deep_array(f, n, deep);

	/*
	for (i = 0 ; i < n ; i++)
		printf("%d ", deep[i]);
	printf("\n");
	*/
	for (i = 0 ; i < n ; i++)
		if (f[i] > 0 && deep[i] == 0) deep[i] = 1;
	for (i = 1 ; i <= 16 ; i++)
	{
		int t = 0;
		for (j = 0 ; j < n ; j++)
			if (deep[j] == i)
			{
				seq[seq_i++] = j;
				t++;
			}

		len[i] = t;
	}

	printf("valid numbers: %d\n", seq_i);

	free(deep);

	return seq_i;
}

/*
int main()
{
	int f[] = {1, 2, 4, 5, 3, 2, 0, 0 ,1};
	int len[17];
	int seq[sizeof(f) / 4];
	int i;
	int n;

	n = get_canonical_huffman(f, len, seq, sizeof(f) / 4);

	for (i = 0 ; i < 16 ; i++)
		printf("%d ", len[i]);
	printf("\n");

	for (i = 0 ; i < n ; i++)
		printf("%d ", seq[i]);
	printf("\n");

	{
		uint_8 len;
		uint_16 code;
		get_huffman_code(104, &len, &code);
		printf("%d 0x%04x\n", len, code);
	}
	return 0;
}
*/

