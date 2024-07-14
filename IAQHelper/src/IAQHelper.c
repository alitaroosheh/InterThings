/**
 * @file IAQHelper.c
 * @author Ali Taroosheh (ali.taroosheh@gmail.com)
 * @brief Indoor Air Quality Sensor (BME680)
 * @version 0.1
 * @date 2024-07-02
 * 
 * 
 */

#include "IAQHelper.h"

static float humidityScore, gasScore;

// static float humidityWeighting = 0.25; // so humidity effect is 25% of the total air quality score
// static float gasWeighting = 0.75; // so gas effect is 75% of the total air quality score

static float humidityScore, gasScore;
static float gasReference = 250000;
static float humidityReference = 40;


/// @brief iaqAltitude: calculate altitude by means of preasure seaLevel default is 1013.25hPa
/// @param press 
/// @param seaLevel 
/// @return float iaqAltitude in Meter
float iaqAltitude(const int32_t press, const float seaLevel)
{

  static float iaqAltitude;
  iaqAltitude = 44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));
  return (iaqAltitude);
}

/// @brief iaqCalculateAirQuality: translate the IAQ to a meaningful sentence
/// @param score 
static void iaqCalculateAirQuality(float score)
{
  printf("Air quality is ");
  score = (100-score)*5;
  if      (score >= 301)                  printf("Hazardous\r\n");
  else if (score >= 201 && score <= 300 ) printf("Very Unhealthy\r\n");
  else if (score >= 176 && score <= 200 ) printf("Unhealthy\r\n");
  else if (score >= 151 && score <= 175 ) printf("Unhealthy for Sensitive Groups\r\n");
  else if (score >=  51 && score <= 150 ) printf("Moderate\r\n");
  else if (score >=  00 && score <=  50 ) printf("Good\r\n");
}

/// @brief iaqGetGasReference: make an avarage from incoming gas levels, each 10 sample will gather and get avarage. gasReference is the avarage result.
/// @param gas 
void iaqGetGasReference(float gas)
{
    // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.

    static int readings = 0;
    static float gasvalue = 250000;
    gasvalue += gas;
    readings++;
    if(readings >= 9)
    {
        gasReference = gasvalue / readings;
        readings = 0;
        gasvalue = gasReference;
    }
}

/// @brief iaqCalculateIndoorAirQuality: to get the AIQ value by means of humidity value.
/// @param currentHumidity
void iaqCalculateIndoorAirQuality(float currentHumidity)
{
    
    if (currentHumidity >= 38 && currentHumidity <= 42)
        humidityScore = 0.25*100; // humidityidity +/-5% around optimum 
    else
    { //sub-optimal
        if (currentHumidity < 38) 
            humidityScore = 0.25/humidityReference*currentHumidity*100;
        else
        {
            humidityScore = ((-0.25/(100-humidityReference)*currentHumidity)+0.416666)*100;
        }
    }

    //Calculate gas contribution to IAQ index
    float gas_lower_limit = 5000;   // Bad air quality limit
    float gas_upper_limit = 50000;  // Good air quality limit 
    if (gasReference > gas_upper_limit) gasReference = gas_upper_limit;
    if (gasReference < gas_lower_limit) gasReference = gas_lower_limit;
    gasScore = (0.75/(gas_upper_limit-gas_lower_limit)*gasReference -(gas_lower_limit*(0.75/(gas_upper_limit-gas_lower_limit))))*100;

    //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
    float air_qualityScore = humidityScore + gasScore;

    iaqCalculateAirQuality(air_qualityScore);
    printf("Air Quality = %.3f\r\n", air_qualityScore);

}


/// @brief iaqTask: this is a task to pull the Humidity, Temprature, Pressure, iaqAltitude and Air Quality values in a sequence.
/// @param parameters 
void iaqTask(void *parameters /*loopInterval in ms*/)
{
    uint32_t loopInterval = *((uint32_t*)parameters);
    struct bme68x_dev bme;
    int8_t rslt;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    struct bme68x_data data[3];
    uint32_t del_period;
    uint8_t n_fields;
    uint16_t sample_count = 1;

    /* Heater temperature in degree Celsius */
    uint16_t temp_prof[10] = { 200, 240, 280, 320, 360, 360, 320, 280, 240, 200 };

    /* Heating duration in milliseconds */
    uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

    /* Interface preference is updated as a parameter
     * For I2C : BME68X_I2C_INTF
     * For SPI : BME68X_SPI_INTF
     */
    uint8_t devAddress;
#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_0)
    devAddress = BME68X_I2C_ADDR_LOW;
#elif defined(CONFIG_EXAMPLE_I2C_ADDRESS_1)
    devAddress = BME68X_I2C_ADDR_HIGH;
#endif
    rslt = bme68x_interface_init(&bme, devAddress, BME68X_I2C_INTF);
    bme68x_check_rslt("bme68x_interface_init", rslt);

    rslt = bme68x_init(&bme);
    bme68x_check_rslt("bme68x_init", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    rslt = bme68x_get_conf(&conf, &bme);
    bme68x_check_rslt("bme68x_get_conf", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    conf.filter = BME68X_FILTER_OFF;
    conf.odr = BME68X_ODR_1000_MS; /* This parameter defines the sleep duration after each profile */
    conf.os_hum = BME68X_OS_16X;
    conf.os_pres = BME68X_OS_1X;
    conf.os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(&conf, &bme);
    bme68x_check_rslt("bme68x_set_conf", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp_prof = temp_prof;
    heatr_conf.heatr_dur_prof = dur_prof;
    heatr_conf.profile_len = 10;
    rslt = bme68x_set_heatr_conf(BME68X_SEQUENTIAL_MODE, &heatr_conf, &bme);//BME68X_SEQUENTIAL_MODE
    bme68x_check_rslt("bme68x_set_heatr_conf", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    rslt = bme68x_set_op_mode(BME68X_SEQUENTIAL_MODE, &bme);
    bme68x_check_rslt("bme68x_set_op_mode", rslt);

    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    while (true)
    {
        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_SEQUENTIAL_MODE, &conf, &bme) + (heatr_conf.heatr_dur_prof[0] * 1000);
        bme.delay_us(del_period, bme.intf_ptr);

        rslt = bme68x_get_data(BME68X_SEQUENTIAL_MODE, data, &n_fields, &bme);
        bme68x_check_rslt("bme68x_get_data", rslt);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        for (uint8_t i = 0; i < n_fields; i++)
        {
            printf("\r\n*********************\r\n");
            printf("Temperature(deg C): %.3f\r\n", data[i].temperature);
            printf("Pressure(Pa): %.3f\r\n", data[i].pressure);
            printf("humidity(%%): %.3f\r\n", data[i].humidity);
            printf("Gas resistance(ohm): %.3f\r\n", data[i].gas_resistance);
            printf("Altitud: %.3f\r\n", iaqAltitude(data[i].pressure, 1013.25));
            iaqGetGasReference(data[i].gas_resistance);
            iaqCalculateIndoorAirQuality(data[i].humidity);
            printf("\r\n*********************\r\n\r\n\r\n");
            sample_count++;
        }
        vTaskDelay(loopInterval / portTICK_PERIOD_MS);
    }
    bme68x_coines_deinit();
}


