#include <stdio.h>
#include "esp_bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

char *TAG = "Plantcare Module";
uint8_t ble_addr_type;
static void ble_app_advertise(void);

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    char *data = (char *)ctxt->om->om_data;
    char *name, *password, *uuid;

    char buffer[256];
    strncpy(buffer, data, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    name = strtok(buffer, "|");
    password = strtok(NULL, "|");
    uuid = strtok(NULL, "|");

    if (name && password && uuid)
    {
        nvs_handle_t nvs_handle;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
            return BLE_ATT_ERR_UNLIKELY;
        }

        nvs_set_str(nvs_handle, "name", name);
        nvs_set_str(nvs_handle, "password", password);
        nvs_set_str(nvs_handle, "uuid", uuid);

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) committing data!", esp_err_to_name(err));
            nvs_close(nvs_handle);
            return BLE_ATT_ERR_UNLIKELY;
        }

        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "Data saved: name=%s, password=%s, uuid=%s", name, password, uuid);
    }
    else
    {
        ESP_LOGE(TAG, "Invalid data format");
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    return 0;
}

static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x180),
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xFEF4),
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},
         {.uuid = BLE_UUID16_DECLARE(0xDEAD),
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

static void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

static void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}

static void host_task(void *param)
{
    nimble_port_run();
}

void disable_bt()
{
    ESP_LOGI(TAG, "Disabling Bluetooth...");
    nimble_port_stop();
    nimble_port_deinit();

    esp_err_t err = esp_nimble_hci_deinit();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to deinitialize NimBLE controller: %s", esp_err_to_name(err));
        return;
    }

    err = esp_bt_controller_disable();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to disable Bluetooth controller: %s", esp_err_to_name(err));
        return;
    }

    err = esp_bt_controller_deinit();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to deinitialize Bluetooth controller: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Bluetooth disabled successfully.");
}

void enable_bt()
{
    nimble_port_init();
    ble_svc_gap_device_name_set("BLE-Server");
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
}