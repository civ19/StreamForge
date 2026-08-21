#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t empty_queue;
extern QueueHandle_t full_queue;

void buf_setup(void);