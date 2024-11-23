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
#include "esp_gatt_defs.h"
#include "esp_spp_api.h"

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

static void gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
        case ESP_BT_GAP_PIN_REQ_EVT: {
            // Parowanie wymaga PIN
            printf("PIN requested\n");
            esp_bt_pin_code_t pin_code = { '1', '2', '3', '4' };
            uint8_t pin_len = 4;
            esp_bt_gap_pin_reply(param->pin_req.bda, true, pin_len, pin_code);
            break;
        }
        case ESP_BT_GAP_CFM_REQ_EVT: {
            // SSP: Weryfikacja PIN-u
            printf("Confirm PIN: %ld\n", param->cfm_req.num_val);
            esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
            break;
        }
        case ESP_BT_GAP_KEY_NOTIF_EVT: {
            // SSP: Powiadomienie o kluczu
            printf("Key notification: %ld\n", param->key_notif.passkey);
            break;
        }
        case ESP_BT_GAP_KEY_REQ_EVT: {
            // SSP: Klucz wymagany
            printf("Passkey required\n");
            break;
        }
        case ESP_BT_GAP_AUTH_CMPL_EVT: {
            // Autoryzacja zakończona
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                printf("Authentication successful\n");
            } else {
                printf("Authentication failed\n");
            }
            break;
        }
        case ESP_BT_GAP_MODE_CHG_EVT: {
            printf("GAP mode changed: discoverable=%d\n", param->mode_chg.mode);
            break;
        }
        default: {
            printf("Unhandled GAP event: %d\n", event);
            break;
        }
    }
}

static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_INIT_EVT: {
            printf("SPP initialized\n");
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            break;
        }
        case ESP_SPP_START_EVT: {
            printf("SPP service started\n");
            break;
        }
        case ESP_SPP_OPEN_EVT: {
            printf("SPP connection opened\n");
            break;
        }
        case ESP_SPP_CLOSE_EVT: {
            printf("SPP connection closed\n");
            break;
        }
        case ESP_SPP_DATA_IND_EVT: {
            printf("SPP data received: %.*s\n", param->data_ind.len, param->data_ind.data);
            break;
        }
        default: {
            printf("Unhandled SPP event: %d\n", event);
            break;
        }
    }
}

#define SPP_SERVER_NAME "SPP_SERVER"
void start_spp_server(void) {
    esp_err_t ret = esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);
    if (ret != ESP_OK) {
        printf("Failed to start SPP server: %s\n", esp_err_to_name(ret));
    } else {
        printf("SPP server started\n");
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

    esp_bt_gap_register_callback(gap_callback);
    esp_spp_register_callback(spp_callback);
    esp_spp_init(ESP_SPP_MODE_CB);

    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
	start_spp_server();
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_gap_set_pin(pin_type, 0, NULL);
}