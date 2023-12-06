#include <HTTPClient.h>

class ModuleService
{
    #define API_BASE_URL "http://192.168.1.40:8080/api"

    private:
        bool isRegistered = false;
    
    public:
        void RegisterService()
        {
            if(!isRegistered)
            {
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
};