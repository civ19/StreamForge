#include "forge_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include <assert.h>

SemaphoreHandle_t printMutex = NULL;


void mutex_log(char type, const char *tag, const char *format, ...) {

    assert(tag != NULL);
    assert(format != NULL);
    esp_log_level_t level;

    switch (type) {
        case 'E': level = ESP_LOG_ERROR; break;
        case 'W': level = ESP_LOG_WARN;  break;
        case 'I': level = ESP_LOG_INFO;  break;
        case 'D': level = ESP_LOG_DEBUG; break;
        default:  level = ESP_LOG_INFO; break;
    }

    char new_format[256];
    snprintf(new_format, sizeof(new_format), "%s\n", format);

    va_list args;
    va_start(args, format);

    if (printMutex == NULL) {
        esp_log_writev(level, tag, new_format, args);
    }
    else if (xSemaphoreTake(printMutex, portMAX_DELAY) == pdTRUE) {
        esp_log_writev(level, tag, new_format, args);
        xSemaphoreGive(printMutex);
    }

    va_end(args);
}