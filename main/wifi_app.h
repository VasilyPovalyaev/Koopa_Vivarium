#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "esp_wifi_types.h"


//WiFi application settings
#define WIFI_AP_SSID            "Koopa's_Vivarium"
#define WIFI_AP_PASSWORD        "password"
#define WIFI_AP_CHANNEL         1
#define WIFI_AP_SSID_HIDDEN     0
#define WIFI_AP_MAX_CONNECTIONS 5
#define WIFI_AP_BEACON_INTERVAL 100
#define WIFI_AP_IP              "192.168.0.1"
#define WIFI_AP_GATEWAY         "192.168.0.1"
#define WIFI_AP_NETMASK         "255.255.255.0"
#define WIFI_AP_BANDWIDTH       WIFI_BW_HT20
#define WIFI_STA_POWER_SAVE     WIFI_PS_NONE
#define MAX_SSID_LENGTH         32
#define MAX_PASSWORD_LENGTH     64
#define MAX_CONNECTION_RETRIES  5

//netif
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/**
 * Message IDs
 */

typedef enum wifi_app_message
{
    WIFI_APP_MSG_START_HTTP_SERVER = 0,
    WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
    WIFI_APP_MSG_STA_DISCONNECTED,
    WIFI_APP_MSG_LOAD_SAVED_CREDS
} wifi_app_message_e;

/**
 * Structure for the message queues
*/

typedef struct wifi_app_queue_message
{
    wifi_app_message_e msgID;
} wifi_app_queue_message_t;


/**
 * Send message to the queue
 * @param msgID message ID from theh wifi_app_message_e enum
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE.
 * @note expand parameter list based on requiremnets.
 */

BaseType_t wifi_app_send_message(wifi_app_message_e msgID);


/**
 * Starts the WiFi RTOS task
 */
void wifi_app_start(void);

/**
 * gets the wifi configuration
 */

wifi_config_t* wifi_app_get_wifi_config(void);

#endif