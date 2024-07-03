include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(srcs "src/IAQHelper.c" "src/bme68x.c" "src/common.c")


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       REQUIRES "driver" "log" "freertos")
