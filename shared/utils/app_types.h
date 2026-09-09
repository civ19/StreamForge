#pragma once
#include <inttypes.h>

#include "dma_mgr.h"

typedef struct {
    uint8_t id;
    uint8_t *tx_buf;
    uint8_t *rx_buf;
} AppBuffer;