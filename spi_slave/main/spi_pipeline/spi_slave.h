#pragma once

#include "esp_err.h"
#include "driver/spi_slave.h"
#include "app_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"



esp_err_t init_slave_bus(void);
void slave_transmit_task(void* pv);

void init_trans(spi_slave_transaction_t *_trans, uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size);
esp_err_t check_bufs(uint8_t* tx_buf, uint8_t* rx_buf, const char* msg);
esp_err_t scale_buf_alloc(uint8_t** tx_buf, uint8_t** rx_buf, size_t n_bufs, size_t bytes);
esp_err_t init_slave_engine_bufs(QueueHandle_t to_empty_queue);
