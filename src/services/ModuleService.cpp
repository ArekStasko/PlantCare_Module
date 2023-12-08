#include <HTTPClient.h>
#include <Preferences.h>

class ModuleService
{
    #define API_BASE_URL "http://192.168.1.40:8080/api"

    private:
        Preferences preferences;
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
                HTTPClient client;
                client.begin(String(API_BASE_URL) + "/modules/Add");
                int httpCode = client.POST("");
                if(httpCode > 0) {
                    String payload = client.getString();
                    Serial.println("\nStatuscode: " + String(httpCode));
                    Serial.println(payload);
                    std::string payloadStd = payload.c_str();
                    preferences.putString("module_id", payload);
                    moduleId = preferences.getString("module_id", "MODULE_NOT_REGISTERED");
                }
            Serial.println("MODULE REGISTRATION COMPLETED");
            }
        }
};