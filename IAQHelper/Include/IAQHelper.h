/**
 * @file IAQHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief Indoor Air Quality Sensor (BME680)
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */

#include <stdio.h>
#include <bme68x.h>
#include <string.h>
#include "common.h"
#include "I2CHelper.h"
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

float                       Altitude                                (const int32_t press, const float seaLevel);
void                        CalculateIAQ                            (float score);
void                        GetGasReference                         (float gas);
void                        CalculateIndoorAirQuality               (float currentHumidityidity);
void                        IAQTask                                 (void *parameters);




