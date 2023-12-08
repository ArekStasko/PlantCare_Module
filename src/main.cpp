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
  wifiService.Connect();
}

bool isRegistered = false;

void loop() {
  wifiService.HealthCheck();

  if(wifiService.isConnected){
    moduleService.RegisterService();
    lcdService.Run();
  }

}