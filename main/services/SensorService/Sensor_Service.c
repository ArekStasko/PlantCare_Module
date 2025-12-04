#include <stdio.h>
#include "driver/adc.h"
#include "esp_log.h"
#define MOISTURE_SENSOR_ADC_CHANNEL ADC1_CHANNEL_4
// I WILL NEED TO RECONFIGURE THIS VALUE
const int dry_value = 0;
const int wet_value = 0;
const int difference = 0;


int get_moisture_value(void)
{
  // TO REBUILD
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MOISTURE_SENSOR_ADC_CHANNEL, ADC_ATTEN_DB_11);
    int value = adc1_get_raw(MOISTURE_SENSOR_ADC_CHANNEL);
    printf("Moisture value: {%d}\n", value);
    int result = ((dry_value - value)*100)/(dry_value - wet_value);
    if(result<0) return 0;
    if(result>100) return 100;
    return result;

}