#include "logger_task.h"

#include "freertos/task.h"

#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "LOGGER";

static void logger_task(void *pvParameters)
{
    QueueHandle_t queue = (QueueHandle_t)pvParameters;

    int value = 0;
    int64_t last_time_us = 0;

    ESP_LOGI(TAG, "logger_task started");

    while (true) {
        // Block until we receive something to log
        if (xQueueReceive(queue, &value, portMAX_DELAY) == pdTRUE) {
            int64_t now_us = esp_timer_get_time();

            if (last_time_us == 0) {
                ESP_LOGI(TAG, "received %d (first value)", value);
            } else {
                // Use esp_timer so timing is not tied to the FreeRTOS tick rate
                int64_t delta_ms = (now_us - last_time_us) / 1000;
                ESP_LOGI(TAG, "received %d, interval since last = %" PRId64 " ms",
                         value, delta_ms);
            }

            last_time_us = now_us;
        }
    }
}

BaseType_t logger_task_start(QueueHandle_t queue)
{
    return xTaskCreate(
        logger_task,
        "logger_task",
        2048,
        (void *)queue, // queue passed via task parameter
        5,             // same as counter so neither starves the other
        NULL);
}


