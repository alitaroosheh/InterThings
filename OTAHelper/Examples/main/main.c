/**
 * @file main.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-26
 * 
 * 
 */

#include <stdio.h>
#include "NVSHelper.h"
#include "OTAHelper.h"
#include "WiFiHelper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
//#include "mbedtls/ssl_internal.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
//#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"
#include "esp_http_client.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#define TAG "OTAHelper_Example"



#define CERT_URL "https://test.mosquitto.org/ssl/mosquitto.org.crt"

#define CERT_FILE_PATH "/spiffs/mosquitto.org.crt"

#define TAG "showcert"
#define SERVER_NAME "test.mosquitto.org"
#define SERVER_PORT "8883"

#define lable "config_nvs"


void show_certificate(mbedtls_x509_crt *cert)
{
    unsigned char buf[1024];
    int ret = mbedtls_x509_crt_info((char *)buf, sizeof(buf) - 1, "  ", cert);
    if (ret > 0) {
        buf[ret] = 0;
        ESP_LOGI(TAG, "\n%s", buf);
    } else {
        ESP_LOGE(TAG, "Failed to parse certificate info");
    }
}

void retrieve_and_show_cert(void)
{
    int ret;
    mbedtls_net_context server_fd;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    const char *pers = "ssl_client1";

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, strlen(pers))) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        goto exit;
    }

    if ((ret = mbedtls_net_connect(&server_fd, SERVER_NAME, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ESP_LOGE(TAG, "mbedtls_net_connect returned %d", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned %d", ret);
        goto exit;
    }

    if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned %d", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
            goto exit;
        }
    }

    ESP_LOGI(TAG, "SSL handshake successful");

    const mbedtls_x509_crt *peer_cert = mbedtls_ssl_get_peer_cert(&ssl);
    if (peer_cert != NULL) {
        show_certificate((mbedtls_x509_crt *)peer_cert);
    } else {
        ESP_LOGE(TAG, "Failed to retrieve peer certificate");
    }

exit:
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_net_free(&server_fd);
}




esp_err_t download_certificate(const char *url, const char *file_path)
{
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = NULL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        esp_http_client_cleanup(client);
        return err;
    }

    // FILE *f = fopen(file_path, "w");
    // if (f == NULL) {
    //     esp_http_client_cleanup(client);
    //     return ESP_FAIL;
    // }

    char buffer[512];
    int data_read;
	ESP_LOGI(TAG, "CRT FILE:");
    while ((data_read = esp_http_client_read(client, buffer, sizeof(buffer))) > 0) {
		for(int idx=0;idx<data_read;idx++)
			printf("%c", buffer[idx]);
        //fwrite(buffer, 1, data_read, f);
    }
    //fclose(f);
    esp_http_client_cleanup(client);
	ESP_LOGI(TAG, "END OF CRT FILE");

    return ESP_OK;
}

void app_main()
{

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK( nvs_flash_erase() );
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

	wifiInit();

	
	ESP_LOGW(TAG, "Start STA Mode");

	wifi_config_t wifiConfigSTA = { 0 };
	strcpy((char *)wifiConfigSTA.sta.ssid, "IoTWiFi");
	strcpy((char *)wifiConfigSTA.sta.password, "alitaroosheh1234");

	wifiStartSTA(wifiConfigSTA, 5*1000);

    // if (download_certificate(CERT_URL, CERT_FILE_PATH) == ESP_OK) {
    //     ESP_LOGI(TAG, "Certificate downloaded successfully.");
    // } else {
    //     ESP_LOGE(TAG, "Failed to download certificate.");
    //     return;
    // }

	retrieve_and_show_cert();


}