#pragma once

#include "esp_err.h"
#include "driver/spi_slave.h"

esp_err_t init_slave_bus(void);
esp_err_t slave_transmit_task(void);

spi_transaction_t init_trans(uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size);
esp_err_t check_bufs(uint8_t* tx_buf, uint8_t* rx_buf, const char* msg);
esp_err_t scale_buf_alloc(uint8_t** tx_buf, uint8_t** rx_buf, size_t n_bufs, size_t bytes);
