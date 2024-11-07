#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_gap_bt_api.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define PLANT_CARE_DEVICE_NAME "PLANT_CARE_DEVICE"

void app_main(void)
{
    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        printf("Failed to initialize controller: %s\n", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        printf("Failed to enable controller: %s\n", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        printf("Failed to initialize Bluedroid: %s\n", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        printf("Failed to enable Bluedroid: %s\n", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_dev_set_device_name(PLANT_CARE_DEVICE_NAME);
    if (ret != ESP_OK) {
        printf("Failed to set device name: %s\n", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    if (ret != ESP_OK) {
        printf("Failed to set scan mode: %s\n", esp_err_to_name(ret));
        return;
    }

    while (1)
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      int status = esp_bluedroid_get_status();
      printf("status=%d\n", status);
    }
}