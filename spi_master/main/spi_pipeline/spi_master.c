#include "spi_master.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <string.h>

#include "forge_err.h"
#include "forge_log.h"
#include "dma_master.h"

//esp32s3
#define MOSI GPIO_NUM_13 
#define MISO GPIO_NUM_14 
#define SCLK GPIO_NUM_9  
#define CS   GPIO_NUM_10 

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

esp_err_t master_trasmit(void) {

    size_t packet_size = 256;

    uint8_t tx_data[16] = {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x10
    };
    
    uint8_t *tx_buf = dma_alloc(packet_size); //dma buf
    
    memcpy(tx_buf, tx_data, sizeof(tx_data));
    
    uint8_t *rx_buf = dma_alloc(packet_size); //expecting 256 bytes back

    spi_transaction_t _trans = {};
    _trans.tx_buffer = tx_buf;
    _trans.rx_buffer = rx_buf;
    _trans.length = packet_size*8; //since 256 bytes per data packet, and for now im sending 8 packets


    esp_err_t ret;

    spi_transaction_t* _trans_addr = &_trans; //ptr to trans for trans result

    CHECK_ERR(ret = spi_device_queue_trans(master_handle, &_trans, portMAX_DELAY), return ret); //qeued for transmit
    CHECK_ERR(ret = spi_device_get_trans_result(master_handle, &_trans_addr, portMAX_DELAY), return ret); //actual returned result
    free(tx_buf);
    free(rx_buf);


    mutex_log('I', TAG, "SPI Device Master Transmit Successful.");

    return ESP_OK;



}

void trans_manage(uint8_t *buf, spi_transaction_t _trans) {
    spi_device_queue_trans(master_handle, &_trans, portMAX_DELAY);
    
}