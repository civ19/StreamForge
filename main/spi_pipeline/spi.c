#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "utils/forge_err.h"
#include "utils/forge_log.h"

#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS GPIO_NUM_10

static const char *TAG = "SPI";

spi_device_handle_t master_handle; 

esp_err_t init_spi_bus(void) {
    spi_bus_config_t bus_conf = {
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .sclk_io_num = SCLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .max_transfer_sz = 1024,
    };

    esp_err_t ret;

    
    CHECK_ERR(ret = spi_bus_initialize(SPI2_HOST, &bus_conf, SPI_DMA_CH_AUTO), return ret); //error checking

    mutex_log('E', TAG, "SPI Physical Bus Initialized Successfully.");

    return ESP_OK;
}

esp_err_t init_spi_devs(void) {
    spi_device_interface_config_t dev_conf = {
        .clock_speed_hz = 1 * 1000 * 1000, //1mhz
        .mode = 0,
        .spics_io_num = CS, //cs gpio
        .queue_size = 1, 
    };

    esp_err_t ret;

    CHECK_ERR(ret = spi_bus_add_device(SPI2_HOST, &dev_conf, &master_handle), return ret);

    mutex_log('I', TAG, "SPI Dev Interface Initialized Successfully.");

}