/**
 * @file IOHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-08
 * 
 * 
 */

#pragma once


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"


extern QueueHandle_t ioEventQueue;

#define takeMutex(mtx) { \
			if ( mtx == NULL) \
				mtx = xSemaphoreCreateMutex(); \
			if (!xSemaphoreTake(mtx, pdMS_TO_TICKS(portMAX_DELAY))) \
			{ \
				ESP_LOGE(TAG, "Could not take mutex"); \
				return ESP_ERR_TIMEOUT; \
			} \
		}

#define giveMutex(mtx) { \
			if (!xSemaphoreGive(mtx)) \
			{ \
				ESP_LOGE(TAG, "Could not give mutex"); \
				return ESP_FAIL; \
			} \
        }

/**
 * @brief ioHoldCallBack will call when the press and hold event occured
 * 
 */
typedef void (*ioHoldCallBack)(void);

typedef struct 
{
	uint64_t pin;
	uint32_t interval; //ms
	uint32_t begin;
	bool level;
	bool pressed;
    ioHoldCallBack callback;
}ioHoldConfig;

/**
 * @brief io_Config: to configure IO as a normal mode
 * 
 * @param pin 
 * @param mode 
 * @param pullup 
 * @param pulldown 
 * @return esp_err_t 
 */
esp_err_t       ioConfig                (uint64_t pin, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown);

/**
 * @brief io_ConfigIsr: to configure an IO as an input with ISR
 * 
 * @param pin 
 * @param pullup 
 * @param pulldown 
 * @param type 
 * @return esp_err_t 
 */
esp_err_t       ioConfigIsr             (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type);

/**
 * @brief io_Set: to write 1 to an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       ioSet                   (gpio_num_t pin);

/**
 * @brief io_Reset: to write 0 to an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       ioReset                 (gpio_num_t pin);

/**
 * @brief io_Toggle: to toggle an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       ioToggle                (gpio_num_t pin);

/**
 * @brief io_ReadPin: to read a single IO input
 * 
 * @param pin 
 * @return true 
 * @return false 
 */
bool            ioReadPin               (gpio_num_t pin);


/**
 * @brief ioConfigHold: to config input value to get press and hold value
 * 
 * @param pin 
 * @param interval 
 * @param callback 
 * @return esp_err_t 
 */
esp_err_t ioConfigHold(gpio_num_t pin, bool level, uint32_t interval /*ms */, ioHoldCallBack callback);







#ifdef __cplusplus
extern "C" {
#endif
    esp_err_t ioHoldAppend(ioHoldConfig hold);
    esp_err_t ioHoldGetAt(uint32_t index, ioHoldConfig *hold);
    esp_err_t ioHoldFindByPin(uint64_t pin, ioHoldConfig *hold);
    esp_err_t ioHoldUpdate(ioHoldConfig hold);
	uint32_t ioHoldCount();
#ifdef __cplusplus
}
#endif