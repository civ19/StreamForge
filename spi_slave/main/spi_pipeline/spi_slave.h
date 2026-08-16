#pragma once

#include "esp_err.h"

esp_err_t init_slave_bus(void);
esp_err_t slave_transmit(void);