#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "NVS_Service.h"
#include "nvs_flash.h"
#include "Sensor_Service.h"
#include "esp_netif_ip_addr.h"
#include "esp_http_client.h"

static bool wifi_started = false;

void send_moisture_to_server(void)
{
    char *savedId = getModuleId();
    int moistureValue = get_moisture_value();
    char *serverAddress = getServerAddress();

    if (!savedId || !serverAddress) return;

    const char *endpoint = "/api/humidity-measurements";

    char full_url[128];
    const int serverPort = 8080;
    snprintf(full_url, sizeof(full_url), "http://%s:%d%s", serverAddress, serverPort, endpoint);


    char post_data[256];
    snprintf(post_data, sizeof(post_data),
             "{\"ModuleId\":\"%s\",\"Humidity\":%d}",
             savedId, moistureValue);

    esp_http_client_config_t config = {
        .url = full_url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_header(client, "Content-Type", "application/json");

    // TO REFACTOR, TOKEN SHOULD BE STORED IN NVS
    const char *auth_token = "deb1197807e28b36bc6a7e5b9d6ad13c9fdc92e407364a5615d31518705057a5";

    char auth_header[128];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", auth_token);
    esp_http_client_set_header(client, "Authorization", auth_header);

    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}


void moisture_task(void *pvParameter)
{
    while (1) {
        send_moisture_to_server();
        vTaskDelay(pdMS_TO_TICKS(1800000));
    }
}

void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
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
        	static bool task_created = false;
            if (!task_created) {
                xTaskCreate(moisture_task, "MoistureTask", 4096, NULL, 5, NULL);
                task_created = true;
            }
        	break;
    	}
    default:
        break;
    }
}

void connect_to_wifi()
{
    if (wifi_started) return;
    wifi_started = true;

    printf("WiFi connecting to WiFi network ...\n");
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
