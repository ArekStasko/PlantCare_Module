#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"

char* getWifiName(void)
{
  	static char wifiName[128];
    size_t required_size = 0;

    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);

    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to open NVS handle! Error: %d", ret);
        return NULL;
    }

    ret = nvs_get_str(nvs_handle, "name", NULL, &required_size);
    if (ret != ESP_OK || required_size == 0) {
        ESP_LOGE("NVS", "Failed to read wifi name or wifi name not found");
        nvs_close(nvs_handle);
        return NULL;
    }

    ret = nvs_get_str(nvs_handle, "name", wifiName, &required_size);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to read wifi name");
        nvs_close(nvs_handle);
        return NULL;
    }

    nvs_close(nvs_handle);
    return wifiName;
}

char* getWifiPassword(void)
{
    static char password[128];
    size_t required_size = 0;

    nvs_handle_t nvs_handle;
    esp_err_t ret = nvs_open("storage", NVS_READONLY, &nvs_handle);

    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to open NVS handle! Error: %d", ret);
        return NULL;
    }

    ret = nvs_get_str(nvs_handle, "password", NULL, &required_size);
    if (ret != ESP_OK || required_size == 0) {
        ESP_LOGE("NVS", "Failed to read password or password not found");
        nvs_close(nvs_handle);
        return NULL;
    }

    ret = nvs_get_str(nvs_handle, "password", password, &required_size);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Failed to read password");
        nvs_close(nvs_handle);
        return NULL;
    }

    nvs_close(nvs_handle);
    return password;
}

void resetWifiData(void)
{
  	nvs_handle_t nvs_handle;
    size_t required_size;
	esp_err_t ret = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if(ret != ESP_OK)
    {
		return;
    }

    if(nvs_get_str(nvs_handle, "name", NULL, &required_size) == ESP_OK)
    {
		nvs_erase_key(nvs_handle, "name");
    }

    if(nvs_get_str(nvs_handle, "password", NULL, &required_size) == ESP_OK)
    {
      	nvs_erase_key(nvs_handle, "password");
    }

    if(nvs_get_str(nvs_handle, "id", NULL, &required_size) == ESP_OK)
    {
      nvs_erase_key(nvs_handle, "id");
    }

    nvs_close(nvs_handle);
}

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

        if (nvs_get_str(nvs_handle, "id", NULL, &required_size) == ESP_OK)
        {
            nvs_get_str(nvs_handle, "id", uuid, &required_size);
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