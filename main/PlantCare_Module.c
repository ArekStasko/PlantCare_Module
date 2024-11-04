#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "driver/i2c.h"
#include "esp_log.h"

static const char *TAG = "i2c-example";

#define I2C_SLAVE_ADDR	0x11
#define TIMEOUT_MS		1000
#define DELAY_MS		1000

void lcd_send_command(uint8_t command) {
    // Wysyłanie komendy do LCD
    i2c_master_write_to_device(I2C_NUM_0, I2C_SLAVE_ADDR, &command, 1, TIMEOUT_MS/portTICK_PERIOD_MS);
}

void lcd_send_data(uint8_t data) {
    // Wysyłanie danych do LCD
    i2c_master_write_to_device(I2C_NUM_0, I2C_SLAVE_ADDR, &data, 1, TIMEOUT_MS/portTICK_PERIOD_MS);
}

void lcd_init() {
    // Wysyłanie komend inicjalizacyjnych do LCD
    lcd_send_command(0x38); // Funkcja 8-bitowa, 2 wiersze
    lcd_send_command(0x0C); // Włącz wyświetlacz, wyłącz kursor
    lcd_send_command(0x06); // Automatyczne przesuwanie w prawo
    lcd_send_command(0x01); // Wyczyść ekran
    vTaskDelay(2 / portTICK_PERIOD_MS); // Opóźnienie dla polecenia wyczyszczenia
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void app_main(void)
{
    uint8_t rx_data[5];

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_NUM_0, &conf);

    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    lcd_init();  // Inicjalizacja LCD
    lcd_print("Hello, world!"); // Wyświetlenie napisu

    while (1) {
        // Odczyt danych, jeśli potrzebne
        vTaskDelay(DELAY_MS/portTICK_PERIOD_MS);
    }
}
