#include <stdio.h>
#include "NVSHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

#define TAG "NVSHelper_example"

void app_main()
{

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK( nvs_flash_erase() );
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	char value1[100] = {};
	size_t length1 = sizeof(value1);
	char value2[100] = {};
	size_t length2 = sizeof(value2);


	err = NvsLoad("namespace", "Key1", value1, &length1);
	if(err == ESP_OK)
	{
		ESP_LOGW(TAG, "***namespace Key1 is exist and this is the value:%s and length: %zu", value1, length1);
	}
	else
	{
		ESP_LOGW(TAG, "***namespace error %d", err);
	}
	err = NvsLoad("namespace", "Key2", value2, &length2);
	if(err == ESP_OK)
	{
		ESP_LOGW(TAG, "***namespace Key2 is exist and this is the value:%s and length: %zu", value2, length2);
	}
	else
	{
		ESP_LOGW(TAG, "***namespace error %d", err);
	}



	if(NvsLoad("namespace", "Key1", value1, &length1) == ESP_OK)
	{
		ESP_LOGI(TAG, "namespace Key1 is exist and this is the value:%s and length: %zu", value1, length1);
	}
	else
	{
		strcpy(value1, "this is the key1 value");
		if(NvsSave("namespace", "Key1", value1, sizeof(value1)) == ESP_OK)
		{
			ESP_LOGI(TAG, "namespace Key1 is saved successfuly on NVS");
		}
	}

	if(NvsLoad("namespace", "Key2", value2, &length2) == ESP_OK)
	{
		ESP_LOGI(TAG, "namespace Key2 is exist and this is the value:%s and length: %zu", value2, length2);
		//NvsDeleteKey("namespace", "Key2");
	}
	else
	{
		strcpy(value2, "this is the Key2 value");
		if(NvsSave("namespace", "Key2", value2, sizeof(value2)) == ESP_OK)
		{
			ESP_LOGI(TAG, "Key2 does not exist, therefore namespace Key2 is saved successfuly on NVS");
		}
	}

}