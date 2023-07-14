#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/apps/sntp.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "http_server.h"
#include "sntp_time.h"
#include "wifi_app.h"

static const char *TAG = "sntp_time";

//SNTP op mode status
static bool sntp_op_mode_set = false;

/**
 * Init sntp service
*/
static void sntp_time_init_sntp(void)
{
    ESP_LOGI(TAG, "Initialising the SNTP service");

    if(!sntp_op_mode_set)
    {
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0,"pool.ntp.org");
        sntp_init();
        sntp_op_mode_set = true;
    }

    http_server_monitor_send_message(HTTP_MSG_TIME_SERVICE_INITIALISED);
}

/**
 * gets the current time and if the current time is not up to date the NTPS time sync function is called
*/
void sntp_time_obtain_time(void)
{
    time_t now;
    struct tm time_info;

    time(&now);
    localtime_r(&now, &time_info);


    if(time_info.tm_year < (2020 - 1900))
    {
        setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
        tzset();
        sntp_time_init_sntp();
        //set local timezxone
    }
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &time_info);
    ESP_LOGW(TAG, "The current date/time in London is: %s, %d", strftime_buf, time_info.tm_year);
}

/**
 * SNTP time task
*/
static void sntp_time_task(void* pvParameter)
{
    for(;;)
    {
        sntp_time_obtain_time();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }

    vTaskDelete(NULL);
}

char* sntp_time_get_time(void)
{   
    static char time_buffer[100] = {0};

    time_t now;
    struct tm time_info;

    time(&now);
    localtime_r(&now, &time_info);

    if(time_info.tm_year < (2022 - 1900))
    {
        ESP_LOGI(TAG, "Time not set yet");
        setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
        tzset();
        sntp_time_init_sntp();
    }
    else
    {
        strftime(time_buffer, sizeof(time_buffer), "%d-%m-%Y %H:%M:%S", &time_info);
        // ESP_LOGI(TAG, "Current time: %s", time_buffer);
    }

    return time_buffer;
}

void sntp_time_task_start(void)
{
    xTaskCreatePinnedToCore(&sntp_time_task, "sntp_time_task", SNTP_TIME_TASK_STACK_SIZE, NULL, SNTP_TIME_TASK_PRIOTIY, NULL, SNTP_TIME_TASK_CORE_ID);
}
