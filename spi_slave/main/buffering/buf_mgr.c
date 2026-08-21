#include "buf_mgr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "dma_master.h"
#include "forge_err.h"
#include "forge_log.h"
#include <string.h>
#include <assert.h>


static Buffer bufs[2];
static const char* TAG = "BUFFERING";

QueueHandle_t empty_queue = NULL;
QueueHandle_t full_queue = NULL;

void consumer_task(void *pv) {
    Buffer *finished_buf = NULL;

    for(;;) {

        mutex_log('E', TAG, "Ownership to CPU. Clearing bufs.");
        if(xQueueReceive(full_queue, &finished_buf, portMAX_DELAY)) {

            ESP_LOG_BUFFER_HEX(TAG, finished_buf->rx_buf, PKT_SIZE);

            memset(finished_buf->rx_buf, 0x00, PKT_SIZE);
            memset(finished_buf->tx_buf, 0x00, PKT_SIZE);

            xQueueSend(empty_queue, &finished_buf, 0);
        }
        
    }
}
    

    


void buf_setup(void) {
    empty_queue = xQueueCreate(2, sizeof(Buffer *)); //size of ptrs to bufs
    full_queue = xQueueCreate(2, sizeof(Buffer *)); 
    
    for(int i = 0; i<2; i++) {
        bufs[i].rx_buf = dma_alloc(PKT_SIZE);
        bufs[i].tx_buf = dma_alloc(PKT_SIZE);

        assert(bufs[i].rx_buf != NULL);
        assert(bufs[i].tx_buf != NULL);

        memset(bufs[i].rx_buf, 0x00, PKT_SIZE);
        memset(bufs[i].tx_buf, 0x00, PKT_SIZE);

        Buffer *pv_buf = &bufs[i];
        xQueueSend(empty_queue, &pv_buf, 0); //ptr to buf to minimize how many mem is allocated. ptr alloc smaller than Buffer struct
    }


}
