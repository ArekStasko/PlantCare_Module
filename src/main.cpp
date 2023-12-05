#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define API_BASE_URL ""

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("starting");
}

bool isConnected = false;
bool isRegistered = false;

void loop() {
  if(WiFi.status() == WL_CONNECTED && !isConnected){
    Serial.println("connected");
    digitalWrite(LED_BUILTIN, HIGH);
    isConnected = true;
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    isConnected = false;
    delay(1000);
  }

  if(!isRegistered){
    Serial.println("START REGISTERING MODULE");
    HTTPClient client;
    client.begin(String(API_BASE_URL) + "/modules/Add");
    int httpCode = client.POST("");
    if(httpCode > 0) {
      String payload = client.getString();
      Serial.println("\nStatuscode: " + String(httpCode));
      Serial.println(payload);
    }
    isRegistered = true;
  }
}