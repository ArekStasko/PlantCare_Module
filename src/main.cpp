#include <WiFi.h>
#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("starting");
}

bool isConnected = false;

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
}