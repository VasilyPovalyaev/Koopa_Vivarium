#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#define TOPIC "/koopa"

bool mqtt_online(void);

void publish_data(char* key, float value);
void mqtt_start(void);

#endif