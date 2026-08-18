#include "spi_master.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <string.h>
#include "esp_log.h"
#include <assert.h>

#include "forge_err.h"
#include "forge_log.h"
#include "dma_master.h"

//esp32s3
// ESP32-S3 WROOM N8R8 — SPI master
#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS   GPIO_NUM_14

static const char *TAG = "SPI_MASTER";

spi_device_handle_t master_handle; 

esp_err_t init_spi_bus(void) {
    spi_bus_config_t bus_conf = {};
    bus_conf.mosi_io_num = MOSI;
    bus_conf.miso_io_num = MISO;
    bus_conf.sclk_io_num = SCLK;
    bus_conf.quadhd_io_num = -1;
    bus_conf.quadwp_io_num = -1;
    bus_conf.max_transfer_sz = 1024;
    

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

    return ESP_OK;

}

esp_err_t master_transmit_task(void)
{
    const size_t packet_size = 16;

    uint8_t tx_data[16] = {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x10
    };

    uint8_t *tx_buf = dma_alloc(packet_size);
    uint8_t *rx_buf = dma_alloc(packet_size);

    for(;;) {

        if (tx_buf == NULL || rx_buf == NULL) { //validation check 
            mutex_log('E', TAG, "DMA allocation failed");
            free(tx_buf);
            free(rx_buf);
            return ESP_ERR_NO_MEM;
        }

        memcpy(tx_buf, tx_data, packet_size);

        memset(rx_buf, 0x00, packet_size);

        spi_transaction_t _trans = {};
        _trans.tx_buffer = tx_buf;
        _trans.rx_buffer = rx_buf;
        _trans.length = packet_size * 8;

        esp_err_t ret;

        spi_transaction_t *trans_addr = &_trans;

        CHECK_ERR(ret = spi_device_queue_trans(master_handle,&_trans, portMAX_DELAY), return ret);

        CHECK_ERR( ret = spi_device_get_trans_result(master_handle,&trans_addr,portMAX_DELAY), return ret);

        ESP_LOG_BUFFER_HEX(TAG, tx_buf, packet_size);

        memset(tx_buf, 0x00, packet_size); //clearing buf to then reuse

        

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    free(tx_buf);
        free(rx_buf);

    return ESP_OK;
}