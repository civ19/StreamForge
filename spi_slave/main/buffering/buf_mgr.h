#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "app_types.h"

extern QueueHandle_t empty_queue;
extern QueueHandle_t full_queue;

#define PKT_SIZE 16 //16 bytes
#define t_n 2



void buf_setup(void);
bool process_received_packet(AppBuffer *finished_buf, bool *synced);
void consumer_task(void* pv);
void print_stress_results(void);