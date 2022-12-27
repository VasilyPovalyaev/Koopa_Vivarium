#include "relays.h"
#include "driver/gpio.h"

int light_state = 0;
int heater_state = 0;

void setup_relays(void){
    gpio_set_direction(LIGHT_PIN, GPIO_MODE_OUTPUT_OD);
    gpio_set_direction(HEATER_PIN, GPIO_MODE_OUTPUT_OD);

    set_lights(light_state);
    set_heater(heater_state);
}

void set_lights(int state){
    gpio_set_level(LIGHT_PIN, !state);
    light_state = state;
}
void set_heater(int state){
    gpio_set_level(HEATER_PIN, !state);
    heater_state = state;
}

int get_light_state(void){return light_state;}
int get_heater_state(void){return heater_state;}
