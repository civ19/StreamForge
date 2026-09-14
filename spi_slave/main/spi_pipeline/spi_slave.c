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
#include "dma_mgr.h"

#include <stddef.h>

//esp32s3
// ESP32-S3 wroom — SPI master
#define MOSI GPIO_NUM_11
#define MISO GPIO_NUM_13
#define SCLK GPIO_NUM_12
#define CS   GPIO_NUM_10


static const char *TAG = "SPI_SLAVE";

typedef struct {
    AppBuffer app_buf;
    spi_slave_transaction_t _etrans;
    
} EngineBuffer;

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



void init_trans(spi_slave_transaction_t *_trans, uint8_t *tx_buf, uint8_t *rx_buf, size_t p_size) { 

    memset(_trans, 0, sizeof(spi_slave_transaction_t)); //clearing the mem loc
    //setting the buffers
    _trans->tx_buffer = tx_buf;
    _trans->rx_buffer = rx_buf;
    _trans->length = p_size * 8;

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

esp_err_t scale_buf_alloc(uint8_t **tx_buf, uint8_t** rx_buf, size_t n_bufs, size_t bytes) {
    
    esp_err_t ret;

    for(int i = 0; i<n_bufs; i++) { //allocating the bytes. n_bufs ius number of buffers were doing, which is equal to the trasnactions numnber
        tx_buf[i] = dma_alloc(bytes);
        rx_buf[i] = dma_alloc(bytes);

        ret = check_bufs(tx_buf[i], rx_buf[i], "dma_alloc Pre loop failed. No Memory.");
        if (ret != ESP_OK) return ret;
    }

    return ESP_OK;
}

static EngineBuffer bufs[2];

esp_err_t init_slave_engine_bufs(QueueHandle_t to_empty_queue) {
    for(int i = 0; i<2; i++) {
        //allocate dma buffers
        bufs[i].app_buf.rx_buf = dma_alloc(PKT_SIZE);
        bufs[i].app_buf.tx_buf = dma_alloc(PKT_SIZE);
        bufs[i].app_buf.id = i;

        //assertions
        assert(bufs[i].app_buf.rx_buf != NULL);
        assert(bufs[i].app_buf.tx_buf != NULL);

        //setting to 0
        memset(bufs[i].app_buf.rx_buf, 0x00, PKT_SIZE); //setting a pkt_size buffer to 0
        memset(bufs[i].app_buf.tx_buf, 0x00, PKT_SIZE); //setting a pkt_size buffer to 0
        memset(&bufs[i]._etrans, 0, sizeof(spi_slave_transaction_t));

        //spi hardware config
        init_trans(&bufs[i]._etrans, bufs[i].app_buf.tx_buf, bufs[i].app_buf.rx_buf, PKT_SIZE);

        bufs[i]._etrans.user = (void*)&bufs[i].app_buf; //safe bucket

        //pushing app buf 
        AppBuffer *pv_app = &bufs[i].app_buf;
        xQueueSend(to_empty_queue, &pv_app, 0);
    }

    return ESP_OK;
}


void slave_transmit_task(void *pv) {
    printf("SLAVE TASK STARTED\n");
    esp_err_t ret;

    AppBuffer *empty_buf = NULL;         
    spi_slave_transaction_t *ret_trans = NULL; 

    //prime the hardware queue pipeline with the very first buffer
    if (xQueueReceive(empty_queue, &empty_buf, portMAX_DELAY)) {
        EngineBuffer *engine_buf = (EngineBuffer *)((char *)empty_buf - offsetof(EngineBuffer, app_buf));
        spi_slave_queue_trans(SPI2_HOST, &engine_buf->_etrans, portMAX_DELAY);
    }

    for(;;) {
        //keeping the pipeline fed: fetch and queue a back-up buffer immediately
        if (xQueueReceive(empty_queue, &empty_buf, portMAX_DELAY)) {
            EngineBuffer *engine_buf = (EngineBuffer *)((char *)empty_buf - offsetof(EngineBuffer, app_buf));
            spi_slave_queue_trans(SPI2_HOST, &engine_buf->_etrans, portMAX_DELAY);
        }

        //block until the oldest queued transaction completes
        ret = spi_slave_get_trans_result(SPI2_HOST, &ret_trans, pdMS_TO_TICKS(1000));

        if (ret == ESP_ERR_TIMEOUT) {
            continue; 
        } else if (ret != ESP_OK) {
            mutex_log('E', TAG, "Fatal Hardware SPI Transaction Error!");
            vTaskDelete(NULL);
        }

        //extract the clean application token from the finished transaction's bucket
        AppBuffer *finished_buf = (AppBuffer *)ret_trans->user;

      
        // This prints out the rx_buf array values up to PKT_SIZE in Hex format
        ESP_LOG_BUFFER_HEX("SPI_SLAVE_ENGINE", finished_buf->rx_buf, PKT_SIZE);

        //transfer the clean public token over to the application layer / CPU
        xQueueSend(full_queue, &finished_buf, 0);
    }
}
