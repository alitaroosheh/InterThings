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
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

extern QueueHandle_t IoEventQueue;

esp_err_t       IoConfig                (uint64_t pin, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown);
esp_err_t       IoConfigIsr             (uint64_t pin, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t type);
esp_err_t       IoSet                   (gpio_num_t pin);
esp_err_t       IoReset                 (gpio_num_t pin);
esp_err_t       IoToggle                (gpio_num_t pin);

