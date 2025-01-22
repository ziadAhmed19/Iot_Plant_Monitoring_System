#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
#include "../oneShotADC_Fn/include/oneShotADC_Fn.h"

// Define the ADC channel and bit width
#define ADC_CHANNEL        ADC_CHANNEL_0    // GPIO 36 (ADC1 channel 0)
#define ADC_WIDTH          ADC_BITWIDTH_12 // 12-bit resolution
int adc_reading = 0 ;
// ADC task handle
TaskHandle_t adc_task_handle = NULL;

// ADC One-shot configuration handle
adc_oneshot_unit_handle_t adc_handle;

// Function to initialize ADC in oneshot mode
void adc_oneshot_init(void) {
    // ADC unit configuration
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .clk_src = 0
    };
    adc_oneshot_new_unit(&init_config, &adc_handle) ;

    // Channel configuration
    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN_DB_12,  // Full range up to 3.3V 
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &channel_config);
}

// Function to perform a one-shot ADC read
int adc_oneshot_read_value(uint8_t Read_ADC_CHANNEL) {
    int adc_reading = 0;

    // Perform the ADC read
    esp_err_t err = adc_oneshot_read(adc_handle, Read_ADC_CHANNEL, &adc_reading);
    if (err == ESP_OK) {
        printf("ADC Reading: %d\n", adc_reading);
    } else {
        printf("Error reading ADC: %s\n", esp_err_to_name(err));
        adc_reading = -1; // Indicate error with -1
    }

    return adc_reading;
}
