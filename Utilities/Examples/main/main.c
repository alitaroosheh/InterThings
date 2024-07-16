#include <stdio.h>
#include "Utilities.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "Utilities_Example"


void app_main()
{

    ESP_LOGI(TAG, "Utilities_Example application is running...\n");

	while(1)
	{
        vTaskDelay(3000 / portTICK_PERIOD_MS);
	}

}