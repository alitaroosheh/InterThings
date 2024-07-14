/**
 * @file NVSHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-07
 * 
 * 
 */

#pragma once

#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

/**
 * @brief nvsSave: to save a key value data in NVS.
 * 
 * @param namespace namespace_name in NVS
 * @param key the key array
 * @param value the input value array
 * @param length the length of value
 * @return esp_err_t equals ESP_OK when successful
 */
esp_err_t                                nvsSave                                        (const char *namespace, const char *key, const char *value, size_t length);

/**
 * @brief nvsLoad: to load a key value from VNS.
 * 
 * @param namespace namespace_name in NVS
 * @param key the key array
 * @param value the output value array
 * @param length the length of value
 * @return esp_err_t equals ESP_OK when successful
 */
esp_err_t                                nvsLoad                                        (const char *namespace, const char *key, char *value, size_t *length);

/**
 * @brief nvsDeleteKey: delete a key value in a namespace from NVS
 * 
 * @param namespace namespace_name in NVS
 * @param key the key array
 * @return esp_err_t equals ESP_OK when successful
 */
esp_err_t                                nvsDeleteKey                                  (const char *namespace, const char *key);

/**
 * @brief nvsDelete: delete a namespace from NVS
 * 
 * @param namespace namespace_name in NVS
 * @return esp_err_t equals ESP_OK when successful
 */
esp_err_t                                nvsDelete                                      (const char *namespace);

