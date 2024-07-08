#include <stdio.h>
#include "IOHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "IOHelper_example"

#define IOPIN1 4
#define IOPIN2 5

static void ioTask(void* arg)
{
    uint32_t pin;
    for (;;) {
        if (xQueueReceive(IoEventQueue, &pin, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", pin, gpio_get_level(pin));
        }
    }
}

void app_main()
{

	IoConfig(IOPIN1, GPIO_MODE_OUTPUT, 0, 0);
	IoConfig(IOPIN2, GPIO_MODE_OUTPUT, 0, 0);

	IoReset(IOPIN1);
	IoReset(IOPIN2);
	vTaskDelay(10000 / portTICK_PERIOD_MS);

	while(1)
	{
		IoToggle(IOPIN1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoToggle(IOPIN1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoToggle(IOPIN1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoReset(IOPIN1);
		vTaskDelay(5000 / portTICK_PERIOD_MS);

		IoToggle(IOPIN2);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoToggle(IOPIN2);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoToggle(IOPIN2);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		IoToggle(IOPIN2);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

}