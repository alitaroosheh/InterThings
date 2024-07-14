/**
 * @file WiFiHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief WiFi Helper header file
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */

#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

esp_err_t                                wifiInit                                   (void);
esp_err_t                                wifiStartAP                                (wifi_config_t config);
esp_err_t                                wifiStartSTA                               (wifi_config_t config, uint32_t timeout);
esp_err_t                                wifiStartAPSTA                             (wifi_config_t configAP, wifi_config_t configSTA, uint32_t timeout);
esp_err_t                                WiFiGetMode                                (wifi_mode_t *mode);
esp_err_t                                wifiStop                                   ();

