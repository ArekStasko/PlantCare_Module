#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <chrono>
#include <headers/MoistureSensorService.h>

class ModuleService
{
    #define API_BASE_URL "http://192.168.1.40:8080/api/v1"

    private:
        Preferences preferences;
        MoistureSensorService moistureSensorService;
        HTTPClient client;
        String moduleId = "MODULE_NOT_REGISTERED";
        bool processed = false;

    public:
        void Initialize()
        {
            Serial.println("START INITIALIZING MODULE SERVICE");
            preferences.begin("module_data", false);
            moduleId = preferences.getString("module_id", "MODULE_NOT_REGISTERED");
            Serial.printf("CURRENT MODULE ID: ");
            Serial.printf(moduleId.c_str());
            Serial.println("END OF INITIALIZING MODULE SERVICE");
        }

        void RegisterModule()
        {
            if(moduleId == "MODULE_NOT_REGISTERED")
            {
                Serial.println("START REGISTERING MODULE");
                client.begin(String(API_BASE_URL) + "/modules/Add");
                int httpCode = client.POST("");
                if(httpCode > 0) 
                {
                    String payload = client.getString();
                    Serial.println("\nStatuscode: " + String(httpCode));
                    Serial.println(payload);
                    std::string payloadStd = payload.c_str();
                    preferences.putString("module_id", payload);
                    moduleId = preferences.getString("module_id", "MODULE_NOT_REGISTERED");
                }

                client.end();
                Serial.println("MODULE REGISTRATION COMPLETED");
            }
        }

        void UpdateMoistureLevel()
        {
             float currentMoistureLevel = moistureSensorService.GetCurrentMoisture();
            Serial.println("CURRENT MOISTURE :");
            Serial.println(currentMoistureLevel);
            client.begin(String(API_BASE_URL) + "/humidity-measurements/Add");
            client.addHeader("Content-Type", "application/json");

            String moduleIdToSent = moduleId;
            Serial.println(moduleIdToSent);

            const size_t CAPACITY = JSON_OBJECT_SIZE(2);
            StaticJsonDocument<CAPACITY> doc;
            Serial.println(moduleId.c_str());
            JsonObject object = doc.to<JsonObject>();
            moduleIdToSent.remove(0, 1);
            moduleIdToSent.remove(moduleIdToSent.length() - 1);
            object["ModuleId"] = moduleIdToSent.c_str();
            object["Humidity"] = currentMoistureLevel;

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
};