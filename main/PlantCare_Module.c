#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define LED_GPIO_PIN 2

void app_main(void)
{
    esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN);
    gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

    while (true) {
        gpio_set_level(LED_GPIO_PIN, 1);
        printf("LED ON\n");

        vTaskDelay(500 / portTICK_PERIOD_MS);

        gpio_set_level(LED_GPIO_PIN, 0);
        printf("LED OFF\n");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
