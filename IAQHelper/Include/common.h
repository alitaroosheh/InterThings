/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include "bme68x.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "bme68x.h"
#include "driver/i2c.h"
#include "I2CHelper.h"


#define I2C_MASTER_SCL_IO           CONFIG_EXAMPLE_I2C_MASTER_SCL       /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_EXAMPLE_I2C_MASTER_SDA       /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                                   /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                              /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define PORT 0
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_0)
#define ADDR BME680_I2C_ADDR_0
#endif
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_1)
#define ADDR BME680_I2C_ADDR_1
#endif

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

/*!
 *  @brief Function to select the interface between SPI and I2C.
 *
 *  @param[in] bme      : Structure instance of bme68x_dev
 *  @param[in] intf     : Interface selection parameter
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t devAddress, uint8_t intf);

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] len          : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose value is to be written.
 *  @param[in] len          : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] len          : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose data has to be written.
 *  @param[in] len          : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = BME68X_INTF_RET_SUCCESS -> Success
 *  @retval != BME68X_INTF_RET_SUCCESS  -> Failure Info
 *
 */
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input provided in some of the
 * APIs.
 *
 *  @param[in] period       : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 *
 */
void bme68x_delay_us(uint32_t period, void *intf_ptr);

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : Name of the API whose execution status has to be printed.
 *  @param[in] rslt     : Error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bme68x_check_rslt(const char api_name[], int8_t rslt);

/*!
 *  @brief Deinitializes coines platform
 *
 *  @return void.
 */
void bme68x_coines_deinit(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */
