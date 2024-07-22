/**
 * @file I2CHelper.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */

#include "I2CHelper.h"

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define WRITE_BIT I2C_MASTER_WRITE  /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ    /*!< I2C master read */
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */

static const char *TAG = "I2CHelper";

static SemaphoreHandle_t i2c_mutex; 


static struct {
    i2c_port_t      port;
    uint32_t        freq;
    uint8_t         sda;
    uint8_t         scl;
}config;

/// @brief i2cConfig
/// @param port 
/// @param frequency 
/// @param sda 
/// @param scl 
/// @return esp_err_t
esp_err_t i2cConfig(i2c_port_t port, uint32_t frequency, uint8_t sda, uint8_t scl)
{

	takeMutex(i2c_mutex);
    config.port = port;
    config.freq = frequency;
    config.sda = sda;
    config.scl = scl;
	giveMutex(i2c_mutex);
    return ESP_OK;
}

/// @brief i2cMasterDriverInitialize static function to configure the I2C bus in master mode
/// @param  
/// @return esp_err_t
static esp_err_t i2cMasterDriverInitialize(void)
{

	takeMutex(i2c_mutex);
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = config.sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = config.scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = config.freq,
    };
    esp_err_t ret = i2c_param_config(config.port, &conf);
	giveMutex(i2c_mutex);
    return ret;

}

/// @brief i2cGet to get data from I2C bus
/// @param chip_address 
/// @param register_address 
/// @param data 
/// @param data_length 
/// @return 
esp_err_t i2cGet(uint8_t chip_address, uint8_t register_address, uint8_t *data, uint32_t data_length)
{

	takeMutex(i2c_mutex);
    i2c_driver_install(config.port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    i2cMasterDriverInitialize();
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    if ((int8_t)register_address != -1) {
        i2c_master_write_byte(cmd, chip_address << 1 | WRITE_BIT, ACK_CHECK_EN);
        i2c_master_write_byte(cmd, register_address, ACK_CHECK_EN);
        i2c_master_start(cmd);
    }
    i2c_master_write_byte(cmd, chip_address << 1 | READ_BIT, ACK_CHECK_EN);
    if (data_length > 1) {
        i2c_master_read(cmd, data, data_length - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data + data_length - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(config.port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK)
    {
        //ESP_LOGI(TAG, "I2C read successful!");
    }
    else if (ret == ESP_ERR_TIMEOUT)
    {
        ESP_LOGW(TAG, "Bus is busy");
    }
    else
    {
        ESP_LOGW(TAG, "Read failed");
    }
    ret = i2c_driver_delete(config.port);
	giveMutex(i2c_mutex);
    return ret;

}
/// @brief i2cSend send data over I2C bus
/// @param chip_address 
/// @param register_address 
/// @param data 
/// @param data_length 
/// @return esp_err_t
esp_err_t i2cSend (uint8_t chip_address, uint8_t register_address, const uint8_t *data, uint32_t data_length)
{

	takeMutex(i2c_mutex);
    i2c_driver_install(config.port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    i2cMasterDriverInitialize();
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_address << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_address, ACK_CHECK_EN);
    for (int i = 0; i < data_length; i++) {
        i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(config.port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK)
    {
        //ESP_LOGI(TAG, "Write OK");
    }
    else if (ret == ESP_ERR_TIMEOUT)
    {
        ESP_LOGW(TAG, "Bus is busy");
    }
    else
    {
        ESP_LOGW(TAG, "Write Failed");
    }
    ret = i2c_driver_delete(config.port);
	giveMutex(i2c_mutex);
    return ret;

}