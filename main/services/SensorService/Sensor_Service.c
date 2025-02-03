#include <stdio.h>
#include "driver/adc.h"
#include "esp_log.h"
#define MOISTURE_SENSOR_ADC_CHANNEL ADC1_CHANNEL_0

int get_moisture_value()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MOISTURE_SENSOR_ADC_CHANNEL, ADC_ATTEN_DB_11);
    return adc1_get_raw(MOISTURE_SENSOR_ADC_CHANNEL);
}