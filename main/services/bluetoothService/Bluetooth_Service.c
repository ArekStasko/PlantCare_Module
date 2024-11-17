#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_gap_bt_api.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define PLANT_CARE_DEVICE_NAME "PLANTCARE_MODULE"

static void initialize_controller(void)
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t result = esp_bt_controller_init(&bt_cfg);

    if (result != ESP_OK) {
        printf("Failed to initialize controller: %s\n", esp_err_to_name(result));
        return;
    }
}

static void enable_controller(void)
{
    esp_err_t result = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (result != ESP_OK) {
        printf("Failed to enable controller: %s\n", esp_err_to_name(result));
        return;
    }
}

static void initialize_bluedroid(void)
{
    esp_err_t result = esp_bluedroid_init();
    if (result != ESP_OK) {
        printf("Failed to initialize Bluedroid: %s\n", esp_err_to_name(result));
        return;
    }
}

static void enable_bluedroid(void)
{
    esp_err_t result = esp_bluedroid_enable();
    if (result != ESP_OK) {
        printf("Failed to enable Bluedroid: %s\n", esp_err_to_name(result));
        return;
    }
}

static void set_device_name(void)
{
    esp_err_t result = esp_bt_dev_set_device_name(PLANT_CARE_DEVICE_NAME);
    if (result != ESP_OK) {
        printf("Failed to set device name: %s\n", esp_err_to_name(result));
        return;
    }
}

static void set_scan_mode(void)
{
    esp_err_t result = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    if (result != ESP_OK) {
        printf("Failed to set scan mode: %s\n", esp_err_to_name(result));
        return;
    }
}

void enable_bt(void)
{
    initialize_controller();
    enable_controller();
    initialize_bluedroid();
    enable_bluedroid();
    set_device_name();
    set_scan_mode();
    int status = esp_bluedroid_get_status();
    printf("status=%d\n", status);
}