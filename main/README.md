# OverView

	This project demonstrates the use of an ESP32-based embedded system for collecting sensor data, 
	packaging it into a JSON object, and publishing it to a cloud service through MQTT. 
	The app_main() function is the entry point of the application and is divided into several stages:
	
	1- WiFi Initialization: Connects the device to a wireless network.
	2- MQTT Client Initialization: Configures and connects the MQTT client to a broker.
	3- ADC One-Shot Initialization: Prepares the ADC for one-shot mode to read sensor data.
	4- JSON Object Initialization: Creates and configures JSON objects to store sensor data.
	5- Publishing Data: Continuously reads data, updates the JSON object, and publishes it to the MQTT broker.

# Code Work Flow 

The app_main() starts by performing important initialization and diagnostic tasks in the ESP-IDF project.

1. Diagnostic Logs:
ESP_LOGI(TAG, "[APP] Startup..");
ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

These log messages provide information about:

1- Startup: Logs a message indicating the application is starting.
2- Free Memory: Uses esp_get_free_heap_size() to display the amount of free heap memory available in bytes.
3- ESP-IDF Version: Uses esp_get_idf_version() to log the version of the ESP-IDF framework being used.

Purpose: Helps with debugging and monitoring during initialization. If there’s an issue (e.g., low memory), these logs can identify it early.

2. Logging Levels:
esp_log_level_set("*", ESP_LOG_INFO);
esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
esp_log_level_set("transport", ESP_LOG_VERBOSE);
esp_log_level_set("outbox", ESP_LOG_VERBOSE);

This section of code is performing important initialization and diagnostic tasks in an ESP-IDF project. Let’s break it down:
Purpose of Each Part
1. Diagnostic Logs:

	ESP_LOGI(TAG, "[APP] Startup..");
	ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    These log messages provide information about:
        Startup: Logs a message indicating the application is starting.
        Free Memory: Uses esp_get_free_heap_size() to display the amount of free heap memory available in bytes.
        ESP-IDF Version: Uses esp_get_idf_version() to log the version of the ESP-IDF framework being used.
    Purpose: Helps with debugging and monitoring during initialization. If there’s an issue (e.g., low memory), these logs can identify it early.

2. Logging Levels:

	esp_log_level_set("*", ESP_LOG_INFO);
	esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
	esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
	esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
	esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
	esp_log_level_set("transport", ESP_LOG_VERBOSE);
	esp_log_level_set("outbox", ESP_LOG_VERBOSE);
	
	Purpose: Configures logging verbosity levels for different components.
		"*": Sets the global log level to INFO. Any logs with levels lower than INFO are ignored globally unless explicitly overridden for a component.
		"mqtt_client", "mqtt_example", etc.: Overrides the global log level for these components, setting them to VERBOSE, which provides highly detailed logs.
	Use Case: Useful for debugging specific components. For example:
		mqtt_client: Detailed logs about MQTT client behavior.
		transport: Logs about underlying transport mechanisms (e.g., TCP or TLS).
		esp-tls: Logs for TLS (secure communication) setup.
		outbox: Logs for MQTT's internal message queue.
		
This section of code is performing important initialization and diagnostic tasks in an ESP-IDF project. Let’s break it down:
Purpose of Each Part

1. Diagnostic Logs:

ESP_LOGI(TAG, "[APP] Startup..");
ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    These log messages provide information about:
        Startup: Logs a message indicating the application is starting.
        Free Memory: Uses esp_get_free_heap_size() to display the amount of free heap memory available in bytes.
        ESP-IDF Version: Uses esp_get_idf_version() to log the version of the ESP-IDF framework being used.
    Purpose: Helps with debugging and monitoring during initialization. If there’s an issue (e.g., low memory), these logs can identify it early.

2. Logging Levels:

esp_log_level_set("*", ESP_LOG_INFO);
esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
esp_log_level_set("transport", ESP_LOG_VERBOSE);
esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    Purpose: Configures logging verbosity levels for different components.
        "*": Sets the global log level to INFO. Any logs with levels lower than INFO are ignored globally unless explicitly overridden for a component.
        "mqtt_client", "mqtt_example", etc.: Overrides the global log level for these components, setting them to VERBOSE, which provides highly detailed logs.
    Use Case: Useful for debugging specific components. For example:
        mqtt_client: Detailed logs about MQTT client behavior.
        transport: Logs about underlying transport mechanisms (e.g., TCP or TLS).
        esp-tls: Logs for TLS (secure communication) setup.
        outbox: Logs for MQTT's internal message queue.

