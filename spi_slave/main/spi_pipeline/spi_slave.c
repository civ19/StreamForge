#include "spi_slave.h"

#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "forge_err.h"
#include "forge_log.h"
#include "dma_master.h"

//esp32 base
#define MOSI GPIO_NUM_23
#define MISO GPIO_NUM_19
#define SCLK GPIO_NUM_18
#define CS   GPIO_NUM_5

static const char *TAG = "SPI_SLAVE";


esp_err_t init_slave_bus(void) {
    spi_bus_config_t slave_bus_conf = {};
    slave_bus_conf.mosi_io_num = MOSI;
    slave_bus_conf.miso_io_num = MISO;
    slave_bus_conf.max_transfer_sz = 1024;
    slave_bus_conf.quadhd_io_num = -1;
    slave_bus_conf.quadwp_io_num = -1;
    slave_bus_conf.sclk_io_num = SCLK;

    mutex_log('I', TAG, "SPI Slave bus initialized.");
    esp_err_t ret;

     

    spi_slave_interface_config_t slave_cfg = {};
    slave_cfg.mode = 0;
    slave_cfg.queue_size = 1;
    slave_cfg.spics_io_num = CS;

    CHECK_ERR(ret = spi_slave_initialize(SPI2_HOST, &slave_bus_conf, &slave_cfg, SPI_DMA_CH_AUTO), return ret);

    mutex_log('I', TAG, "SPI Slave dev initialized.");
    return ESP_OK;


}

esp_err_t slave_transmit(void)
{
    const size_t packet_size = 16;

    while (1) {

        uint8_t *tx_buf = dma_slave_alloc(packet_size);
        uint8_t *rx_buf = dma_slave_alloc(packet_size);

        if (tx_buf == NULL || rx_buf == NULL) {
            mutex_log('E', TAG, "DMA allocation failed");
            free(tx_buf);
            free(rx_buf);
            return ESP_ERR_NO_MEM;
        }

        memset(tx_buf, 0x00, packet_size);
        memset(rx_buf, 0x00, packet_size);

        spi_slave_transaction_t _trans = {};

        _trans.length = packet_size * 8;
        _trans.tx_buffer = tx_buf;
        trans.rx_buffer = rx_buf;

        esp_err_t ret;

        spi_slave_transaction_t *trans_addr = &_trans;

        CHECK_ERR(ret = spi_slave_queue_trans(SPI2_HOST,&trans,portMAX_DELAY), return ret);

        CHECK_ERR(
            ret = spi_slave_get_trans_result(SPI2_HOST, &trans_addr, portMAX_DELAY),return ret);

        ESP_LOG_BUFFER_HEX(TAG, rx_buf, packet_size);

        free(tx_buf);
        free(rx_buf);

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    return ESP_OK;
}