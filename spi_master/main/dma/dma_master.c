#include "dma_master.h"
#include "forge_err.h"
#include "forge_log.h"

#include "esp_heap_caps.h"

static const char *TAG = "DMA";


void *dma_alloc(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
}

esp_err_t init_dma(void) {

    mutex_log('I', TAG, "SPI DMA Init successfully");

    return ESP_OK;
}