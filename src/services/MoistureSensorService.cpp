#include <Arduino.h>

class MoistureSensorService 
{

    private:
        int dry = 595;
        int wet = 239;
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
            int moisture_value = moisture_analog * (5.0 / 1023.0);
            moisture_percentage = map(moisture_value, wet, dry, 100, 0);
            Serial.println(moisture_analog);
            return moisture_percentage;
        }
};