idf_component_register(SRCS "PlantCare_Module.c" "./services/bluetoothService/Bluetooth_Service.c"
                    INCLUDE_DIRS "." "./services/bluetoothService"
                    REQUIRES bt nvs_flash driver)
