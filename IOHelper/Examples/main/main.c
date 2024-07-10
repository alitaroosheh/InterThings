#include <stdio.h>
#include "IOHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "IOHelper_example"

#define IOPIN1 4
#define IOPIN2 0

static void ioTask(void* arg)
{
    uint32_t pin;
    for (;;) {
        if (xQueueReceive(IoEventQueue, &pin, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", pin, IoReadPin(pin));
        }
    }
}

void app_main()
{

    ESP_LOGI(TAG, "IO helper application is running...\n");
	IoConfig(IOPIN1, GPIO_MODE_OUTPUT, 0, 0);
	IoConfigIsr (IOPIN2, 1, 0, GPIO_INTR_ANYEDGE);
	
    xTaskCreate(ioTask, "ioTask", 2048, NULL, 10, NULL);

	while(1)
	{
		IoToggle(IOPIN1);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

}