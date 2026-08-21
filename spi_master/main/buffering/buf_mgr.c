#include "buf_mgr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "forge_err.h"
#include "forge_log.h"
#include <string.h>

#define PKT_SIZE 16 //16 bytes

static Buffer bufs[2];

typedef struct {
    int pos;
    uint8_t data[PKT_SIZE];
} Buffer;

void producer_task(void *pv) {
    Buffer* buf;

    for(;;) {

    }
    if(xQueueReceive(empty_queue, &buf, portMAX_DELAY)) {

        for(int i=0; i<PKT_SIZE; i++) {
            buf->data[i] = 
        }
    }
}

void consumer_task(void *pv) {
    Buffer* buf; //ptr to int arr

    for(;;) {
        if(xQueueReceive(full_queue, &buf, portMAX_DELAY)) {
            memset(buf->data, 0x00, PKT_SIZE);

            xQueueSend(empty_queue, &buf, 0);
        }
        
    }
}
    

    


void buf_setup(void) {
    empty_queue = xQueueCreate(2, sizeof(Buffer *)); //size of ptrs to bufs
    full_queue = xQueueCreate(2, sizeof(Buffer *)); 
    

    bufs[0].pos = 0;
    bufs[1].pos = 1;

    Buffer *buf;

    buf = &bufs[0];
    


}
