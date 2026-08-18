#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "spi_pipeline/spi_master.h"

#include "forge_err.h"
#include "forge_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "MAIN_MASTER";

void app_main(void) {

    
    printMutex = xSemaphoreCreateMutex();
    
    esp_err_t err;

    CHECK_ERR(err = init_spi_bus(), return);
    CHECK_ERR(err = init_spi_devs(), return);

    vTaskDelay(pdMS_TO_TICKS(3000));
    
    CHECK_ERR(err = master_transmit_task(), return);

}