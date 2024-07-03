/**
 * @file I2CHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */


#pragma once

#include <stdio.h>
#include <stdint.h>
#include "driver/i2c.h"
#include "esp_log.h"




void i2cConfig(i2c_port_t port, uint32_t frequency, uint8_t sda, uint8_t scl);
esp_err_t i2cGet(uint8_t chip_address, uint8_t register_address, uint8_t *data, uint32_t data_length);
esp_err_t i2cSend (uint8_t chip_address, uint8_t register_address, const uint8_t *data, uint32_t data_length);

