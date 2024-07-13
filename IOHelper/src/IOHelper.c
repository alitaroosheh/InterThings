/**
 * @file IOHelper.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-08
 * 
 * 
 */

#include "IOHelper.h"

#define TAG "IOHelper"
#define ESP_INTR_FLAG_DEFAULT 0

SemaphoreHandle_t io_mutex;
QueueHandle_t ioEventQueue = NULL;
TaskHandle_t holdTaskHandle = NULL;


static void IRAM_ATTR io_isr_Handler(void* arg)
{
    uint32_t pin = (uint32_t) arg;
    xQueueSendFromISR(ioEventQueue, &pin, NULL);
}


static esp_err_t ioHoldConfigIsr (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type)
{

	esp_err_t result;
	takeMutex(io_mutex);
    //zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//interrupt of rising edge
    io_conf.intr_type = type;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 1ULL<<pin;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(result = gpio_config(&io_conf));
	

    gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
	if(ioEventQueue == NULL)
	{
    	ioEventQueue = xQueueCreate(10, sizeof(uint32_t));
	}

    //install gpio isr service
    ESP_ERROR_CHECK(result = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    //hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(result = gpio_isr_handler_add(pin, io_isr_Handler, (void*)(uintptr_t)pin));

    //remove isr handler for gpio number.
    ESP_ERROR_CHECK(result = gpio_isr_handler_remove(pin));
    //hook isr handler for specific gpio pin again
    ESP_ERROR_CHECK(result = gpio_isr_handler_add(pin, io_isr_Handler, (void*)(uintptr_t)pin));

	giveMutex(io_mutex);
	return result;

}


esp_err_t ioConfigIsr (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type)
{

	esp_err_t result;
	takeMutex(io_mutex);
    //zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//interrupt of rising edge
    io_conf.intr_type = type;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = 1ULL<<pin;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    ESP_ERROR_CHECK(result = gpio_config(&io_conf));
	

    gpio_set_intr_type(pin, type);//GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
	if(ioEventQueue == NULL)
	{
    	ioEventQueue = xQueueCreate(10, sizeof(uint32_t));
	}

    //install gpio isr service
    ESP_ERROR_CHECK(result = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    //hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(result = gpio_isr_handler_add(pin, io_isr_Handler, (void*)(uintptr_t)pin));

    //remove isr handler for gpio number.
    ESP_ERROR_CHECK(result = gpio_isr_handler_remove(pin));
    //hook isr handler for specific gpio pin again
    ESP_ERROR_CHECK(result = gpio_isr_handler_add(pin, io_isr_Handler, (void*)(uintptr_t)pin));

	giveMutex(io_mutex);
	return result;

}


bool ioReadPin (gpio_num_t pin)
{

	uint32_t result;
	takeMutex(io_mutex);
	result = (uint32_t)gpio_get_level(pin);
	giveMutex(io_mutex);
	return result == 0 ? false:true;

}

esp_err_t ioConfig (uint64_t pin, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown)
{

	esp_err_t result;
	takeMutex(io_mutex);
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = mode == GPIO_MODE_OUTPUT ? GPIO_MODE_INPUT_OUTPUT:mode;
    io_conf.pin_bit_mask = (1ULL<<pin);
    io_conf.pull_down_en = pulldown;
    io_conf.pull_up_en = pullup;
    ESP_ERROR_CHECK(result = gpio_config(&io_conf));
	giveMutex(io_mutex);
	return result;

}


esp_err_t ioSet(gpio_num_t pin)
{

	esp_err_t result;
	takeMutex(io_mutex);
	ESP_ERROR_CHECK(result = gpio_set_level(pin, 1));
	giveMutex(io_mutex);
	return result;

}


esp_err_t ioReset(gpio_num_t pin)
{

	esp_err_t result;
	takeMutex(io_mutex);
	ESP_ERROR_CHECK(result = gpio_set_level(pin, 0));
	giveMutex(io_mutex);
	return result;

}


esp_err_t ioToggle(gpio_num_t pin)
{

	esp_err_t result;
	takeMutex(io_mutex);
	ESP_ERROR_CHECK(result = gpio_set_level(pin, (gpio_get_level(pin) == 0) ? 1:0));
	giveMutex(io_mutex);
	return result;

}


static void ioHoldTask(void *pvParameters)
{
	uint32_t pin;
    while (true)
	{
        if (xQueueReceive(ioEventQueue, &pin, portTICK_PERIOD_MS * 50))
		{
			bool pinstate = ioReadPin(pin);
            printf("GPIO[%"PRIu32"] intr, val: %d\n", pin, pinstate);
			ioHoldConfig hold;
			if(ioHoldFindByPin(pin, &hold) == ESP_OK)
			{
				if (hold.level == pinstate && hold.pressed == false)
				{
					TickType_t tickCount = xTaskGetTickCount();
					hold.begin = tickCount * portTICK_PERIOD_MS;
					hold.pressed = true;
					ioHoldUpdate(hold);
            		printf("hold.begin: %"PRIu32" hold.pressed: %d\n", hold.begin, hold.pressed);
				}
				else if (hold.level != pinstate)
				{
					hold.begin = 0;
					hold.pressed = false;
					ioHoldUpdate(hold);
            		printf("hold.begin: %"PRIu32" hold.pressed: %d\n", hold.begin, hold.pressed);
				}
			}
        }//todo
		uint32_t count = ioHoldCount();
		printf("count: %"PRIu32"\r\n", count);
		for(uint32_t index = 0; index < count; index++)
		{
			ioHoldConfig hold;
			if(ioHoldGetAt(index, &hold) == ESP_OK)
			{
				printf("hold.begin: %"PRIu32" xTaskGetTickCount: %"PRIu32" pressed: %d\n", hold.begin, (xTaskGetTickCount() * portTICK_PERIOD_MS), hold.pressed);
				if((hold.pressed == true) && (hold.begin + hold.interval) <= (xTaskGetTickCount() * portTICK_PERIOD_MS))
				{
					
            		printf("time expired hold.begin: %"PRIu32" hold.pressed: %d\n", hold.begin, hold.pressed);
					hold.begin = 0;
					hold.pressed = false;
					ioHoldUpdate(hold);
					hold.callback();
				}
			}
		}
		vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}


esp_err_t ioConfigHold(gpio_num_t pin, bool level, uint32_t interval /*ms */, ioHoldCallBack callback)
{

	esp_err_t result;
	ioHoldConfig config = {
		.pin = pin,
		.interval = interval,
		.level = level,
		.pressed = false,
		.callback = callback
	};

	if ((result = ioHoldAppend(config)) != ESP_OK)
	{
		return result;
	}

	ioHoldConfigIsr(pin, 1, 0, GPIO_INTR_ANYEDGE);

	if (holdTaskHandle == NULL)
	{
        // Task is not running, create it
        xTaskCreate(ioHoldTask, "ioHoldTask", configMINIMAL_STACK_SIZE*5, NULL, tskIDLE_PRIORITY + 1, &holdTaskHandle);
        
        if (holdTaskHandle == NULL)
		{
			return ESP_ERR_NOT_ALLOWED;
        }
    }
	return ESP_OK;

}

