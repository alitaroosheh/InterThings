#include <stdio.h>
#include "IOHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "IOHelper_example"

#define IOPIN1 4
#define IOPIN0 0

void holdCallback(void)
{
    ESP_LOGE(TAG, "holdCallback called from HoldTask\n");
}

static void ioTask(void* arg)
{
    uint32_t pin;
    for (;;) {
        if (xQueueReceive(ioEventQueue, &pin, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", pin, ioReadPin(pin));
        }
    }
}

void app_main()
{

    ESP_LOGI(TAG, "IO helper application is running...\n");
	ioConfig(IOPIN1, GPIO_MODE_OUTPUT, 0, 0);
	//ioConfigIsr (IOPIN0, 1, 0, GPIO_INTR_ANYEDGE);
    ioConfigHold(IOPIN0, false, 5000, &holdCallback);
	
    //xTaskCreate(ioTask, "ioTask", 2048, NULL, 10, NULL);

    // ioHoldConfig ioconf1 = {.pin = 4, .interval = 10000, .level = false, .pressed = false};
    // ioHoldConfig ioconf2 = {.pin = 1, .interval = 20000, .level = true, .pressed = false};
    // ioHoldConfig ioconf3 = {.pin = 2, .interval = 30000, .level = true, .pressed = false};
    // ioHoldConfig ioconf4 = {.pin = 3, .interval = 40000, .level = false, .pressed = false};

    // ioHoldAppend(ioconf1);
    // ioHoldAppend(ioconf2);
    // ioHoldAppend(ioconf3);
    // ioHoldAppend(ioconf4);

	while(1)
	{
		ioToggle(IOPIN1);
		vTaskDelay(5000 / portTICK_PERIOD_MS);

        for(int index=0; index<4; index++)
        {
            ioHoldConfig ioconf;
            if (ioHoldGetAt(index, &ioconf) == ESP_OK)
            {
                // ESP_LOGI(TAG, "ioconf%d pin:%" PRId64 " interval:%" PRIu32 " level:%d pressed:%d",
                //     index,
                //     ioconf.pin,
                //     ioconf.interval,
                //     ioconf.level,
                //     ioconf.pressed
                // );
            }
            else
            {
                ESP_LOGI(TAG, "index %d is out of bound", index);
            }
        }

        ioHoldConfig ioconf;
        esp_err_t err = ioHoldFindByPin(2, &ioconf);
        if (err == ESP_OK)
        {
            // ESP_LOGI(TAG, "ioHoldFindByPin%d pin:%" PRId64 " interval:%" PRIu32 " level:%d pressed:%d",
            //     2,
            //     ioconf.pin,
            //     ioconf.interval,
            //     ioconf.level,
            //     ioconf.pressed
            // );
        }
        else
        {
            ESP_LOGI(TAG, "index2 error %d not found", err);
        }
	}

}