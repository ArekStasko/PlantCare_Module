#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "Bluetooth_Service.h"

void app_main(void)
{
    nvs_flash_init();
    enable_bt();
}