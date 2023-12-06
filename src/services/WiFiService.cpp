#include <string>
#include <WiFi.h>

class WiFiService
{
    #define WIFI_SSID "Orange_Swiatlowod_D5A0"
    #define WIFI_PASSWORD "FYPWSCW59ZAY"

    public:
        bool isConnected = false;

        void Connect()
        {
            Serial.println("Connecting to WiFi");
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }

        void HealthCheck()
        {
            if(WiFi.status() == WL_CONNECTED && !isConnected){
                Serial.println("Successfully connected to WiFi");
                isConnected = true;
            }

            if(WiFi.status() != WL_CONNECTED){
                Serial.println("Connection with WiFi lost");
                isConnected = false;
            }
        }
};