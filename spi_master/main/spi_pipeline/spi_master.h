#pragma once

#include "esp_err.h"
#include "driver/spi_master.h"


esp_err_t init_spi_bus(void);
esp_err_t init_spi_devs(void);
esp_err_t master_transmit_task(void);

spi_transaction_t init_trans(uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size);
esp_err_t check_bufs(uint8_t* tx_buf, uint8_t* rx_buf, const char* msg);


