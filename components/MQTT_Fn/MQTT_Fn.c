#include "D:/ESP/bin/Project0/components/MQTT_Fn/include/MQTT_Fn.h"
#include <stdint.h>

static int msg_id; 
char *received_data ;
static const char *TAG = "LOG_MQTT";
extern esp_mqtt_client_handle_t client ;

const char* get_received_data() {
    return received_data;
}

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
        msg_id = esp_mqtt_client_subscribe(client, "sensor/status", 1);
        
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
		
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        received_data = event->data;
        printf("DATA=%.*s\r\n", event->data_len, event->data);
		
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
