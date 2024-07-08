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

SemaphoreHandle_t ioMutex;
QueueHandle_t IoEventQueue = NULL;

#define ioTakeMutext { \
			if ( ioMutex == NULL) \
				ioMutex = xSemaphoreCreateMutex(); \
			if (!xSemaphoreTake(ioMutex, pdMS_TO_TICKS(portMAX_DELAY))) \
			{ \
				ESP_LOGE(TAG, "Could not take ioMutex"); \
				return ESP_ERR_TIMEOUT; \
			} \
		}

#define ioGiveMutext { \
			if (!xSemaphoreGive(ioMutex)) \
			{ \
				ESP_LOGE(TAG, "Could not give ioMutex"); \
				return ESP_FAIL; \
			} \
        }


static void IRAM_ATTR ioIsrHandler(void* arg)
{
    uint32_t pin = (uint32_t) arg;
    xQueueSendFromISR(IoEventQueue, &pin, NULL);
}


esp_err_t IoConfigIsr (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type)
{
	return ESP_OK;
	// esp_err_t result;
	// ioTakeMutext;
    // gpio_config_t io_conf = {};
    // io_conf.intr_type = type;//GPIO_INTR_POSEDGE;
    // io_conf.pin_bit_mask = (1ULL<<pin);
    // io_conf.mode = GPIO_MODE_INPUT;
    // io_conf.pull_down_en = pulldown;
    // io_conf.pull_up_en = pullup;
    // result = gpio_config(&io_conf);
	// if(result != ESP_OK)
	// {
	// 	ioGiveMutext;
	// 	return result;
	// }

    // if(IoEventQueue == NULL)
	// {
	// 	IoEventQueue = xQueueCreate(10, sizeof(uint32_t));
	// }
	// result = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	// if(result != ESP_OK)
	// {
	// 	ioGiveMutext;
	// 	return result;
	// }
    // result = gpio_isr_handler_remove(pin);
	// if(result != ESP_OK)
	// {
	// 	ioGiveMutext;
	// 	return result;
	// }
	// result = gpio_isr_handler_add(pin, ioIsrHandler, (void*) pin);
	// ioGiveMutext;
	// return result;

}

esp_err_t IoConfig (uint64_t pin, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown)
{

	ioTakeMutext;
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = mode == GPIO_MODE_OUTPUT ? GPIO_MODE_INPUT_OUTPUT:mode;
    io_conf.pin_bit_mask = (1ULL<<pin);
    io_conf.pull_down_en = pulldown;
    io_conf.pull_up_en = pullup;
    esp_err_t result = gpio_config(&io_conf);
	ioGiveMutext;
	return result;

}


esp_err_t IoSet(gpio_num_t pin)
{

	ioTakeMutext;
	esp_err_t result = gpio_set_level(pin, 1);
	ioGiveMutext;
	return result;

}


esp_err_t IoReset(gpio_num_t pin)
{

	ioTakeMutext;
	esp_err_t result = gpio_set_level(pin, 0);
	ioGiveMutext;
	return result;

}


esp_err_t IoToggle(gpio_num_t pin)
{

	ESP_LOGI(TAG, "current IO %d value is %d", pin, gpio_get_level(pin));
	ioTakeMutext;
	esp_err_t result = gpio_set_level(pin, (gpio_get_level(pin) == 0) ? 1:0);
	ESP_LOGI(TAG, "Now IO %d is %d", pin, gpio_get_level(pin));
	ioGiveMutext;
	return result;

}



