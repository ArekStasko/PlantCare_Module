#include <stdio.h>
#include <string.h>
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

static void bt_gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_BT_GAP_PIN_REQ_EVT: {
            printf("PIN code requested, providing default PIN.\n");

            esp_bt_pin_code_t pin_code = { '1', '2', '3', '4' };
            uint8_t pin_len = 4;

            esp_err_t result = esp_bt_gap_pin_reply(param->pin_req.bda, true, pin_len, pin_code);
            if (result != ESP_OK) {
                printf("Failed to reply with PIN: %s\n", esp_err_to_name(result));
            }
            break;
        }
        case ESP_BT_GAP_AUTH_CMPL_EVT:
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                printf("Pairing successful with device: %s\n", param->auth_cmpl.device_name);
            } else {
                printf("Pairing failed, status: %d\n", param->auth_cmpl.stat);
            }
        break;
        default:
            printf("Unhandled GAP event: %d\n", event);
        break;
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

    esp_bt_gap_register_callback(bt_gap_callback);
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
    esp_bt_pin_code_t pin_code;
    strncpy((char *)pin_code, "1234", 4);
    esp_bt_gap_set_pin(pin_type, 4, pin_code);

    esp_bt_gap_register_callback(spp_callback);
}