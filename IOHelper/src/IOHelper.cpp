/**
 * @file IOHelper.cpp
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-11
 * 
 * 
 */

#include "IOHelper.h"

#include <vector>
#include <algorithm>

#define TAG "IOHelperCPP"


SemaphoreHandle_t vector_mutex;

std::vector<ioHoldConfig> holdConfig;

extern "C" esp_err_t ioHoldAppend(ioHoldConfig hold)
{
    // Check if an element with the same pin already exists
	takeMutex(vector_mutex);
    auto it = std::find_if
	(
		holdConfig.begin(),
		holdConfig.end(),
        [hold](const ioHoldConfig& config)
		{ return config.pin == hold.pin; }
	);

    if (it != holdConfig.end())
	{
		giveMutex(vector_mutex);
        return ESP_ERR_NOT_ALLOWED; // Pin already exists, return error
    }

    // Pin does not exist, append to the vector
    holdConfig.push_back(hold);
	giveMutex(vector_mutex);
    return ESP_OK; // Successfully appended
}

extern "C" esp_err_t ioHoldUpdate(ioHoldConfig hold)
{

	takeMutex(vector_mutex);
    auto it = std::find_if
	(
		holdConfig.begin(),
		holdConfig.end(),
		[hold](const ioHoldConfig& config)
		{ return config.pin == hold.pin; }
	);

    if (it != holdConfig.end()) {
        *it = hold; // Update the found element with the new values
        giveMutex(vector_mutex);
        return ESP_OK; // Successfully updated
    }

    giveMutex(vector_mutex);
    return ESP_ERR_NOT_FOUND; // Pin not found
}

extern "C" esp_err_t ioHoldGetAt(uint32_t index, ioHoldConfig *hold)
{

	takeMutex(vector_mutex);
    if (index >= holdConfig.size())
	{
		giveMutex(vector_mutex);
        return ESP_ERR_NOT_ALLOWED; // Index out of bounds
    }
	*hold = holdConfig[index];
	giveMutex(vector_mutex);
    return ESP_OK;

}

extern "C" uint32_t ioHoldCount()
{

	takeMutex(vector_mutex);
	uint32_t result = holdConfig.size();
	giveMutex(vector_mutex);
    return result;

}

extern "C" esp_err_t ioHoldFindByPin(uint64_t pin, ioHoldConfig *hold)
{

	takeMutex(vector_mutex);
    if (hold == nullptr)
	{
		giveMutex(vector_mutex);
        return ESP_ERR_NOT_FOUND; // Invalid pointer
    }

    auto it = std::find_if
	(
		holdConfig.begin(),
		holdConfig.end(),
		[pin](const ioHoldConfig& config)
		{ return config.pin == pin; }
	);

    if (it != holdConfig.end())
	{
        *hold = (*it); // Copy the found element to the provided pointer
		giveMutex(vector_mutex);
        return ESP_OK; // Successfully found
    }

	giveMutex(vector_mutex);
    return ESP_ERR_NOT_FOUND; // Pin not found
}


extern "C" esp_err_t ioHoldDeleteByPin(uint64_t pin)
{

	takeMutex(vector_mutex);
    auto it = std::remove_if
	(
		holdConfig.begin(),
		holdConfig.end(),
        [pin](const ioHoldConfig& config)
		{ return config.pin == pin; }
	);

    if (it != holdConfig.end())
	{
        holdConfig.erase(it, holdConfig.end()); // Erase the removed elements from the vector
		giveMutex(vector_mutex);
        return ESP_OK; // Successfully deleted
    }

	giveMutex(vector_mutex);
    return ESP_ERR_NOT_FOUND; // Pin not found

}
