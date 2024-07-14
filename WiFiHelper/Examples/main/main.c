#include <stdio.h>
#include "WiFiHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define TAG "WiFiHelper"

void app_main()
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK( nvs_flash_erase() );
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	wifiInit();

#if CONFIG_WIFI_CONNECT_AP
	ESP_LOGW(TAG, "Start AP Mode");
	wifi_config_t wifiConfigAP = { 0 };
	strcpy((char *)wifiConfigAP.ap.ssid,CONFIG_AP_WIFI_SSID);
	strcpy((char *)wifiConfigAP.ap.password, CONFIG_AP_WIFI_PASSWORD);
	wifiConfigAP.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	wifiConfigAP.ap.ssid_len = strlen(CONFIG_AP_WIFI_SSID);
	wifiConfigAP.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	wifiConfigAP.ap.channel = CONFIG_AP_WIFI_CHANNEL;

	if (strlen(CONFIG_AP_WIFI_PASSWORD) == 0) {
		wifiConfigAP.ap.authmode = WIFI_AUTH_OPEN;
	}
	wifiStartAP(wifiConfigAP);
#elif CONFIG_WIFI_CONNECT_STA
	ESP_LOGW(TAG, "Start STA Mode");

	wifi_config_t wifiConfigSTA = { 0 };
	strcpy((char *)wifiConfigSTA.sta.ssid, CONFIG_STA_WIFI_SSID);
	strcpy((char *)wifiConfigSTA.sta.password, CONFIG_STA_WIFI_PASSWORD);

	wifiStartSTA(wifiConfigSTA, CONFIG_STA_CONNECT_TIMEOUT*1000);
#elif CONFIG_WIFI_CONNECT_APSTA
	ESP_LOGW(TAG, "Start APSTA Mode");

	wifi_config_t ap_config = { 0 };
	strcpy((char *)ap_config.ap.ssid,CONFIG_AP_WIFI_SSID);
	strcpy((char *)ap_config.ap.password, CONFIG_AP_WIFI_PASSWORD);
	ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	ap_config.ap.ssid_len = strlen(CONFIG_AP_WIFI_SSID);
	ap_config.ap.max_connection = CONFIG_AP_MAX_STA_CONN;
	ap_config.ap.channel = CONFIG_AP_WIFI_CHANNEL;

	if (strlen(CONFIG_AP_WIFI_PASSWORD) == 0) {
		ap_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	wifi_config_t sta_config = { 0 };
	strcpy((char *)sta_config.sta.ssid, CONFIG_STA_WIFI_SSID);
	strcpy((char *)sta_config.sta.password, CONFIG_STA_WIFI_PASSWORD);

	wifiStartAPSTA(ap_config, sta_config, CONFIG_STA_CONNECT_TIMEOUT*1000);
#endif
}