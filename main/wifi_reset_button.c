#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

static const char *TAG = "wifi_reset_button";

//Semaphore handle
SemaphoreHandle_t wifi_reset_semaphore = NULL;
/**
 * ISR handler for the wifi reset
 * @param arg
*/
void IRAM_ATTR wifi_reset_button_isr_handler(void *arg)
{
    //Notify the button task
    xSemaphoreGiveFromISR(wifi_reset_semaphore, NULL);
}

/**
 * Wifi reset button task reacts tot a boot button event by sending a message to a wifi app to disconnect from wifi and clear creds
 * @param pvParam 
*/
void wifi_reset_button_task(void *pvParameter)
{
    for (;;)
    {
        if(xSemaphoreTake(wifi_reset_semaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "WIFI RESET BUTTON INTERRUPT");

            //send the message to disconnect wifi
            wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void wifi_reset_button_config(void)
{
    //create the binary semaphore
    wifi_reset_semaphore = xSemaphoreCreateBinary();

    //Configure the button
    gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);

    //Enbale intr on neg
    gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE);

    //create the reset task
    xTaskCreatePinnedToCore(&wifi_reset_button_task, "wifi_reset_button_task", WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_TASK_PRIORITY, NULL, WIFI_RESET_BUTTON_TASK_CORE_ID);

    //install gpio isr  
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    //attach isr
    gpio_isr_handler_add(WIFI_RESET_BUTTON, wifi_reset_button_isr_handler, NULL);
}