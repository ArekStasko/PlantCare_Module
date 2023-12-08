#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define LCD_CS 5
#define LCD_DC 16
#define LCD_RST 4
#define LCD_DIN 23
#define LCD_CLK 18
#define LCD_LED 2

class LCDService
{
    private:
        bool isProcessed = false;
        Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RST, LCD_DIN, LCD_CLK);

    public:
        void Setup()
        {
            lcd.initR(INITR_BLACKTAB);
            lcd.setRotation(0);
            pinMode(LCD_LED, OUTPUT);
            digitalWrite(LCD_LED, HIGH);
        }

        void Run()
        {
                if(!isProcessed)
                {
                    Serial.println("Processing Lcd Screen");
                    lcd.fillScreen(ST7735_BLUE);
                    lcd.setCursor(10, 10);
                    lcd.setTextSize(2);
                    lcd.setTextColor(ST7735_WHITE);
                    lcd.print("IT FUCKING WORKS !");
                    Serial.println("Lcd Screen Processed");
                }
                isProcessed = true;
        }
};