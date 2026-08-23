#pragma once

#include <stddef.h>

#define MAGIC_BYTE 0x5A
#define PKT_SIZE 16

extern int t_stop;
void *dma_alloc(size_t size);

void* dma_slave_alloc(size_t size);