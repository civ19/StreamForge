#include "buf_mgr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"

#include "dma_mgr.h"
#include "app_types.h"
#include "forge_err.h"
#include "forge_log.h"
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "spi_slave.h"



static const char* TAG = "BUFFERING";

static int64_t pkt_rec = 0;
static int64_t pkt_valid = 0;
static int64_t seq_err = 0;
static int64_t pkt_malf = 0;
static int64_t task_timeout = 0;

size_t start_heap = 0;
size_t runtime_baseline_heap = 0;

static uint8_t exp_seq = 1;

static const int64_t test_dur = 30000000;

QueueHandle_t empty_queue = NULL;
QueueHandle_t full_queue = NULL;

void consumer_task(void *pv) {
    AppBuffer *finished_buf = NULL;
    
    uint8_t received_seq;
    
    bool synced = false;
    bool valid = true;


    while(test_dur - esp_timer_get_time() > 0) {

        valid = true;
        

        mutex_log('I', TAG, "Ownership to CPU. Clearing bufs.");
        if(xQueueReceive(full_queue, &finished_buf, pdMS_TO_TICKS(1000))) {
            pkt_rec++;

            received_seq = finished_buf->rx_buf[1];

            if(synced == false) {
                exp_seq = received_seq;
                synced = true;
                mutex_log('I', TAG, "First packet caught! Synced sequence marker.");
            }

            if(received_seq != exp_seq) {
                mutex_log('W', TAG, "Sequences Malform[DMA]! Expected Seq %d but got %d. Attempting resync...", exp_seq, received_seq);
                exp_seq = received_seq;
                valid = false;
                seq_err++;
            }
            
            exp_seq++;

            if(finished_buf->rx_buf[0] != MAGIC_BYTE) {
                mutex_log('W', TAG, "Malformed packet detected! Dropping frame.");
                memset(finished_buf->rx_buf, 0x00, PKT_SIZE);
                memset(finished_buf->tx_buf, 0x00, PKT_SIZE);
                pkt_malf++;

                xQueueSend(empty_queue, &finished_buf, 0);
                continue;
            }
            memset(finished_buf->rx_buf, 0x00, PKT_SIZE);
            memset(finished_buf->tx_buf, 0x00, PKT_SIZE);

            mutex_log('E', TAG, "CPU Clearing Complete! Transferring to DMA.");
            if(valid) pkt_valid++;
            xQueueSend(empty_queue, &finished_buf, 0);
        } else { 
            mutex_log('E', TAG, "Timeout failed. No new buffer on time to CPU. Continuing pipeline...");
            task_timeout++;
        }
            
        
    }

    print_stress_results();

    vTaskDelete(NULL);

    
}
    
void print_stress_results(void) {
    size_t end_heap = heap_caps_get_free_size(MALLOC_CAP_DMA); //size in kb


    printf("Recieved packets: %" PRId64 "\n", pkt_rec);
    printf("Valid packets: %" PRId64 "\n", pkt_valid);
    printf("Sequence break packets: %" PRId64 "\n", seq_err);
    printf("Malformed packets: %" PRId64 "\n", pkt_malf);
    printf("Task timeouts: %" PRId64 "\n\n", task_timeout);

    printf("--- StreamForge Memory Analytics ---\n");
    printf("Fixed Infrastructure Cost:  %zu bytes\n", (start_heap - runtime_baseline_heap));
    printf("Sustained Processing Leak:  %zd bytes\n", (ssize_t)(runtime_baseline_heap - end_heap));
    printf("------------------------------------\n");
}
    


void buf_setup(void) {
    start_heap = heap_caps_get_free_size(MALLOC_CAP_DMA);

    empty_queue = xQueueCreate(2, sizeof(AppBuffer *)); //size of ptrs to bufs
    full_queue = xQueueCreate(2, sizeof(AppBuffer *)); 
    
    init_slave_engine_bufs(empty_queue);


}
