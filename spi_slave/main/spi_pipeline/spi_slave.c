#include "spi_slave.h"

#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "buffering/buf_mgr.h"

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
    slave_cfg.queue_size = 2;
    slave_cfg.spics_io_num = CS;

    CHECK_ERR(ret = spi_slave_initialize(SPI2_HOST, &slave_bus_conf, &slave_cfg, SPI_DMA_CH_AUTO), return ret);

    mutex_log('I', TAG, "SPI Slave dev initialized.");
    return ESP_OK;


}

spi_slave_transaction_t init_trans(uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size) { 

    spi_slave_transaction_t _trans = {};
    _trans.tx_buffer = tx_buf;
    _trans.rx_buffer = rx_buf;
    _trans.length = p_size * 8;

    return _trans; 
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

esp_err_t scale_buf_alloc(uint8_t** tx_buf, uint8_t** rx_buf, size_t n_bufs, size_t bytes) {
    
    esp_err_t ret;

    for(int i = 0; i<n_bufs; i++) { //allocating the bytes. n_bufs ius number of buffers were doing, which is equal to the trasnactions numnber
        tx_buf[i] = dma_alloc(bytes);
        rx_buf[i] = dma_alloc(bytes);

        ret = check_bufs(tx_buf[i], rx_buf[i], "dma_alloc Pre loop failed. No Memory.");
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

void slave_transmit_task(void *pv) {

    size_t packet_size = 16;
    //size_t t_n = 2; //expecting t_n trasnactions from master
    esp_err_t ret;

    Buffer *empty_buf = NULL; //1 buffer with tx and rx ptrs

    spi_slave_transaction_t _trans; //arr of slave transactions
    spi_slave_transaction_t *trans_addr = &_trans;


    mutex_log('I', TAG, "Ownership to DMA. Filling buffer...");

    for(;;) {

        if(xQueueReceive(empty_queue, &empty_buf, pdMS_TO_TICKS(1000))) {

            memset(empty_buf->tx_buf, 0x00, packet_size); //clears individual elts inside the tx data array
            memset(empty_buf->rx_buf, 0x00, packet_size); 
            

            //getting addr and _tranbs generation/conf
            _trans = init_trans(empty_buf->tx_buf, empty_buf->rx_buf, packet_size);
        
            _trans.user = (void*)empty_buf; //keeping it in a safe place so we know were sending out empty buf

            CHECK_ERR(ret = spi_slave_queue_trans(SPI2_HOST, &_trans, portMAX_DELAY), vTaskDelete(NULL));
            
            mutex_log('I', TAG, "All transactions successfully queued. Results incoming...");

            //getting slave result
            ret = spi_slave_get_trans_result(SPI2_HOST, &trans_addr, pdMS_TO_TICKS(1000));
            if(ret == ESP_ERR_TIMEOUT) {
                mutex_log('W', TAG, "Producer Transaction timeout. Requeuing...");
                xQueueSend(empty_queue, &empty_buf, 0);
                continue;
            }

            else if(ret != ESP_OK) {
                mutex_log('E', TAG, "Fatal SPI Hardware Transaction Error: 0x%X (%s)", ret, esp_err_to_name(ret));
                vTaskDelete(NULL);
            }
            Buffer *finished_buf = (Buffer *)_trans.user; 

            xQueueSend(full_queue, &finished_buf, 0);
            
            ESP_LOG_BUFFER_HEX(TAG, empty_buf->rx_buf, packet_size);

        } else  mutex_log('W', TAG, "SPI Transaction Timeout! Re-queuing buffer...");
          
        
       
    }


   
}