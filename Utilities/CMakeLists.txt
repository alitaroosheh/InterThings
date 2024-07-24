include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(srcs "src/Utilities.c")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       REQUIRES "driver" "log" "freertos" "esp_event" "esp_system" "json")
