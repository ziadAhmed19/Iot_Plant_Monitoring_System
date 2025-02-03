# Code Overview

# Global Variables

	static int msg_id; 
	char *received_data;
	static const char *TAG = "LOG_MQTT";
	extern esp_mqtt_client_handle_t client;

	msg_id			: Used to store the ID of MQTT messages sent or received (e.g., for debugging and tracking).
	received_data 	: Pointer to store the payload of the received MQTT message.
	TAG 			: A logging tag used for identifying messages from this module in the ESP-IDF logging system.
	client 			: A global MQTT client handle that is shared across the program. The `extern` keyword means this variable is defined elsewhere (likely in the main application).

# MQTT Event Handler

	void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {

	This is the callback function that gets triggered whenever an MQTT event occurs (e.g., connection established, data received, or a message published).

#	Switch Case for Event Handling

	switch((esp_mqtt_event_id_t)event_id) {

	The `event_id` determines the type of MQTT event. Let's review each case:

	1. MQTT_EVENT_BEFORE_CONNECT
	- Logs that the MQTT client is initializing a connection to the broker.
	
	ESP_LOGI(TAG, "The client is initialized and about to start connecting to the broker...");
	
	
	2. MQTT_EVENT_CONNECTED
	- Triggered when the MQTT client successfully connects to the broker.
	- Subscribes to the `"sensor/status"` topic.
	
	ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
	msg_id = esp_mqtt_client_subscribe(client, "sensor/status", 1);
	
	
	3. MQTT_EVENT_DISCONNECTED
	- Logs a warning when the MQTT client disconnects from the broker.
	
	ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
	
	
	4. MQTT_EVENT_SUBSCRIBED
	- Logs the success of a subscription request and displays the `msg_id`.
	
	ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
	
	
	5. MQTT_EVENT_UNSUBSCRIBED
	- Logs when the client successfully unsubscribes from a topic.
	
	ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
	
	
	6. MQTT_EVENT_PUBLISHED
	- Logs when the client successfully publishes a message.
	
	ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
	
	
	7. MQTT_EVENT_DATA
	- Triggered when the client receives data from the broker.
	- Extracts the topic and data payload and logs them.
	
	ESP_LOGI(TAG, "MQTT_EVENT_DATA");
	printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);  // Log topic
	received_data = event->data;  // Store received data payload
	printf("DATA=%.*s\r\n", event->data_len, received_data);  // Log data
	
	
	8. MQTT_EVENT_ERROR
	- Triggered when there is an error in the MQTT client.
	- If the error is related to the transport layer, it logs additional details.
	
	ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
	if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
		esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
		esp_log_level_set("transport", ESP_LOG_VERBOSE);
		ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
	}
	
	
	9. Other Events
	- Logs any unhandled or unexpected MQTT events.
	
	ESP_LOGI(TAG, "Unhandled MQTT Event ID: %d", event_id);
	
# MQTT Initialization

	void MQTT_CLIENT_INIT() {
		ESP_LOGI(TAG, "Initialization of Client...");
	
	- This function initializes and starts the MQTT client.

# Configuration
	
	esp_mqtt_client_config_t client_config = {
		.broker = {
			.address.hostname = "test.mosquitto.org",
			.address.port = 1883,
			.address.transport = MQTT_TRANSPORT_OVER_TCP
		},
		.credentials = {
			.username = NULL,
			.authentication.password = NULL
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
	
	- Configures the broker's hostname (`test.mosquitto.org`) and port (`1883` for unencrypted MQTT).
	- Sets up buffer sizes, keepalive intervals, and task priority for the MQTT client.
	
# Registering the Event Handler

	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);

	- Registers the `mqtt_event_handler` to process MQTT events.
	
#  Start the MQTT Client 

	esp_mqtt_client_start(client);

	- Starts the MQTT client and establishes a connection to the broker.
	
	
# Flow of the Program
	
	1. Initialization:
	- The `MQTT_CLIENT_INIT()` function sets up the client, registers the event handler, and starts the client.
	
	2.  Connecting to the Broker :
	- The client connects to the broker (`MQTT_EVENT_CONNECTED`) and subscribes to the `"sensor/status"` topic.
	
	3.  Receiving Data :
	- When a message is received on a subscribed topic (`MQTT_EVENT_DATA`), the event handler extracts the topic and payload, stores the data in `received_data`, and logs it.
	
	4.  Publishing Messages :
	- The application can publish data using `esp_mqtt_client_publish()`.
	
	5.  Error Handling :
	- Any errors are logged, and details are provided for debugging.
