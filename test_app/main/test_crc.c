#include "unity.h"
#include "fff.h"
#include "app_types.h"
#include "forge_log.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "spi_master.h"
#include "buf_mgr.h"

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
    
    uint8_t mock_rx_data[PKT_SIZE] = {0};
    uint8_t mock_tx_data[PKT_SIZE] = {0};

    mock_rx_data[0] = 0x3A;
    mock_rx_data[1] = 0x05;

    AppBuffer mock_buf = {
        .id = 1,
        .rx_buf = mock_rx_data,
        .tx_buf = mock_tx_data
    };

    bool mock_sync_state = false;

    //ACT
    bool result = process_received_packet(&mock_buf, &mock_sync_state);
    
    //ASSERT
    TEST_ASSERT_FALSE_MESSAGE(result, "Streamforge failed to drop a packet with a bad magic byte!");

    for(int i = 0; i<PKT_SIZE; i++) {
        TEST_ASSERT_EQUAL(0x00,mock_rx_data[i]);
        TEST_ASSERT_EQUAL(0x00, mock_tx_data[i]);
    }


    
}
/*
Test 1: Absolute Packet Drop (Invalid Magic Byte)

What it tests: A packet arrives with correct CRC but Byte 0 is missing the 0x5A token
 (simulating a loose connection or bit-shift alignment error).

Expected behavior: The application must instantly discard the block, 
execute zero business logic, and recycle the exact same pointer back to the empty_queue
*/

