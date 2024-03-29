/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "relays.h"
#include "mqtt.h"
#include "DHT22.h"
#include "sntp_time.h"

static const char *TAG = "MQTT_EXAMPLE";

bool _mqtt_online = false;

esp_mqtt_client_handle_t client;

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

bool mqtt_online(void){return _mqtt_online;}

void mqtt_handler(esp_mqtt_event_handle_t event)
{
    ESP_LOGW(TAG, "HANDLER: Topic = %.*s\r\n", event->topic_len, event->topic);
    ESP_LOGW(TAG, "HANDLER: Data = %.*s\r\n", event->data_len, event->data);

    char * key = strtok(event->data, " ");
    char * val = strtok(NULL, " ");
    
    if (!strcmp(key, "set_lights"))
    {
        set_lights(atoi(val));
    }
    else if (!strcmp(key, "set_heater"))
    {
        set_heater(atoi(val));
    }
    else if (!strcmp(key, "set_refresh"))
    {
        publish_data("temp0", getTemperature(0));
        publish_data("temp1", getTemperature(1));
        publish_data("humidity0", getHumidity(0));
        publish_data("humidity0", getHumidity(0));
        publish_data("lights", get_light_state());
        publish_data("heater", get_heater_state());
    }
    else if (!strcmp(key, "set_time"))
    {
        char *time = sntp_time_get_time();
        esp_mqtt_client_publish(client, MQTT_TOPIC, time, strlen(time),0,0);
    }
}

void publish_data(char* key, float value)
{
    char payload[50];
    strcpy(payload, "get_");
    memcpy(payload+4, key, strlen(key)+1);

    char val[12];
    snprintf(val, 12, " %.2f", value);
    
    strcat(payload, val);
    
    esp_mqtt_client_publish(client, MQTT_TOPIC, payload, strlen(payload), 0, 0);
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        _mqtt_online = true;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        _mqtt_online = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        break;
    case MQTT_EVENT_PUBLISHED:
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        mqtt_handler(event);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}