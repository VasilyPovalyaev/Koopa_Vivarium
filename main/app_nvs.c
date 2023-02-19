#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "app_nvs.h"
#include "wifi_app.h"

static const char *TAG = "nvs";

const char *app_nvs_sta_creds_namespace = "stacreds";

/**
 * Saves STA mode wifi creds to nvs
 * @return ESP_OK
*/
esp_err_t app_nvs_save_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Saving STA creds to flash");

    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

    if (wifi_sta_config)
    {
        esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
        if(esp_err != ESP_OK)
        {
            ESP_LOGE(TAG, "app_nvs_save_sta_creds: Error opening NVS handle - %s", esp_err_to_name(esp_err));
            return esp_err;
        }
        //set SSID
        esp_err = nvs_set_blob(handle, "ssid", wifi_sta_config->sta.ssid, MAX_SSID_LENGTH);
        if(esp_err != ESP_OK)
        {
            ESP_LOGE(TAG, "app_nvs_save_sta_creds: Error setting the SSID - %s", esp_err_to_name(esp_err));
            return esp_err;
        }
        //set password
        esp_err = nvs_set_blob(handle, "password", wifi_sta_config->sta.password, MAX_PASSWORD_LENGTH);
        if(esp_err != ESP_OK)
        {
            ESP_LOGE(TAG, "app_nvs_save_sta_creds: Error setting the password - %s", esp_err_to_name(esp_err));
            return esp_err;
        }

        //commit to nvs
        esp_err = nvs_commit(handle);
        if(esp_err != ESP_OK)
        {
            ESP_LOGE(TAG, "app_nvs_save_sta_creds: Error commiting credentials to nvs - %s", esp_err_to_name(esp_err));
            return esp_err;
        }
        nvs_close(handle);
        ESP_LOGI(TAG, "app_nvs_save_sta_creds: Wrote wifi_sta_config: Station SSID: %s Password: %s", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
    }   

    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Returned ESP_OK");
    return ESP_OK;

}

/**
 * loads the previousely set creds from nvs
 * @return true if creds found
*/

bool app_nvs_load_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;

    ESP_LOGI(TAG, "app_nvs_load_sta_creds: Loading WiFi creds form flash");

    if(nvs_open(app_nvs_sta_creds_namespace, NVS_READONLY, &handle) == ESP_OK)
    {
        wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

        if(wifi_sta_config == NULL)
        {
            wifi_sta_config = (wifi_config_t*)malloc(sizeof(wifi_config_t));
        }
        memset(wifi_sta_config, 0x00, sizeof(wifi_config_t));

        //Allocate a buffer
        size_t wifi_config_size = sizeof(wifi_config_t);
        uint8_t *wifi_config_buffer = (uint8_t*)malloc(sizeof(uint8_t)*wifi_config_size);
        memset(wifi_config_buffer, 0x00, sizeof(wifi_config_size));

        //load SSID
        wifi_config_size = sizeof(wifi_sta_config->sta.ssid);
        esp_err = nvs_get_blob(handle, "ssid", wifi_config_buffer, &wifi_config_size);

        if(esp_err != ESP_OK)
        {
            free(wifi_config_buffer);
            ESP_LOGE(TAG, "app_nvs_load_sta_creds: No station SSID found in NVS - %s", esp_err_to_name(esp_err));
            return false;
        }
        memcpy(wifi_sta_config->sta.ssid, wifi_config_buffer, wifi_config_size);

        //load Password
        wifi_config_size = sizeof(wifi_sta_config->sta.password);
        esp_err = nvs_get_blob(handle, "password", wifi_config_buffer, &wifi_config_size);

        if(esp_err != ESP_OK)
        {
            free(wifi_config_buffer);
            ESP_LOGE(TAG, "app_nvs_load_sta_creds: No station Password found in NVS - %s", esp_err_to_name(esp_err));
            return false;
        }
        memcpy(wifi_sta_config->sta.password, wifi_config_buffer, wifi_config_size);

        free(wifi_config_buffer);
        nvs_close(handle);
        ESP_LOGI(TAG, "app_nvs_load_sta_creds: Found SSID: %s and Password: %s", wifi_sta_config->sta.ssid, wifi_sta_config->sta.password);
        return wifi_sta_config->sta.ssid[0] != '\0';
    }
    else
    {
        return false;
    }
}

/**
 * Clears station mode creds from nvs
 * @return ESP_OK
*/

esp_err_t app_nvs_clear_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;
    ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Clearing the WiFi staion mode credentials form flash");
    esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);

    if(esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "app_nvs_clear_sta_creds: Error opening NVS handle - %s", esp_err_to_name(esp_err));
        return esp_err;
    }

    //erase creds
    esp_err = nvs_erase_all(handle);
    if(esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "app_nvs_clear_sta_creds: Error erasing station mode credintials - %s", esp_err_to_name(esp_err));
        return esp_err;
    }

    //commit to nvs
    esp_err = nvs_commit(handle);
    if(esp_err != ESP_OK)
    {
        ESP_LOGE(TAG, "app_nvs_clear_sta_creds: Error NVS commit - %s", esp_err_to_name(esp_err));
        return esp_err;
    }
    nvs_close(handle);
    ESP_LOGI(TAG, "app_nvs_clear_sta_creds: Returned ESP_OK");
    return ESP_OK;
}
