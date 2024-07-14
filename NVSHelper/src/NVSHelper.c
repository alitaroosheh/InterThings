/**
 * @file NVSHelper.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-07
 * 
 * 
 */

#include "NVSHelper.h"

#define TAG "NVSHelper"



esp_err_t nvsSave (const char *namespace, const char *key, char const *value, size_t length)
{
	nvs_handle handler;
	esp_err_t result;
	result = nvs_open(namespace, NVS_READWRITE, &handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsSave couldn't open the namespace %s with error %s.", namespace, esp_err_to_name(result));
		return result;
	}

	result = nvs_set_blob(handler, key, value, length);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsSave error on blob %s.", esp_err_to_name(result));
		return result;
	}

	result = nvs_commit(handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsSave error on commit %s.", esp_err_to_name(result));
		return result;
	}

	nvs_close(handler);
	ESP_LOGI(TAG, "nvsSave done!");
	return ESP_OK;

}


esp_err_t nvsLoad (const char *namespace, const char *key, char *value, size_t *length)
{

	nvs_handle handler;
	esp_err_t result;
	result = nvs_open(namespace, NVS_READWRITE, &handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsLoad couldn't open the namespace %s with error %s.", namespace, esp_err_to_name(result));
		return result;
	}
	result = nvs_get_blob(handler, key, value, length);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsLoad got error %s on blob.", esp_err_to_name(result));
		return result;
	}

	nvs_close(handler);
	ESP_LOGI(TAG, "nvsLoad done!");
	return ESP_OK;

}


esp_err_t nvsDeleteKey (const char *namespace, const char *key)
{

	nvs_handle handler;
	esp_err_t result;
	result = nvs_open(namespace, NVS_READWRITE, &handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDeleteKey couldn't open the namespace %s with error %s.", namespace, esp_err_to_name(result));
		return result;
	}

	result = nvs_erase_key(handler, key);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDeleteKey got error %s on nvs_erase_all.", esp_err_to_name(result));
		return result;
	}

	result = nvs_commit(handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDeleteKey got error %s on nvs_commit.", esp_err_to_name(result));
		return result;
	}

	nvs_close(handler);

	ESP_LOGI(TAG, "nvsDeleteKey key %s from namespace %s deleted!", key, namespace);
	return ESP_OK;
}


esp_err_t nvsDelete (const char *namespace)
{

	nvs_handle handler;
	esp_err_t result;
	result = nvs_open(namespace, NVS_READWRITE, &handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDelete couldn't open the namespace %s with error %s.", namespace, esp_err_to_name(result));
		return result;
	}

	result = nvs_erase_all(handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDelete got error %s on nvs_erase_all.", esp_err_to_name(result));
		return result;
	}

	result = nvs_commit(handler);
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsDelete got error %s on nvs_commit.", esp_err_to_name(result));
		return result;
	}

	nvs_close(handler);

	ESP_LOGI(TAG, "nvsDelete namespace %s deleted!", namespace);
	return ESP_OK;
}