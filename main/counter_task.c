#include "counter_task.h"

#include "freertos/task.h"

#include "esp_log.h"

static const char *TAG = "COUNTER";

#define COUNTER_PERIOD_MS    5000
#define QUEUE_SEND_TIMEOUT_MS 100

static void counter_task(void *pvParameters)
{
    QueueHandle_t queue = (QueueHandle_t)pvParameters;

    int counter = 0;
    const TickType_t delay_ticks = pdMS_TO_TICKS(COUNTER_PERIOD_MS);
    const TickType_t send_timeout = pdMS_TO_TICKS(QUEUE_SEND_TIMEOUT_MS);

    ESP_LOGI(TAG, "counter_task started");

    while (true) {
        counter++;

        // Short timeout so we don’t block forever if the logger task is stuck.
        if (xQueueSend(queue, &counter, send_timeout) != pdPASS) {
            ESP_LOGW(TAG, "queue full, value %d dropped", counter);
        } else {
            ESP_LOGI(TAG, "sent value %d", counter);
        }

        vTaskDelay(delay_ticks);
    }
}

BaseType_t counter_task_start(QueueHandle_t queue)
{
    return xTaskCreate(
        counter_task,
        "counter_task",
        2048,          // stack size in words. enough for this task
        (void *)queue, // queue handle passed via task parameter
        5,             // same priority as logger so they stay balanced
        NULL);
}


