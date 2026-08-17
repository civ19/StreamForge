#pragma once

#include <stddef.h>
#include "esp_err.h"

esp_err_t dma_master_init(void);

void *dma_alloc(size_t size);