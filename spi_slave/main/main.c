#include "spi_pipeline/spi_slave.h"
#include "esp_err.h"
#include "buffering/buf_mgr.h"

#include "forge_err.h"
#include "forge_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG = "MAIN_SLAVE";

void app_main(void) {

    printMutex = xSemaphoreCreateMutex();

    esp_err_t err;

    CHECK_ERR(err = init_slave_bus(), return);
    
    buf_setup();
    BaseType_t ret = xTaskCreatePinnedToCore(slave_transmit_task, "SlaveTransmit", 8192, NULL, 5, NULL, 1);
    if(ret != pdPASS) return;

    ret = xTaskCreatePinnedToCore(consumer_task, "ConsumerTask", 8192, NULL, 4, NULL, 1);
    if(ret != pdPASS) return;


}