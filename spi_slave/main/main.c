#include "spi_pipeline/spi_slave.h"
#include "esp_err.h"

#include "forge_err.h"
#include "forge_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char* TAG = "MAIN_SLAVE";

void app_main(void) {
    


    printMutex = xSemaphoreCreateMutex();

    esp_err_t err;

    CHECK_ERR(err = init_slave_bus(), return);
    
    CHECK_ERR(err = slave_transmit_task(), return);

}