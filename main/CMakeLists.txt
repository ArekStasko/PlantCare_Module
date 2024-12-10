idf_component_register(SRCS "PlantCare_Module.c" "./services/bluetoothService/Bluetooth_Service.c" "./services/NvsService/NVS_Service.c"
                    INCLUDE_DIRS "." "./services/bluetoothService" "./services/NvsService"
                    REQUIRES bt nvs_flash driver)
