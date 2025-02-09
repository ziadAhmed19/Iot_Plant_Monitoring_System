#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "D:/ESP/bin/Project0/components/WiFi_Fn/include/WiFi_Fn.h"
#include "D:/ESP/bin/Project0/components/MQTT_Fn/include/MQTT_Fn.h"

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
	MQTT_Queue_init();
	adc_oneshot_init();
	
	// Create a task to process received MQTT messages
    xTaskCreate(vMQTT_QUEUE_PROCESSING_TASK,
     "MQTT Process Task",
      2248,
       NULL,
       5,
        NULL);
	// Create a task to publish MQTT 
	xTaskCreate(vMQTT_DATA_PUBLISHING,
	"MQTT Publishing Task",
	 2268,
	  NULL,
	   4,
	   NULL);


}