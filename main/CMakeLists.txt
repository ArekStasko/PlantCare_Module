idf_component_register(SRCS "PlantCare_Module.c" "./services/bluetoothService/Bluetooth_Service.c" "./services/NvsService/NVS_Service.c" "./services/WifiService/WIFI_Service.c" "./services/SensorService/Sensor_Service.c" "./services/batteryService/Battery_Service.c"
        INCLUDE_DIRS "." "./services/bluetoothService" "./services/NvsService" "./services/WifiService" "./services/SensorService" "./services/batteryService"
        REQUIRES bt nvs_flash driver esp_wifi esp_http_client esp_adc)
