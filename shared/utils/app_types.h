#pragma once
#include <inttypes.h>

#include "dma_mgr.h"

typedef struct {
    uint8_t id;
    uint8_t *tx_buf[PKT_SIZE];
    uint8_t *rx_buf[PKT_SIZE];
} AppBuffer;