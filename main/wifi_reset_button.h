#ifndef WIFI_RESET_BUTTON_H_
#define WIFI_RESET_BUTTON_H_

//Default interrupt flag
#define ESP_INTR_FLAG_DEFAULT   0

//WiFi reset button is the BOOT butotn on the devkit
#define WIFI_RESET_BUTTON   0

/**
 * Configure WiFi reset button
*/
void wifi_reset_button_config(void);

#endif