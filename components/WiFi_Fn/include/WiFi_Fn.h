#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"

// Defines
#define WIFI_SSID "Ahmed Banna_2.4G"
#define WIFI_PASS "Shary@1281987"
#define MAX_RETRY 5
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define MAX_FAILURES 10

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
                                
void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

esp_err_t connect_wifi();                                                                                            