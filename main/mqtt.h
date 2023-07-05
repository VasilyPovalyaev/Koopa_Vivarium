#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#define MQTT_BROKER "mqtt://ec2-35-176-90-64.eu-west-2.compute.amazonaws.com"
#define MQTT_TOPIC "/koopa"

bool mqtt_online(void);

void publish_data(char* key, float value);
void mqtt_start(void);

#endif