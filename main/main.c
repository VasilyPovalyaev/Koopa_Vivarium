
#include "nvs_flash.h"
#include "esp_log.h"

#include "DHT22.h"
#include "relays.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"
#include "sntp_time.h"
#include "mqtt.h"

const static char *TAG = "main";

void wifi_application_connected_events(void)
{
    ESP_LOGI(TAG, "WiFi application Connected");
    // sntp_time_task_start();
    sntp_time_obtain_time();
    mqtt_start();
}

void app_main(void)
{
    
    // initalize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Start WiFi
    wifi_app_start();

    //Configure WiFi reset button
    wifi_reset_button_config();

    //Start DHT22 tsak
    DHT22_task0_start();
    DHT22_task1_start();

    //Init relays
    setup_relays();    

    //set wifi cb
    wifi_app_set_callback(&wifi_application_connected_events);
}
