#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "driver/gpio.h"

#define PLANT_CARE_DEVICE_NAME "PLANTCARE_MODULE"
#define SPP_SERVER_NAME "SPP_SERVER"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA

#define LED_PIN 2

const char *tag = "Bluetooth";

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

static void init_led(void)
{
    gpio_reset_pin(LED_PIN);
    //gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    printf("Init led completed\n");
}

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
    esp_err_t result = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
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

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
    {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
        {
            printf("authentication success \n");
        }
        else
        {
            printf("authentication failed \n");
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:
    {
        printf("ESP_BT_GAP_PIN_REQ_EVT \n");
        if (param->pin_req.min_16_digit)
        {
            printf("Input pin code: 0000 0000 0000 0000 \n");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        }
        else
        {
            printf("Input pin code: 1234 \n");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        printf("ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d \n", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        printf("ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d \n", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        printf("ESP_BT_GAP_KEY_REQ_EVT Please enter passkey! \n");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        printf("ESP_BT_GAP_MODE_CHG_EVT mode:%d \n", param->mode_chg.mode);
        break;

    default:
    {
        printf("event: %d \n", event);
        break;
    }
    }
    return;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event)
    {
    case ESP_SPP_INIT_EVT:
        printf("ESP_SPP_INIT_EVT \n");
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        printf("ESP_SPP_DISCOVERY_COMP_EVT \n");
        break;
    case ESP_SPP_OPEN_EVT:
        printf("ESP_SPP_OPEN_EVT \n");
        break;
    case ESP_SPP_CLOSE_EVT:
        printf("ESP_SPP_CLOSE_EVT \n");
        break;
    case ESP_SPP_START_EVT:
        printf("ESP_SPP_START_EVT \n");
        esp_bt_dev_set_device_name(PLANT_CARE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    case ESP_SPP_CL_INIT_EVT:
        printf("ESP_SPP_CL_INIT_EVT \n");
        break;
    case ESP_SPP_DATA_IND_EVT:

        printf("ESP_SPP_DATA_IND_EVT \n");
        char received_data[5];
		snprintf(received_data, sizeof(received_data), "%s", param->data_ind.data);
        if (strncmp(received_data, "ON", 2) == 0)
        {
            gpio_set_level(LED_PIN, 1);
            printf("Setting LED to ON \n");
        }
        else if (strncmp(received_data, "OFF", 3) == 0)
        {
            gpio_set_level(LED_PIN, 0);
            printf("Setting LED to OFF \n");
        }
        esp_spp_write(param->data_ind.handle, param->data_ind.len, param->data_ind.data);
        break;
    case ESP_SPP_CONG_EVT:
        printf("ESP_SPP_CONG_EVT \n");
        break;
    case ESP_SPP_WRITE_EVT:
        printf("ESP_SPP_WRITE_EVT \n");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        printf("ESP_SPP_SRV_OPEN_EVT \n");
        break;
    case ESP_SPP_SRV_STOP_EVT:
        printf("ESP_SPP_SRV_STOP_EVT \n");
        break;
    case ESP_SPP_UNINIT_EVT:
        printf("ESP_SPP_UNINIT_EVT \n");
        break;
    default:
        break;
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

static void gap_register_callback(void)
{
  	esp_err_t result = esp_bt_gap_register_callback(esp_bt_gap_cb);
    if (result != ESP_OK) {
        printf("gap register failed\n");
        return;
    }
}

static void spp_register_callback(void)
{
    esp_err_t result = esp_spp_register_callback(esp_spp_cb);
    if (result != ESP_OK)
  	{
        printf("spp register failed\n");
        return;
    }
}

static void spp_init(void)
{
  	esp_err_t result = esp_spp_init(esp_spp_mode);
    if (result != ESP_OK) {
        printf("spp init failed\n");
        return;
    }
}

void enable_bt(void)
{
  	init_led();
    initialize_controller();
    enable_controller();
    initialize_bluedroid();
    enable_bluedroid();
    gap_register_callback();
    spp_register_callback();
    spp_init();
    set_device_name();
    set_scan_mode();
    int status = esp_bluedroid_get_status();
    printf("status=%d\n", status);

    #if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}