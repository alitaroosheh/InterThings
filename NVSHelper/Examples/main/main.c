/**
 * @file main.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-16
 * 
 * 
 */

#include <stdio.h>
#include "NVSHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "NVSHelper_example"

#define lable "config_nvs"

void app_main()
{

	nvsInit(lable, false);
	

	char value[25] = {0};
	size_t length = sizeof(value);



	esp_err_t err = nvsLoad(lable, "namespace", "Key", value, &length);
	if(err == ESP_OK)
	{
		ESP_LOGI(TAG, "***namespace Key is exist and this is the value:%s and length: %zu", value, length);
	}
	else
	{
		ESP_LOGW(TAG, "***namespace error %d", err);
		strcpy(value, "0");
	}
	int incr = atoi(value) + 1;
	sprintf(value, "%d", incr);
	ESP_LOGW(TAG, "writing value %d", incr);
	err = nvsSave(lable, "namespace", "Key", value, length);
}