3. NVS (Non-Volatile Storage) Initialization:
	
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	ESP_ERROR_CHECK(nvs_flash_erase());
	ret = nvs_flash_init();
	}

	What is NVS?

    Non-Volatile Storage (NVS) is a key-value storage system provided by ESP-IDF.
	It stores persistent data like Wi-Fi credentials or other configuration settings.
	
	
	Initialization Process:

    nvs_flash_init() initializes the NVS storage.
    
	If the NVS partition is full or a new version of NVS is found, it:
        Erases the NVS partition: nvs_flash_erase().
        Re-initializes NVS: nvs_flash_init().

	Purpose: Ensures that the device has a working NVS storage for the application to use. 
	If the partition is corrupted or outdated, this code resolves it.
	
# WiFi Initialization

	status = connect_wifi();
	The connect_wifi() function:

    Attempts to connect to a pre-configured Access Point (AP).
    Logs success or failure. If the connection fails, the program exits with a failure message.

# MQTT Initialization

	MQTT_CLIENT_INIT();
	
	This function:

    Connects the client to a specified broker (test.mosquitto.org in this case).
    Subscribes to the topic "sensor/status".
    Prepares the client to publish and receive messages.

# ADC Oneshot Initialization

	adc_oneshot_init();
	
	This function:

    Configures ADC channels for reading analog sensor data.
    Allows precise single-sample reads when needed.

# JSON Object Initialization
	
	The application creates a JSON object to hold sensor data:
	
	cJSON *msg = cJSON_CreateObject();
	cJSON_AddNumberToObject(msg, "Temperature", 0);
	cJSON_AddNumberToObject(msg, "Humidity", 0);
	cJSON_AddNumberToObject(msg, "Battery_Health", 0);
	cJSON_AddNumberToObject(msg, "Signal_Strength", 0);
	
	These fields are initialized to 0 and dynamically updated in the main loop.
	
# Data Publishing

	The main loop continuously performs the following tasks:

    Reads data from ADC channels using:
		adc_oneshot_read_value(ADC_CHANNEL_X);

	Updates the JSON object:
		cJSON_ReplaceItemInObject(msg, "Temperature", cJSON_CreateNumber(new_temperature));
	
	Converts the JSON object into a string:
		char *json_string = cJSON_PrintUnformatted(msg);
	
	Publishes the JSON data to the MQTT broker:
		esp_mqtt_client_publish(client, "sensor/status", json_string, 0, 1, 0);
	
	Logs the success or failure of the publish operation.
	
# Project Features

1-  Real-Time Data Collection:

    Reads temperature and humidity data from ADC channels.
    Simulates additional metrics like battery health and signal strength.

2-	MQTT Integration:

    Publishes sensor data to an MQTT broker in JSON format.
    Allows integration with backend services for real-time monitoring.

3-	Flexible Logging:

    Verbose logs for debugging.
    Configurable levels to reduce log clutter in production.

4-	JSON Support:

    Uses the cJSON library to manage and send structured data.
	
# Example WorkFlow

1-	The device starts and logs its memory and firmware version.
2-	Connects to WiFi and logs the connection status.
3-	Initializes the MQTT client and connects to the broker.
4-	Reads ADC values and updates the JSON object:
	{
	"Temperature": 27,
	"Humidity": 45,
	"Battery_Health": 85,
	"Signal_Strength": 72
	}
5-	Publishes data to MQTT broker every 500 ms

# How to Run
	
	Configure WiFi Credentials: Update WiFi_SSID and WiFi_PASS in the WiFi initialization component.

	Build and Flash: Use the ESP-IDF build system:
	
	idf.py build
	idf.py flash monitor
	
	Monitor Logs: View the logs for debugging and status updates.
	
# Dependencies
	
	ESP-IDF Framework
	cJSON Library
	FreeRTOS (for task management)
	MQTT Client Library