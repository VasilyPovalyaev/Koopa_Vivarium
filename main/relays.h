#ifndef RELAYS_H_
#define RELAYS_H_

#define LIGHT_PIN 18
#define HEATER_PIN 19

void set_lights(int);
void set_heater(int);
int get_heater_state(void);
int get_light_state(void);

void setup_relays(void);

#endif