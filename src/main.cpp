#include <Arduino.h>
#include <headers/WiFiService.h>
#include <headers/ModuleService.h>

WiFiService wifiService;
ModuleService moduleService;

void setup() {
  Serial.begin(921600);
  moduleService.Initialize();
  wifiService.Connect();
}

bool isRegistered = false;
long lastMillis;

void loop() {
  wifiService.HealthCheck();

  if(wifiService.isConnected){
    moduleService.RegisterModule();

    if(millis() - lastMillis >= 1*30*1000UL) 
    {
      lastMillis = millis();
      moduleService.UpdateMoistureLevel();
    }

  }
}