#include <stdio.h>
#include "IAQHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define I2C_MASTER_NUM              0

#define TAG "IAQHelper"

void app_main(void)
{
    
    ESP_LOGI(TAG, "Indoor Air Quality Sensor...");
    uint32_t loopInterval = 5000;
    xTaskCreate(&IAQTask, "IAQTask", configMINIMAL_STACK_SIZE * 2, (void*)&loopInterval, 5, NULL);
        while(true)
    {    
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    
}