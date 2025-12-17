#include "led_task.h"

#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "LED";

#define LED_BLINK_PERIOD_MS 1000

static void led_task(void *pvParameters)
{
    // Task param comes as void* and move through intptr_t to get back the GPIO number safely.
    int gpio_num = (int) (intptr_t) pvParameters;

    bool on = false;
    const TickType_t delay_ticks = pdMS_TO_TICKS(LED_BLINK_PERIOD_MS);

    ESP_LOGI(TAG, "led_task started on GPIO %d", gpio_num);

    while (true) {
        on = !on;
        gpio_set_level(gpio_num, on);
        vTaskDelay(delay_ticks);
    }
}

BaseType_t led_task_start(int gpio_num)
{
    return xTaskCreate(
        led_task,
        "led_task",
        2048,
        (void *)(intptr_t)gpio_num, // cast to void* and then to intptr_t to get back the GPIO number safely
        3,                          // lower priority as it is just a visual effect
        NULL);
}


