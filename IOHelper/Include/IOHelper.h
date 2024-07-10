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

extern QueueHandle_t IoEventQueue;

/**
 * @brief IoConfig: to configure IO as a normal mode
 * 
 * @param pin 
 * @param mode 
 * @param pullup 
 * @param pulldown 
 * @return esp_err_t 
 */
esp_err_t       IoConfig                (uint64_t pin, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown);

/**
 * @brief IoConfigIsr: to configure an IO as an input with ISR
 * 
 * @param pin 
 * @param pullup 
 * @param pulldown 
 * @param type 
 * @return esp_err_t 
 */
esp_err_t       IoConfigIsr             (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type);

/**
 * @brief IoSet: to write 1 to an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       IoSet                   (gpio_num_t pin);

/**
 * @brief IoReset: to write 0 to an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       IoReset                 (gpio_num_t pin);

/**
 * @brief IoToggle: to toggle an IO
 * 
 * @param pin 
 * @return esp_err_t 
 */
esp_err_t       IoToggle                (gpio_num_t pin);

/**
 * @brief IoReadPin: to read a single IO input
 * 
 * @param pin 
 * @return true 
 * @return false 
 */
bool            IoReadPin               (gpio_num_t pin);

