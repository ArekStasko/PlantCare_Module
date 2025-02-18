#include <stdio.h>
#include "driver/adc.h"
#include "esp_log.h"
#define MOISTURE_SENSOR_ADC_CHANNEL ADC1_CHANNEL_4
const int dry_value = 2600;
const int wet_value = 1700;
const int difference = 800;


int get_moisture_value(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MOISTURE_SENSOR_ADC_CHANNEL, ADC_ATTEN_DB_11);
    int value = adc1_get_raw(MOISTURE_SENSOR_ADC_CHANNEL);
    printf("Moisture value: {%d}\n", value);
    return ((dry_value - value)*100)/(dry_value - wet_value);
}