#pragma once

#include <stddef.h>

#define MAGIC_BYTE 0x5A

void *dma_alloc(size_t size);

void* dma_slave_alloc(size_t size);