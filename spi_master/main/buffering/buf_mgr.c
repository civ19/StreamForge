#include "buf_mgr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "forge_err.h"
#include "forge_log.h"

#define PKT_SIZE 16 //16 bytes

typedef struct {
    int pos;
    uint8_t data[PKT_SIZE];
} Buffer;

void buf_setup(void) {
    
}
