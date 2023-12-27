#include <Arduino.h>
#include <headers/WiFiService.h>
#include <headers/ModuleService.h>
#include <headers/LCDService.h>

WiFiService wifiService;
ModuleService moduleService;
LCDService lcdService;

void setup() {
  Serial.begin(921600);
  lcdService.Setup();
  moduleService.Initialize();
  wifiService.Connect();
}

bool isRegistered = false;
long lastMillis;

void loop() {
  wifiService.HealthCheck();

  if(wifiService.isConnected){
    moduleService.RegisterModule();
    lcdService.Run();

    if(millis() - lastMillis >= 30*60*1000UL) 
    {
      lastMillis = millis();
      moduleService.UpdateMoistureLevel();
    }
  }

}