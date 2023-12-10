#include <Arduino.h>

class MoistureSensorService 
{

    private:
        int dry = 4095;
        int wet = 2755;
        int sensorPin = 32;

    public:
        float GetCurrentMoisture()
        {
            pinMode(2, OUTPUT);
            float moisture_percentage;
            int moisture_analog;
            digitalWrite(2, LOW);
            delay(500);
            moisture_analog = analogRead(sensorPin);
            digitalWrite(2, HIGH);
            moisture_percentage = map(moisture_analog, wet, dry, 100, 0);
            Serial.println(moisture_analog);
            return moisture_percentage;
        }
};