#ifndef _DEF_H
#define _DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define uint_8 unsigned char
#define uint_16 unsigned short
#define uint_32 unsigned int
#define bool unsigned char

#include "huffman.h"
#include "process.h"

void revert(void *src, void *dst, size_t t);

/* used to control what quantization table to use, see "process.c" */
#define GOOD

#endif