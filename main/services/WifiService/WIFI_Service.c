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
#include "Sensor_Service.h"
#define MAX_RESPONSE_LENGTH 64

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
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void connect_to_wifi()
{
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

static esp_err_t get_handler(httpd_req_t *req)
{
    char savedId = getModuleId();
    char query[100];
    char id[10] = {0};

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        printf("Query string: %s", query);

        if (httpd_query_key_value(query, "id", id, sizeof(id)) == ESP_OK) {
            if(savedId == id)
          	{
              httpd_resp_send(req, "-1", HTTPD_RESP_USE_STRLEN);
              return ESP_OK;
          	}

            char response[MAX_RESPONSE_LENGTH];
            int moistureValue = get_moisture_value();
            snprintf(response, MAX_RESPONSE_LENGTH, "%d", moistureValue);
            httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    		return ESP_OK;
        } else {
            printf("ID not found in query");
            httpd_resp_send(req, "-1", HTTPD_RESP_USE_STRLEN);
            return ESP_OK;
        }
    } else {
        printf("Failed to get query string");
    }

    return ESP_OK;
}

void server_initiation()
{
	httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    server_config.max_uri_handlers = 10;
    httpd_handle_t server_handle = NULL;
    httpd_start(&server_handle, &server_config);
    httpd_uri_t uri_post = {
        .uri = "/humidity",
        .method = HTTP_GET,
        .handler = get_handler,
        .user_ctx = NULL};
    esp_err_t err = httpd_register_uri_handler(server_handle, &uri_post);
	if (err != ESP_OK) {
    	ESP_LOGE("HTTP_SERVER", "Failed to register URI handler. Error: %d", err);
	} else {
    	ESP_LOGI("HTTP_SERVER", "Handler registered for URI: %s", uri_post.uri);
	}
}