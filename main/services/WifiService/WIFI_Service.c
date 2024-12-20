#include <stdio.h>
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "NVS_Service.h"

void connectToWifi()
{
  	char* wifiName = getWifiName();
    char* wifiPassword = getWifiPassword();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_wifi_init(&cfg);
    if(ret != ESP_OK)
    {
      printf("Failed to initialize WiFi network\n");
    }
	ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if(ret != ESP_OK)
    {
      printf("Failed to set WiFi as STA mode\n");
    }
}