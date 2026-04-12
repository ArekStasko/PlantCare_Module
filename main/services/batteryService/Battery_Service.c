#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t cali_handle = NULL;

int Get_Battery_Percentage_From_ADC(int adc_mv)
{
  float voltage = adc_mv * 3.39 / 1000.0;

  if (voltage >= 4.2) return 100;
  if (voltage >= 4.0) return 80;
  if (voltage >= 3.85) return 60;
  if (voltage >= 3.7) return 40;
  if (voltage >= 3.5) return 20;
  if (voltage >= 3.3) return 10;
  return 0;
}

void Battery_ADC_Init(void)
{
  adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_1,
};
  adc_oneshot_new_unit(&init_config, &adc_handle);

  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_11,
};

  adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config);

  adc_cali_line_fitting_config_t cali_config = {
    .unit_id = ADC_UNIT_1,
    .atten = ADC_ATTEN_DB_11,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
};

  adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);
}

int Get_Battery_Level(void)
{
  int raw = 0;
  int voltage = 0;

  adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &raw);

  if (cali_handle) {
    adc_cali_raw_to_voltage(cali_handle, raw, &voltage);
    return Get_Battery_Percentage_From_ADC(voltage);
  }

  return 0;
}