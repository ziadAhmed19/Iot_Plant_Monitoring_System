#include "D:/ESP/bin/Project0/components/MQTT_Fn/include/MQTT_Fn.h"


static const char *TAG = "LOG_MQTT";

extern esp_mqtt_client_handle_t client ;

QueueHandle_t MQTT_Queue = NULL;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data){
	
	esp_mqtt_event_handle_t event = event_data; 
	client = event->client;
    
    switch((esp_mqtt_event_id_t)event_id)
    {
		case MQTT_EVENT_BEFORE_CONNECT:
		ESP_LOGI(TAG,"The client is initialized and about to start connecting to the broker...");
		break ;
		
		case MQTT_EVENT_CONNECTED :
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "sensor/status", 1);
        esp_mqtt_client_subscribe(client, "sensor/control", 1);
        
		break;
		
		case MQTT_EVENT_DISCONNECTED:
		ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
		
		case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        
        break;
		
		case MQTT_EVENT_UNSUBSCRIBED :
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
		
		case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);		
		
		break;
		
		case MQTT_EVENT_DATA :
		// Displaying data and status
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        
        // Create buffer to store received data
        char *receivedMessage = malloc(event->data_len + 1);
        strncpy(receivedMessage, event->data, event->data_len);
        receivedMessage[event->data_len] = '\0';  // Null-terminate the string
		
		// Send data to queue
        if (xQueueSend(MQTT_Queue,
         	&receivedMessage,
          	pdMS_TO_TICKS(100)) != pdPASS) {
        	
        	ESP_LOGE("MQTT", "Failed to send data to queue!");
        	free(receivedMessage);  // Free memory if queue is full
        }		
		
		break;
		
		case MQTT_EVENT_ERROR :
		 ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) 
        {
            esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
			esp_log_level_set("transport", ESP_LOG_VERBOSE);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
		break;
		
		case MQTT_EVENT_ANY :
		ESP_LOGI(TAG,"MQTT_EVENT_ANY");
		break;
		
		case MQTT_EVENT_DELETED :
		ESP_LOGI(TAG,"MQTT_EVENT_DELETED");
		break;
		
		case MQTT_USER_EVENT:
		ESP_LOGI(TAG,"MQTT_USER_EVENTs");
		break;
		
		default:
		ESP_LOGI(TAG,"UNIDENFIED EVENT");
	}
}


void MQTT_CLIENT_INIT(){
	
	ESP_LOGI(TAG,"Initialization of Client...");
	MQTT_Queue_init();
	esp_mqtt_client_config_t client_config = {
		.broker = {
			.address.hostname = "test.mosquitto.org",
			.address.port = 1883,
			.address.transport = MQTT_TRANSPORT_OVER_TCP
		},
		.credentials = {
			.username = NULL,
			.authentication.password = NULL
			//.client_id = "mqttx_dfd37fc1"
		},
		.task = {
			.priority = 3,
			.stack_size = 8192
		},
		.buffer = {
			.size = 1024,
			.out_size = 512
		},
		.session.keepalive = 60
	};
	client = esp_mqtt_client_init(&client_config);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
	esp_mqtt_client_start(client);
	
	
} 


void MQTT_Queue_init(){
	
	MQTT_Queue = xQueueCreate(MQTT_QUEUE_LENGTH, sizeof(char *));
	//Error Handling
	if(MQTT_Queue == NULL){
		ESP_LOGE(TAG,"Queue was not created");	 
		vQueueDelete( MQTT_Queue );
	}
	
	gpio_set_direction(ALARM_GPIO_PIN,GPIO_MODE_OUTPUT);
}

CommandType get_command_type(const char *cmd) {
    if (strcmp(cmd, "DECREASE_TEMPERATURE") == 0) 	return CMD_DECREASE_TEMPERATURE;
    if (strcmp(cmd, "INCREASE_TEMPERATURE") == 0) 	return CMD_INCREASE_TEMPERATURE;
    if (strcmp(cmd, "DECREASE_HUMIDITY") == 0) 		return CMD_DECREASE_HUMIDITY;
    if (strcmp(cmd, "INCREASE_HUMIDITY") == 0) 		return CMD_INCREASE_HUMIDITY;
    if (strcmp(cmd, "FAN_SPEED_LOW") == 0) 			return CMD_FAN_SPEED_LOW;
    if (strcmp(cmd, "FAN_SPEED_HIGH") == 0) 		return CMD_FAN_SPEED_HIGH;
    if (strcmp(cmd, "START_IRRIGATION") == 0) 		return CMD_START_IRRIGATION;
    if (strcmp(cmd, "STOP_IRRIGATION") == 0) 		return CMD_STOP_IRRIGATION;
    if (strcmp(cmd, "LIGHT_ON") == 0) 				return CMD_LIGHT_ON;
    if (strcmp(cmd, "LIGHT_OFF") == 0) 				return CMD_LIGHT_OFF;
    if (strcmp(cmd, "START_FERTILIZING") == 0) 		return CMD_START_FERTILIZING;
    if (strcmp(cmd, "STOP_FERTILIZING") == 0) 		return CMD_STOP_FERTILIZING;
    if (strcmp(cmd, "AUTOMODE_ON") == 0) 			return CMD_AUTOMODE_ON;
    if (strcmp(cmd, "AUTOMODE_OFF") == 0) 			return CMD_AUTOMODE_OFF;
    if (strcmp(cmd, "PESTCONTROL_ON") == 0) 		return CMD_PESTCONTROL_ON;
    if (strcmp(cmd, "PESTCONTROL_OFF") == 0) 		return CMD_PESTCONTROL_OFF;
    if (strcmp(cmd, "ALARM_ON") == 0) 				return CMD_ALARM_ON;
    if (strcmp(cmd, "ALARM_OFF") == 0) 				return CMD_ALARM_OFF;
    
    
    return CMD_UNKNOWN;  // If command is not recognized
}

