#include "unity.h"
#include "fff.h"
#include "app_types.h"
#include "forge_log.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "spi_master.h"

FAKE_VALUE_FUNC(void *, dma_alloc, size_t);


void setUp(void) {
    RESET_FAKE(dma_alloc);
}

void tearDown(void) {
    
}

void test_dma_alloc_returns_null_cleanly(void) {
    //ARRANGE
    dma_alloc_fake.return_val = NULL; //should return null
    
    
    uint8_t *tx[1] = {0};
    uint8_t *rx[1] = {0};

    esp_err_t ret = scale_buf_alloc(tx, rx, 1, 16);
    TEST_ASSERT_EQUAL_HEX32(ESP_ERR_NO_MEM, ret);

}

void test_wrong_magic_byte_drops(void) {
    //ARRANGE
    
    QueueHandle_t empty_queue = xQueueCreate(2, sizeof(AppBuffer *)); //size of ptrs to bufs
    QueueHandle_t full_queue = xQueueCreate(2, sizeof(AppBuffer *)); 

    uint8_t rx_data[2] = {MAGIC_BYTE, 0x01};
    uint8_t *rx = rx_data;

    AppBuffer *mock_buf = {.rx_buf = rx};

    if(xQueueSend(empty_queue, &mock_buf, portMAX_DELAY)) {
        ESP_LOGI("MOCK_TEST", "Mock Buf Sent");
    }




    
}
/*
Test 1: Absolute Packet Drop (Invalid Magic Byte)

What it tests: A packet arrives with correct CRC but Byte 0 is missing the 0x5A token
 (simulating a loose connection or bit-shift alignment error).

Expected behavior: The application must instantly discard the block, 
execute zero business logic, and recycle the exact same pointer back to the empty_queue
*/

