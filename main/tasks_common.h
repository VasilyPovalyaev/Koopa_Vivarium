#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

//WIFI Application Tasks
#define WIFI_APP_TASK_STACK_SIZE            4096
#define WIFI_APP_TASK_PRIORITY              5
#define WIFI_APP_TASK_CORE_ID               0

//HTTP Server Tasks
#define HTTP_SERVER_TASK_STACK_SIZE         8192
#define HTTP_SERVER_TASK_PRIORITY           4
#define HTTP_SERVER_TASK_CORE_ID            0
#define HTTP_SERVER_TASK_MAX_URI            25
#define HTTP_SERVER_TASK_TIMEOUT            10

//HTTP Server Monitor Task
#define HTTP_SERVER_MONITOR_STACK_SIZE      4096
#define HTTP_SERVER_MONITOR_PRIORITY        3
#define HTTP_SERVER_MONITOR_CORE_ID         0

//DHT22 Sensor task
#define DHT22_TASK_STACK_SIZE               4096
#define DHT22_TASK_PRIORITY                 5
#define DHT22_TASK_CORE_ID                  1

//WiFi Reset Button task
#define WIFI_RESET_BUTTON_TASK_STACK_SIZE   2048
#define WIFI_RESET_BUTTON_TASK_PRIORITY     6
#define WIFI_RESET_BUTTON_TASK_CORE_ID      0

//SNTP Time
#define SNTP_TIME_TASK_STACK_SIZE           4096
#define SNTP_TIME_TASK_PRIOTIY              4
#define SNTP_TIME_TASK_CORE_ID              1

#endif