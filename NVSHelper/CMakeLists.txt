include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(srcs "src/NVSHelper.c")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       REQUIRES "driver" "log" "freertos" "esp_event" "nvs_flash" "esp_system" "nvs_sec_provider" "bootloader" "bootloader_support" "efuse")
