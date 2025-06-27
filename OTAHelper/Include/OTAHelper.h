/**
 * @file OTAHelper.h
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-26
 * 
 * 
 */

#pragma once

#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_sec_provider.h"

#if SOC_HMAC_SUPPORTED
#include "bootloader_random.h"
#include "esp_random.h"

#include "esp_efuse.h"
#include "esp_efuse_chip.h"
#endif  // SOC_HMAC_SUPPORTED

