#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "utils/forge_err.h"
#include "utils/forge_log.h"

#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS GPIO_NUM_10

static const char *TAG = "SPI_SLAVE";

esp_err_t init_slave_bus (void) {
    spi_bus_config_t slave_bus_conf = {};
    slave_bus_conf.mosi_io_num = MOSI;
    slave_bus_conf.miso_io_num = MISO;
    slave_bus_conf.max_transfer_sz = 1024;
    slave_bus_conf.quadhd_io_num = -1;
    slave_bus_conf.quadwp_io_num = -1;
    slave_bus_conf.sclk_io_num = SCLK;


    esp_err_t ret;
    CHECK_ERR(ret = spi_bus_initialize(SPI2_HOST, &slave_bus_conf, SPI_DMA_CH_AUTO), return ret);

    return ESP_OK;

}

