#include "buf_mgr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "utils/forge_err.h"
#include "utils/forge_log.h"
#include <string.h>


static Buffer bufs[2];

static const char* TAG = "BUFFERING";


}

void consumer_task(void *pv) {
    Buffer *finished_buf = NULL;

    for(;;) {

        mutex_log('E', TAG, "Ownership to CPU. Clearing bufs.");
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
