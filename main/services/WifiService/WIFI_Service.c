#include <stdio.h>
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "NVS_Service.h"

void connectToWifi()
{
  	char* wifiName = getWifiName();
    char* wifiPassword = getWifiPassword();
    printf("Wifi name: %s\n", wifiName);
    printf("Wifi password: %s\n", wifiPassword);
    printf("Connecting to WiFi network...");
}