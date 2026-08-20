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
        .queue_size = 2, 
    };

    esp_err_t ret;

    CHECK_ERR(ret = spi_bus_add_device(SPI2_HOST, &dev_conf, &master_handle), return ret);

    mutex_log('I', TAG, "SPI Dev Interface Initialized Successfully.");

    return ESP_OK;

}

spi_transaction_t init_trans(uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size) { //since i want the ptr to the buffers not the values in it

    spi_transaction_t _trans = {};
    _trans.tx_buffer = tx_buf;
    _trans.rx_buffer = rx_buf;
    _trans.length = p_size * 8;

    return _trans; //also this is just a helper function to generate transactions. will be good for scale/when you want alot of transactions asyncrhonously

}

esp_err_t check_bufs(uint8_t* tx_buf, uint8_t* rx_buf, const char* msg) {

    if (tx_buf == NULL || rx_buf == NULL) { //validation check 
        mutex_log('E', TAG, msg);
        free(tx_buf);
        free(rx_buf);
        return ESP_ERR_NO_MEM;
    }
}

void manage_trans(size_t N) { //scalability function
    uint8_t n = 0; //number inside each tx_data val

    uint8_t tx_data[N]; 

    for(int i = 0; i<N; i++) { //0x01 = 1
        tx_data[i] = (uint8_t)n;
        n++;

    }


}
esp_err_t master_transmit_task(void)
{
    size_t packet_size = 16;
    size_t t_n = 2; //number of transactions
    esp_err_t ret;

    spi_transaction_t _trans[t_n]; //array of transactions

    uint8_t tx_d1[16] = {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x10
    };

    uint8_t tx_d2[16] = {
        0x11, 0x22, 0x33, 0x44,
        0x05, 0xe6, 0xe7, 0x08,
        0x09, 0xeA, 0x0B, 0x0C,
        0x0D, 0x0E, 0xeF, 0x10
    };

    uint8_t *tx_buf1 = dma_alloc(packet_size); //allocations
    uint8_t *rx_buf1 = dma_alloc(packet_size);

    ret = check_bufs(tx_buf1, rx_buf1, "dma_alloc Pre loop failed. No Memory.");
    if (ret != ESP_OK) return ret;

    uint8_t *tx_buf2 = dma_alloc(packet_size);
    uint8_t *rx_buf2 = dma_alloc(packet_size);

    ret = check_bufs(tx_buf2, rx_buf2, "dma_alloc Pre loop failed. No Memory.");
    if (ret != ESP_OK) return ret;

    for(;;) {

        
        memcpy(tx_buf1, tx_d1, packet_size); //copyinh 
        memset(rx_buf1, 0x00, packet_size);

        memcpy(tx_buf2, tx_d2, packet_size);
        memset(rx_buf2, 0x00, packet_size);
     
        

        spi_transaction_t _trans1 = init_trans(tx_buf1, rx_buf1, packet_size);
        spi_transaction_t *t1_addr = &_trans1; //trans1 addr

        spi_transaction_t _trans2 = init_trans(tx_buf2, rx_buf2, packet_size);
        spi_transaction_t *t2_addr = &_trans2; //trans2 addr


        CHECK_ERR(ret = spi_device_queue_trans(master_handle,&_trans1, portMAX_DELAY), return ret);
        CHECK_ERR(ret = spi_device_queue_trans(master_handle,&_trans2, portMAX_DELAY), return ret);

        mutex_log('I', TAG, "All transactions successfully queued. Results incoming...");

        CHECK_ERR( ret = spi_device_get_trans_result(master_handle,&t1_addr,portMAX_DELAY), return ret);
        CHECK_ERR( ret = spi_device_get_trans_result(master_handle,&t2_addr,portMAX_DELAY), return ret);

        ESP_LOG_BUFFER_HEX(TAG, tx_buf1, packet_size);
        ESP_LOG_BUFFER_HEX(TAG, tx_buf2, packet_size);

        memset(tx_buf1, 0x00, packet_size); //clearing buf to then reuse
        memset(tx_buf2, 0x00, packet_size); //clearing buf to then reuse

        
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    free(tx_buf1);
    free(tx_buf2);

    free(rx_buf1);
    free(rx_buf2);

    return ESP_OK;
}