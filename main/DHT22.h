/* 

	DHT22 temperature sensor driver

*/

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2
#define DHT_READ_ERROR -3

#define DHT0_GPIO			25
#define DHT1_GPIO			26

/**
 * Starts DHT22 sensor task
 */
void DHT22_task0_start(void);
void DHT22_task1_start(void);

// == function prototypes =======================================

void 	setDHTgpio(int gpio0, int gpio1);
void 	errorHandler(int response);
int 	readDHT(int idx);
float 	getHumidity(int idx);
float	getTemperature(int idx);
int 	getSignalLevel( int usTimeOut, bool state , int idx);

#endif
