#pragma once

#include "esp_err.h"


esp_err_t init_spi_bus(void);
esp_err_t init_spi_devs(void);
esp_err_t master_trasmit(void);
