#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "D:/ESP/bin/Project0/components/cJSON/include/cJSON.h"
#include "D:/ESP/bin/Project0/components/WiFi_Fn/include/WiFi_Fn.h"
#include "D:/ESP/bin/Project0/components/MQTT_Fn/include/MQTT_Fn.h"
#include "D:/ESP/bin/Project0/components/oneShotADC_Fn/include/oneShotADC_Fn.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

static const char *TAG = "LOG_main";

// Declare a global MQTT client handle
esp_mqtt_client_handle_t client = NULL;
static int msg_id; 

void app_main(void)
{	
	ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);
		
	esp_err_t status = WIFI_FAILURE;
	//initialize storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
      // connect to wireless AP
	status = connect_wifi();
	if (WIFI_SUCCESS != status)
	{
		ESP_LOGI(TAG, "Failed to associate to AP, dying...");
		return;
	}
	//MQTT Start
	MQTT_CLIENT_INIT();
	extern char *received_data ;
	//oneShot init
	adc_oneshot_init();
	// Creating JSON OBJ
	cJSON *msg  = cJSON_CreateObject();
	if (msg == NULL) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        cJSON_Delete(msg) ;
        return;
    }
    //JSON OBJ Initialization
    cJSON_AddNumberToObject(msg, "Temperature", 0);
	cJSON_AddNumberToObject(msg, "Humidity", 0);
	cJSON_AddNumberToObject(msg, "Battery_Health", 0);
	cJSON_AddNumberToObject(msg, "Signal_Strength", 0);

	while(1) {
		cJSON_ReplaceItemInObject(msg, "Temperature", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_0)));
    	cJSON_ReplaceItemInObject(msg, "Humidity", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_3)));
		cJSON_ReplaceItemInObject(msg, "Battery_Health", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_0) % 100));
    	cJSON_ReplaceItemInObject(msg, "Signal_Strength", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_3) % 100));
		// Convert JSON object to string
    	char *json_string = cJSON_PrintUnformatted(msg); 
		// Publish a message 
   		msg_id = esp_mqtt_client_publish(client, "sensor/status", json_string , 0, 1, 0);
   		
   		ESP_LOGI(TAG, "Message published with msg_id=%d", msg_id);
   		vTaskDelay(500);
   	}
}