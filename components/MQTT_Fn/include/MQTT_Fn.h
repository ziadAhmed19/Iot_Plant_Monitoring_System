#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "driver/gpio.h"

#include "mqtt_client.h"

#include "hal/adc_types.h"

#include "D:/ESP/bin/Project0/components/cJSON/include/cJSON.h"
#include "D:/ESP/bin/Project0/components/oneShotADC_Fn/include/oneShotADC_Fn.h"

#define MQTT_QUEUE_LENGTH 5
#define ALARM_GPIO_PIN GPIO_NUM_2

typedef enum {
    CMD_UNKNOWN = -1,
    CMD_DECREASE_TEMPERATURE,
    CMD_INCREASE_TEMPERATURE,
    CMD_DECREASE_HUMIDITY,
    CMD_INCREASE_HUMIDITY,
    CMD_FAN_SPEED_LOW,
    CMD_FAN_SPEED_HIGH,
    CMD_START_IRRIGATION,
    CMD_STOP_IRRIGATION,
    CMD_LIGHT_ON,
    CMD_LIGHT_OFF,
    CMD_START_FERTILIZING,
    CMD_STOP_FERTILIZING,
    CMD_AUTOMODE_ON,
    CMD_AUTOMODE_OFF,
    CMD_PESTCONTROL_ON,
    CMD_PESTCONTROL_OFF,
    CMD_ALARM_ON,
    CMD_ALARM_OFF
} CommandType;


void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void MQTT_CLIENT_INIT();
void MQTT_Queue_init();
void vMQTT_QUEUE_PROCESSING_TASK(void *pvParameters);
void vMQTT_DATA_PUBLISHING(void *pvParameters);
CommandType get_command_type(const char *cmd);