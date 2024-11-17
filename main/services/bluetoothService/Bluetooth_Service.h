
#ifndef BLUETOOTH_SERVICE_H
#define BLUETOOTH_SERVICE_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_gap_bt_api.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

void enable_bt(void);

#endif //BLUETOOTH_SERVICE_H
