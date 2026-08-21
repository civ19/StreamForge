#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t empty_queue;
extern QueueHandle_t full_queue;

#define PKT_SIZE 16 //16 bytes

typedef struct {
    uint8_t *tx_buf; //ptr to a uint8_t data arr
    uint8_t *rx_buf;
} Buffer;

void buf_setup(void);