void vMQTT_QUEUE_PROCESSING_TASK(void *pvParameters){
	char *receivedMessage;
		while(1){
			
		// Wait for a message in the queue
        if (xQueueReceive(MQTT_Queue,
         	&receivedMessage,
         	portMAX_DELAY) == pdTRUE){
				
				ESP_LOGI("MQTT_PROCESS", "Processing message: %s", receivedMessage);
				
				// Get the command type as an enum
            	CommandType command = get_command_type(receivedMessage);
            	
            switch (command) {
               case CMD_DECREASE_TEMPERATURE:
                    ESP_LOGI("ACTION", "Decreasing Temperature");
                    // decrease_temperature();
                    break;
               case CMD_INCREASE_TEMPERATURE:
                    ESP_LOGI("ACTION", "Increasing Temperature");
                    // increase_temperature();
                    break;
                    
               case CMD_DECREASE_HUMIDITY:
                    ESP_LOGI("ACTION", "Decreasing Humidity");                    
                    break;                
               case CMD_INCREASE_HUMIDITY:
                    ESP_LOGI("ACTION", "Increasing Humidity");                    
                    break;
                    
               case CMD_FAN_SPEED_LOW:
                    ESP_LOGI("ACTION", "Setting Fan Speed to LOW");
                    // fan_speed_low();
                    break;
               case CMD_FAN_SPEED_HIGH:
                    ESP_LOGI("ACTION", "Setting Fan Speed to HIGH");
                    // fan_speed_high();
                    break;
                    
               case CMD_LIGHT_ON:
                    ESP_LOGI("ACTION", "Turning Light ON");
                    // light_on();
                    break;
               case CMD_LIGHT_OFF:
                    ESP_LOGI("ACTION", "Turning Light OFF");
                    // light_off();
                    break;
               case CMD_START_IRRIGATION:
                    ESP_LOGI("ACTION", "Starting Irrigation");
                    // start_irrigation();
                    break;
               case CMD_STOP_IRRIGATION:
                    ESP_LOGI("ACTION", "Stopping Irrigation");
                    // stop_irrigation();
                    break;
                    
               case CMD_START_FERTILIZING:
                    ESP_LOGI("ACTION", "START FERTILIZING");                    
                    break;
               case CMD_STOP_FERTILIZING:
                    ESP_LOGI("ACTION", "STOP FERTILIZING");                    
                    break;
                
               case CMD_AUTOMODE_ON:
                    ESP_LOGI("ACTION", "AUTOMODE ON");                    
                    break;
               case CMD_AUTOMODE_OFF:
                    ESP_LOGI("ACTION", "AUTOMODE OFF");                    
                    break;
               
               case CMD_PESTCONTROL_ON:
                    ESP_LOGI("ACTION", "PESTCONTROL ON");                    
                    break;
               case CMD_PESTCONTROL_OFF:
                    ESP_LOGI("ACTION", "PESTCONTROL OFF");                    
                    break;
                    
               case CMD_ALARM_ON:
                    ESP_LOGI("ACTION", "ALARM ON");
                    gpio_set_level(ALARM_GPIO_PIN, 1);                    
                    break;
               case CMD_ALARM_OFF:
                    ESP_LOGI("ACTION", "ALARM OFF");                    
                    gpio_set_level(ALARM_GPIO_PIN, 0);
                    break;
                default:
                    ESP_LOGW("MQTT_PROCESS", "Unknown command: %s", receivedMessage);
                    break;
            		}

            	free(receivedMessage);  // Free dynamically allocated memory 
		}
	}
}

void GPIO_INIT(){
	
	gpio_reset_pin(ALARM_GPIO_PIN); 
    gpio_set_direction(ALARM_GPIO_PIN, GPIO_MODE_OUTPUT);
}


void vMQTT_DATA_PUBLISHING(void *pvParameters){
	
	static int msg_id;
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
	cJSON_AddNumberToObject(msg, "lightLevel", 0);
	cJSON_AddNumberToObject(msg, "soilMoisture", 0);
	cJSON_AddNumberToObject(msg, "soilNutrients", 0);
	
	while(1){
		
		cJSON_ReplaceItemInObject(msg, "Temperature", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_0)%100));
    	cJSON_ReplaceItemInObject(msg, "Humidity", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_3)%100));
		cJSON_ReplaceItemInObject(msg, "Battery_Health", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_0) % 100));
    	cJSON_ReplaceItemInObject(msg, "Signal_Strength", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_3) % 100));
    	cJSON_ReplaceItemInObject(msg, "lightLevel", cJSON_CreateNumber((adc_oneshot_read_value(ADC_CHANNEL_3) - 7) % 50));
    	cJSON_ReplaceItemInObject(msg, "soilMoisture", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_0) % 30));
    	cJSON_ReplaceItemInObject(msg, "soilNutrients", cJSON_CreateNumber(adc_oneshot_read_value(ADC_CHANNEL_3) % 70));
    	
    	
		// Convert JSON object to string
    	char *json_string = cJSON_PrintUnformatted(msg); 
		// Publish a message 
   		msg_id = esp_mqtt_client_publish(client, "sensor/status", json_string , 0, 1, 0);
   		ESP_LOGI(TAG, "Message published with msg_id=%d", msg_id);
   		vTaskDelay(500);	
   		}
}