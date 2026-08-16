#pragma once

#include "freertos/FreeRTOS.h" 
#include "freertos/semphr.h"
#include "esp_err.h"

extern SemaphoreHandle_t printMutex;

void mutex_log(char type, const char *tag, const char *format, ...);