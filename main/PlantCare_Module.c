#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#define PLANT_CARE_DEVICE_NAME "PLANT_CARE_DEVICE"

void app_main(void)
{
    esp_bluedroid_enable();
    esp_bluedroid_init();
    esp_bt_dev_set_device_name(PLANT_CARE_DEVICE_NAME);
    while (1)
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      int status = esp_bluedroid_get_status();
      printf("status=%d\n", status);
    }
}