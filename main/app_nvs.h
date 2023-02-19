#ifndef APP_NVS_H_
#define APP_NVS_H_

/**
 * Saves STA mode wifi creds to nvs
 * @return ESP_OK
*/
esp_err_t app_nvs_save_sta_creds(void);

/**
 * loads the previousely set creds from nvs
 * @return true if creds found
*/

bool app_nvs_load_sta_creds(void);

/**
 * Clears station mode creds from nvs
 * @return ESP_OK
*/

esp_err_t app_nvs_clear_sta_creds(void);

#endif