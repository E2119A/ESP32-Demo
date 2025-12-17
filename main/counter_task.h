#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

BaseType_t counter_task_start(QueueHandle_t queue);
