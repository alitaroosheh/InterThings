/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include "coines.h"
#include "common.h"

#define TAG "common"

/******************************************************************************/
/*!                Static variable definition                                 */
static uint8_t dev_addr;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * I2C read function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t*)intf_ptr;
    (void)intf_ptr;    

    int result =  i2cGet(device_addr, reg_addr, reg_data, len);
    return result == ESP_OK ? 0:-1;
}

/*!
 * I2C write function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t device_addr = *(uint8_t*)intf_ptr;
    (void)intf_ptr;


    esp_err_t result = i2cSend (device_addr, reg_addr, reg_data, len);
    return result == ESP_OK ? 0:-1;
}

/*!
 * SPI read function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return -1;
}

/*!
 * SPI write function map to COINES platform
 */
BME68X_INTF_RET_TYPE bme68x_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    return -1;
}

/*!
 * Delay function map to COINES platform
 */
void bme68x_delay_us(uint32_t period, void *intf_ptr)
{
    (void)intf_ptr;
    vTaskDelay(period/1000);
}

void bme68x_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
        case BME68X_OK:

            /* Do nothing */
            break;
        case BME68X_E_NULL_PTR:
            //printf("API name [%s]  Error [%d] : Null pointer\r\n", api_name, rslt);
            break;
        case BME68X_E_COM_FAIL:
            //printf("API name [%s]  Error [%d] : Communication failure\r\n", api_name, rslt);
            break;
        case BME68X_E_INVALID_LENGTH:
            //printf("API name [%s]  Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
            break;
        case BME68X_E_DEV_NOT_FOUND:
            //printf("API name [%s]  Error [%d] : Device not found\r\n", api_name, rslt);
            break;
        case BME68X_E_SELF_TEST:
            //printf("API name [%s]  Error [%d] : Self test error\r\n", api_name, rslt);
            break;
        case BME68X_W_NO_NEW_DATA:
            //printf("API name [%s]  Warning [%d] : No new data found\r\n", api_name, rslt);
            break;
        default:
            //printf("API name [%s]  Error [%d] : Unknown error code\r\n", api_name, rslt);
            break;
    }
}

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    
    i2cConfig(i2c_master_port, 100000, I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    return ESP_OK;
}

int8_t bme68x_interface_init(struct bme68x_dev *bme, uint8_t devAddress, uint8_t intf)
{
    int8_t rslt = BME68X_OK;

    if (bme != NULL)
    {
        /* Bus configuration : I2C */
        if (intf == BME68X_I2C_INTF)
        {
            dev_addr = devAddress;
            bme->read = bme68x_i2c_read;
            bme->write = bme68x_i2c_write;
            bme->intf = BME68X_I2C_INTF;
            
            i2c_master_init();
        }
        /* Bus configuration : SPI */
        else if (intf == BME68X_SPI_INTF)
        {
            // dev_addr = COINES_SHUTTLE_PIN_7;
            // bme->read = bme68x_spi_read;
            // bme->write = bme68x_spi_write;
            // bme->intf = BME68X_SPI_INTF;
            //(void)coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_7_5_MHZ, COINES_SPI_MODE0);    // done
        }

        vTaskDelay(100);

        bme->delay_us = bme68x_delay_us;
        bme->intf_ptr = &dev_addr;
        bme->amb_temp = 25; /* The ambient temperature in deg C is used for defining the heater temperature */
    }
    else
    {
        rslt = BME68X_E_NULL_PTR;
    }
    return rslt;
}

void bme68x_coines_deinit(void)
{
    (void)fflush(stdout);
    vTaskDelay(1000);
    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    vTaskDelay(1000);
}
