#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_err.h"
#include "Adc_Service.h"

#define MOISTURE_SENSOR_CHANNEL ADC_CHANNEL_4

static adc_oneshot_unit_handle_t adc_handle;

const int dry_value = 2567;
const int wet_value = 1344;

void sensor_init(void)
{
    adc_handle = ADC_GetHandle();

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_4, &config);
}

int get_moisture_value(void)
{
    int value = 0;
    adc_oneshot_read(adc_handle, MOISTURE_SENSOR_CHANNEL, &value);

    printf("Moisture value: %d\n", value);

    int result = ((dry_value - value) * 100) / (dry_value - wet_value);

    if (result < 0) return 0;
    if (result > 100) return 100;

    return result;
}