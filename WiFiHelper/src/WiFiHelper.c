/**
 * @file WiFiHelper.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief WiFi Helper, a wraper on ESP-IDF WiFi connectivity
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */

#include "WiFiHelper.h"

#define TAG "WiFiHelper"



static EventGroupHandle_t wifiEventGroup;
const int CONNECTED_BIT = BIT0;


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
		esp_wifi_connect();
		xEventGroupClearBits(wifiEventGroup, CONNECTED_BIT);
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
		xEventGroupSetBits(wifiEventGroup, CONNECTED_BIT);
	}
}


esp_err_t WiFiGetMode(wifi_mode_t *mode)
{
	return esp_wifi_get_mode(mode);
}


esp_err_t wifiStop()
{
	return esp_wifi_deinit();
}

esp_err_t wifiInit(void)
{
	static bool initialized = false;
	esp_err_t ret;
	if (initialized) {
		return ESP_OK;
	}
	ESP_ERROR_CHECK(esp_netif_init());
	wifiEventGroup = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
	assert(ap_netif);
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( ret = esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( ret = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &event_handler, NULL) );
	ESP_ERROR_CHECK( ret = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );

	ESP_ERROR_CHECK( ret = esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( ret = esp_wifi_set_mode(WIFI_MODE_NULL) );
	ESP_ERROR_CHECK( ret = esp_wifi_start() );

	initialized = true;
	return ret;
}


esp_err_t wifiStartAP(wifi_config_t config)
{
	esp_err_t ret;
	ESP_ERROR_CHECK( ret = esp_wifi_set_mode(WIFI_MODE_AP) );
	ESP_ERROR_CHECK( ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &config) );
	ESP_ERROR_CHECK( ret = esp_wifi_start() );
	ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s channel:%d",
			 (char *)config.ap.ssid, config.ap.channel);
	return ret;
}


esp_err_t wifiStartSTA(wifi_config_t config, uint32_t timeout_ms)
{

	esp_err_t ret;

	ESP_ERROR_CHECK( ret = esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &config) );
	ESP_ERROR_CHECK( ret = esp_wifi_connect() );

	EventBits_t bits = xEventGroupWaitBits(wifiEventGroup, CONNECTED_BIT,
								   pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAG, "bits=%x", (unsigned int)bits);
	if (bits) {
		ESP_LOGI(TAG, "WIFI_MODE_STA connected. SSID:%s",
			 (char *)config.sta.ssid);
	} else {
		ESP_LOGI(TAG, "WIFI_MODE_STA can't connected. SSID:%s",
			 (char *)config.sta.ssid);
	}
	if(ret == ESP_OK && (bits & CONNECTED_BIT) != 0)
	{
		return ESP_OK;
	}
	return ret;
	//return (bits & CONNECTED_BIT) != 0;
}


esp_err_t wifiStartAPSTA(wifi_config_t configAP, wifi_config_t configSTA, uint32_t timeout)
{

	esp_err_t ret;

	ESP_ERROR_CHECK( ret = esp_wifi_set_mode(WIFI_MODE_APSTA) );
	ESP_ERROR_CHECK( ret = esp_wifi_set_config(ESP_IF_WIFI_AP, &configAP) );
	ESP_ERROR_CHECK( ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &configSTA) );
	ESP_ERROR_CHECK( ret = esp_wifi_start() );
	ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s channel:%d",
			 (char *)configAP.ap.ssid, configAP.ap.channel);

	ESP_ERROR_CHECK( ret = esp_wifi_connect() );
	EventBits_t bits = xEventGroupWaitBits(wifiEventGroup, CONNECTED_BIT,
								   pdFALSE, pdTRUE, timeout / portTICK_PERIOD_MS);
	ESP_LOGI(TAG, "bits=%ld", (uint32_t)bits);
	if (bits) {
		ESP_LOGI(TAG, "WIFI_MODE_STA connected. SSID:%s",
			 (char *)configSTA.sta.ssid);
	} else {
		ESP_LOGI(TAG, "WIFI_MODE_STA can't connected. SSID:%s",
			 (char *)configSTA.sta.ssid);
	}
	if(ret == ESP_OK && (bits & CONNECTED_BIT) != 0)
	{
		return ESP_OK;
	}
	return ret;
	//return (bits & CONNECTED_BIT) != 0;
}


