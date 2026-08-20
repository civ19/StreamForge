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

    return ESP_OK;
}

uint8_t scale_data(size_t N) { //scalability function
    uint8_t n = 0; //number inside each tx_data val

    uint8_t tx_data[N]; //N being how many bytes for the array

    for(int i = 0; i<N; i++) { //0x01 = 1
        tx_data[i] = (uint8_t)n;
        n++;
    }

    return tx_data;
}

void scale_buf_alloc(uint8_t* tx_buf, uint8_t* rx_buf, size_t n_bufs, size_t bytes) {
    
    esp_err_t ret;

    for(int i = 0; i<n_bufs; i++) { //allocating the bytes. n_bufs ius number of buffers were doing, which is equal to the trasnactions numnber
        tx_buf[i] = dma_alloc(bytes);
        rx_buf[i] = dma_alloc(bytes);

        ret = check_bufs(tx_buf[i], rx_buf[i], "dma_alloc Pre loop failed. No Memory.");
        if (ret != ESP_OK) return ret;
    }
}

spi_transaction_t scale_trans(size_t N, uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size) {
    spi_transaction_t _trans[N];

    for(int i = 0; i<N; i++) init_trans(tx_buf[i], rx_buf[i], p_size);
}

esp_err_t master_transmit_task(void)
{
    size_t packet_size = 16; //16 for 16 bytes
    size_t t_n = 2; //number of transactions
    esp_err_t ret;

    spi_transaction_t _trans[t_n]; //array of transactions
    uint8_t tx_data[packet_size];
    uint8_t *tx_buf[t_n]; //ptrs to the buffers were gonna allocate later
    uint8_t *rx_buf[t_n];

    tx_data[packet_size] = scale_data(packet_size); //entire tx_data array for inf transactions

    //now allocating trhe buffers

    scale_buf_alloc(tx_buf, rx_buf, t_n, packet_size);


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