#include "dma.h"

#include "driver/gdma.h"

static const char *TAG = "DMA";

esp_err_t init_dma(void) {

    mutex_log('I', TAG, "SPI DMA Init successfully");

    return ESP_OK;
}