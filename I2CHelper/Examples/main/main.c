#include <stdio.h>
#include "I2CHelper.h"

#define I2C_MASTER_NUM              0

void app_main(void)
{

    i2cConfig(I2C_MASTER_NUM, 100000, 13, 14);
    uint8_t data[5] = {0};
    esp_err_t result = i2cGet(0x77, 0xd0, data, 1);
    printf("result: %d - data: %d\r\n", result, data[0]);
    
}