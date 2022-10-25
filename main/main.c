
#include "nvs_flash.h"

#include "DHT22.h"
#include "wifi_app.h"

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

    //Start DHT22 tsak
    DHT22_task0_start();
    DHT22_task1_start();
    
}
