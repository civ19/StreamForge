#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t empty_queue;
extern QueueHandle_t full_queue;

#define PKT_SIZE 16 //16 bytes
#define t_n 2



void buf_setup(void);
void consumer_task(void* pv);
void print_stress_results(void);