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


static esp_err_t nvsPartInit(const char *partition, bool secured)
{

    esp_err_t ret = ESP_FAIL;

	if(secured)
	{
#if SOC_HMAC_SUPPORTED
		nvs_sec_cfg_t cfg = {};
		nvs_sec_scheme_t *sec_scheme_handle = NULL;

		nvs_sec_config_hmac_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT();

		ret = nvs_sec_provider_register_hmac(&sec_scheme_cfg, &sec_scheme_handle);
		if (ret != ESP_OK) {
			return ret;
		}

		ret = nvs_flash_read_security_cfg_v2(sec_scheme_handle, &cfg);
		if (ret != ESP_OK) {
			/* We shall not generate keys here as that must have been done in default NVS partition initialization case */
			ESP_LOGE(TAG, "Failed to read NVS security cfg: [0x%02X] (%s)", ret, esp_err_to_name(ret));
			return ret;
		}

		ret = nvs_flash_secure_init_partition(partition, &cfg);
		if (ret == ESP_OK) {
			ESP_LOGI(TAG, "NVS partition \"%s\" is encrypted.", partition);
		}
		memset(&cfg, 0x00, sizeof(nvs_sec_cfg_t));
#else
		ESP_LOGE(TAG, "HMAC not supported on this device");
		ret = ESP_ERR_NOT_SUPPORTED;
#endif
	}
	else
	{
    	ret = nvs_flash_init_partition(partition);
	}

    return ret;

}

esp_err_t nvsErasePartition(const char *partition)
{
	return nvs_flash_erase_partition(partition);
}

esp_err_t nvsInit(const char *partition, bool secured)
{
	
    /* Initialising the default NVS partition */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Erasing the custom NVS partition */
    //ESP_ERROR_CHECK(nvs_flash_erase_partition(partition));
	
    /* Initialize the custom NVS partition with encryption enabled */
    esp_err_t ret = nvsPartInit(partition, secured);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
        ESP_ERROR_CHECK(nvs_flash_erase_partition(partition));
        ret = nvsPartInit(partition, secured);
    }
    ESP_ERROR_CHECK(ret);
	return ret;
}


esp_err_t nvsSave (const char *partition, const char *namespace, const char *key, char const *value, size_t length)
{
	nvs_handle handler;
	esp_err_t result;


    // Open
    result = nvs_open_from_partition(partition, namespace, NVS_READWRITE, &handler);
	//result = nvs_open(namespace, NVS_READWRITE, &handler);// we need to access further partiotions that's why I am using open from partiotion to give the partition name
	if (result != ESP_OK)
	{
		ESP_LOGE(TAG, "nvsSave couldn't open the namespace %s with error %s.", namespace, esp_err_to_name(result));
		return result;
	}

	result = nvs_set_blob(handler, key, value, length);
	//result = nvs_set_str(handler, key, value);// we need variablke length that's why I am using blob instead
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


esp_err_t nvsLoad (const char *partition, const char *namespace, const char *key, char *value, size_t *length)
{

	nvs_handle handler;
	esp_err_t result;
    result = nvs_open_from_partition(partition, namespace, NVS_READWRITE, &handler);
	//result = nvs_open(namespace, NVS_READWRITE, &handler);// we need to access further partiotions that's why I am using open from partiotion to give the partition name
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


esp_err_t nvsDeleteKey (const char *partition, const char *namespace, const char *key)
{

	nvs_handle handler;
	esp_err_t result;
    result = nvs_open_from_partition(partition, namespace, NVS_READWRITE, &handler);
	//result = nvs_open(namespace, NVS_READWRITE, &handler);// we need to access further partiotions that's why I am using open from partiotion to give the partition name
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


esp_err_t nvsDelete (const char *partition, const char *namespace)
{

	nvs_handle handler;
	esp_err_t result;
    result = nvs_open_from_partition(partition, namespace, NVS_READWRITE, &handler);
	//result = nvs_open(namespace, NVS_READWRITE, &handler);// we need to access further partiotions that's why I am using open from partiotion to give the labpartitionle name
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