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
    

    uint8_t tx_data[N]; //N being how many bytes for the array

    for(int i = 0; i<N; i++) { //0x01 = 1
        tx_data[i] = (uint8_t)val;
        val++;
    }

    return tx_data;
}

esp_err_t scale_buf_alloc(uint8_t* tx_buf, uint8_t* rx_buf, size_t n_bufs, size_t bytes) {
    
    esp_err_t ret;

    for(int i = 0; i<n_bufs; i++) { //allocating the bytes. n_bufs ius number of buffers were doing, which is equal to the trasnactions numnber
        tx_buf[i] = dma_alloc(bytes);
        rx_buf[i] = dma_alloc(bytes);

        ret = check_bufs(tx_buf[i], rx_buf[i], "dma_alloc Pre loop failed. No Memory.");
        if (ret != ESP_OK) return ret;
    }
}


spi_transaction_t scale_trans(size_t t_n, uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size) {
    
    spi_transaction_t _trans[t_n];

    for(int i = 0; i<t_n; i++) {
        _trans[i] = init_trans(tx_buf[i], rx_buf[i], p_size);
    }

    return _trans;
}

esp_err_t master_transmit_task(void)
{
    size_t packet_size = 16; //16 for 16 bytes
    size_t t_n = 2; //number of transactions
    esp_err_t ret;

    spi_transaction_t _trans[t_n]; //arr of transactions
    spi_transaction_t *trans_addr[t_n];

    uint8_t tx_data[packet_size];
    uint8_t *tx_buf[t_n]; //ptrs to the buffers were gonna allocate later
    uint8_t *rx_buf[t_n];


    //initializing and setting tx_data for scalability
    uint8_t val = 0; //num inside each tx data val
    for(int i = 0; i<t_n; i++) { //0x01 = 1
        tx_data[i] = (uint8_t)val;
        val++;
    }

    //now allocating trhe buffers
    scale_buf_alloc(tx_buf, rx_buf, t_n, packet_size);

    //checking if any of the buf allocs failed
    for(int i = 0; i<t_n; i++) { //0x01 = 1
        ret = check_bufs(tx_buf, rx_buf, "dma_alloc Pre loop failed. No Memory.");
        if (ret != ESP_OK) return ret;
    }


    for(int i = 0; i<t_n; i++) { //initializing transactions and configuring them
        _trans[i] = init_trans(tx_buf[i], rx_buf[i], packet_size);
    }

    //clearing rx bufs and copying tx bufs
    for(int i = 0; i<t_n; i++) {
        memcpy(tx_buf[i], tx_data[i], t_n);
        memcpy(rx_buf[i], 0x00, t_n);
    }

    for(;;) {
        

        
        


        for(int i = 0; i<t_n; i++) { //queuing all transactions and getting the addr of all
            CHECK_ERR(ret = spi_device_queue_trans(master_handle,&_trans[i], portMAX_DELAY), return ret);
            trans_addr[i] = &_trans[i];
        }
        mutex_log('I', TAG, "All transactions successfully queued. Results incoming...");

        for(int i = 0; i<t_n; i++) { //getting reuslt 
            CHECK_ERR(ret = spi_device_get_trans_result(master_handle,&trans_addr[i], portMAX_DELAY), return ret);
            printf("Transaction buffer %d: ", t_n);
            ESP_LOG_BUFFER_HEX(TAG, tx_buf[i], packet_size);
        }

        //resetting buffers so we can reuse them
        for(int i = 0; i<t_n; i++) {
            memcpy(tx_buf[i], 0x00, packet_size);
            memcpy(rx_buf[i], 0x00, packet_size);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    for(int i = 0; i<t_n; i++) { //freeing
        free(tx_buf[i]);
        free(rx_buf[i]);
    }

    return ESP_OK;
}