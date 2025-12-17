#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "esp_log.h"

#include "counter_task.h"
#include "logger_task.h"
#include "led_task.h"

// LED on ESP32 DevKit V1 board.
#define BLINK_GPIO           2

// How many counter values we can store if logger runs behind.
#define COUNTER_QUEUE_LENGTH 5

static const char *TAG = "MAIN";

// Queue shared between counter (producer) and logger (consumer).
static QueueHandle_t s_counter_queue = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "app_main start");

    // Basic GPIO setup.
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BLINK_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(BLINK_GPIO, 0);

    // Queue connects two tasks without forcing them to run synchronously.
    s_counter_queue = xQueueCreate(COUNTER_QUEUE_LENGTH, sizeof(int));
    if (s_counter_queue == NULL) {
        ESP_LOGE(TAG, "failed to create queue");
        return;
    }

    // Start three tasks. app_main doesn’t care about stack/prio details.
    if (counter_task_start(s_counter_queue) != pdPASS) {
        ESP_LOGE(TAG, "failed to start counter_task");
        return;
    }

    if (logger_task_start(s_counter_queue) != pdPASS) {
        ESP_LOGE(TAG, "failed to start logger_task");
        return;
    }

    if (led_task_start(BLINK_GPIO) != pdPASS) {
        ESP_LOGE(TAG, "failed to start led_task");
        return;
    }

    // Once we return only three worker tasks keep running.
    ESP_LOGI(TAG, "initialisation done");
}


