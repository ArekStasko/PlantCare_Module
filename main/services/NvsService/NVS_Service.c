#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"

char* getWifiName(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (nvs_get_str(nvs_handle, "name", NULL, &required_size) == ESP_OK)
    {
        return nvs_get_str(nvs_handle, "name", name, &required_size);
    }
    return [];
}

char* getWifiPassword(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (nvs_get_str(nvs_handle, "password", NULL, &required_size) == ESP_OK)
    {
        return nvs_get_str(nvs_handle, "password", password, &required_size);
    }
    return []
}
W
bool checkIfWiFiDataExists(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (ret == ESP_OK)
    {
        char name[64], password[64], uuid[64];
        size_t required_size;
        bool all_keys_present = true;

        if (nvs_get_str(nvs_handle, "name", NULL, &required_size) == ESP_OK)
        {
            nvs_get_str(nvs_handle, "name", name, &required_size);
        }
        else
        {
            all_keys_present = false;
        }

        if (nvs_get_str(nvs_handle, "password", NULL, &required_size) == ESP_OK)
        {
            nvs_get_str(nvs_handle, "password", password, &required_size);
        }
        else
        {
            all_keys_present = false;
        }

        if (nvs_get_str(nvs_handle, "uuid", NULL, &required_size) == ESP_OK)
        {
            nvs_get_str(nvs_handle, "uuid", uuid, &required_size);
        }
        else
        {
            all_keys_present = false;
        }

        nvs_close(nvs_handle);
        return all_keys_present;
    }
    return false;
}