#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void adc_oneshot_init(void);
int adc_oneshot_read_value(uint8_t Read_ADC_CHANNEL);