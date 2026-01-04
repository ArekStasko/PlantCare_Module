#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "NVS_Service.h"
#include "nvs_flash.h"
#include "Sensor_Service.h"
#include "esp_netif_ip_addr.h"
#define MAX_RESPONSE_LENGTH 64

static void stop_wifi_service(void)
{
  esp_err_t result = esp_wifi_stop();
  if(result != ESP_OK)
  {
    printf("esp_wifi_stop failed (%d)\n", result);
  }
};

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting WIFI_EVENT_STA_START ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected WIFI_EVENT_STA_CONNECTED ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection WIFI_EVENT_STA_DISCONNECTED ... \n");
        esp_wifi_connect();
        break;
    case IP_EVENT_STA_GOT_IP:
        {
      		char* ip_address = getWifiIpAddress();
            if (ip_address == NULL){
              break;
            }
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        	char ip_str[16];
        	esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        	printf("IP ADDRESS: %s\n", ip_str);

            nvs_handle_t nvs_handle;
        	esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
        	if (err != ESP_OK)
        	{
                printf("UNDABLE TO OPEN NVS STORAGE \n");
            	break;
        	}
            nvs_set_str(nvs_handle, "address", ip_str);
        	break;
    	}
    default:
        break;
    }
}

void connect_to_wifi()
{
    printf("WiFi connecting to WiFi ...\n");
  	char* wifiName = getWifiName();
    char* wifiPassword = getWifiPassword();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {0};
    snprintf((char*)wifi_configuration.sta.ssid, sizeof(wifi_configuration.sta.ssid), "%s", wifiName);
    snprintf((char*)wifi_configuration.sta.password, sizeof(wifi_configuration.sta.password), "%s", wifiPassword);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}

        void UpdateMoistureLevel()
        {
            char savedId = getModuleId();
            if(savedId == NULL) return;

            char* serverAddress = getServerAddress();
            int moistureValue = get_moisture_value();
            Serial.println("CURRENT MOISTURE :");
            Serial.println(currentMoistureLevel);
            client.begin(String(serverAddress) + "/humidity-measurements/Add");
            client.addHeader("Content-Type", "application/json");

            String moduleIdToSent = moduleId;
            Serial.println(moduleIdToSent);

            const size_t CAPACITY = JSON_OBJECT_SIZE(2);
            StaticJsonDocument<CAPACITY> doc;
            Serial.println(moduleId.c_str());
            JsonObject object = doc.to<JsonObject>();
            moduleIdToSent.remove(0, 1);
            moduleIdToSent.remove(moduleIdToSent.length() - 1);
            object["ModuleId"] = savedId.c_str();
            object["Humidity"] = moistureValue;

            String jsonOutput;
            serializeJson(doc, jsonOutput);

            Serial.println(jsonOutput);

            int httpCode = client.POST(jsonOutput);

            if(httpCode > 0)
            {
                String payload = client.getString();
                Serial.println("\nStatuscode: " + String(httpCode));
                Serial.println(payload);
            }

            client.end();

            if(httpCode == 200)
            {
                Serial.println("Humidity Measurement successfully registered");
            }
        }
