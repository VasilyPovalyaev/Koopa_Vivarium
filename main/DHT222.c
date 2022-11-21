#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

#include "DHT22.h"
#include "tasks_common.h"

static const char* TAG = "DHT";

typedef struct DHT22
{
    int pin;
    float temperature;
    float humidity;
    uint8_t data[5];

}DHT22_t;

void get_humidity(DHT22_t* sensor){
    uint16_t h = sensor->data[0] << 8 | sensor->data[1];
    float rh = h/10.;
    sensor->humidity = rh;
}
void get_temperature(DHT22_t* sensor){
    uint16_t t = (sensor->data[2] & 0x7f) << 8;
    t |= sensor->data[3];
    float temp = t/10.;
    if (sensor->data[2] & 0x80){
        sensor->temperature = temp*-1;
    }else{
        sensor->temperature = temp;
    }
}
int checksum(DHT22_t* sensor){
    if (sensor->data[4] == ((sensor->data[0] + sensor->data[1] + sensor->data[2] + sensor->data[2] + sensor->data[3]) & 0xff)){
        return DHT_OK;
    }else{
        return DHT_CHECKSUM_ERROR;
    }
}
int get_signal_level(DHT22_t* sensor, int timeout, bool level){
    int64_t start = esp_timer_get_time();
    int64_t stop = esp_timer_get_time();
    while(gpio_get_level(sensor->pin) == level){
        if(stop-start>timeout){
            return -1;
        }
        int64_t stop = esp_timer_get_time();
    }
    return stop-start;
}
int readDHT(DHT22_t* sensor){
    
    gpio_set_direction(sensor->pin, GPIO_MODE_OUTPUT);
    gpio_set_level(sensor->pin, 0);
    esp_rom_delay_us(1100);
    gpio_set_level(sensor->pin, 1);
    esp_rom_delay_us(25);
    gpio_set_direction(sensor->pin, GPIO_MODE_INPUT);
    
    

    for (int byte = 0; byte < sizeof(sensor->data); byte++){
        sensor->data[byte] = 0x00;
        for(int bit = 7; bit >=0; bit--){

        }
    }


}