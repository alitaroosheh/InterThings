/**
 * @file Utilities.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-16
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
#include "cJSON.h"


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



#define jsonGetString(root,key,output) { \
    if(!cJSON_GetObjectItem(root, key)) \
    { \
        ESP_LOGE(TAG, "jsonGetString %s not exist.", key); \
        return ESP_ERR_NOT_FOUND; \
    } \
    else \
    { \
        strcpy((char *)output, cJSON_GetObjectItem(root, key)->valuestring); \
        ESP_LOGI(TAG, "key: %s", output); \
    } \
}


#define jsonGetInt(root,key,output) { \
    if(!cJSON_GetObjectItem(root, key)) \
    { \
        ESP_LOGE(TAG, "jsonGetInt %s not exist.", key); \
        return ESP_ERR_NOT_FOUND; \
    } \
    else \
    { \
        *output = cJSON_GetObjectItem(root, key)->valueint; \
        ESP_LOGI(TAG, "key: %"PRId32 "", (uint32_t)output); \
    } \
}


#define jsonGetDouble(root,key,output) { \
    if(!cJSON_GetObjectItem(root, key)) \
    { \
        ESP_LOGE(TAG, "jsonGetDouble %s not exist.", key); \
        return ESP_ERR_NOT_FOUND; \
    } \
    else \
    { \
        *output = cJSON_GetObjectItem(root, key)->valuedouble; \
        ESP_LOGI(TAG, "key: %f", (double)output); \
    } \
}



