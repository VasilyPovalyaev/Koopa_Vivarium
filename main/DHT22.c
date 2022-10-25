/*------------------------------------------------------------------------------

	DHT22 temperature & humidity sensor AM2302 (DHT22) driver for ESP32

	Jun 2017:	Ricardo Timmermann, new for DHT22  	

	Code Based on Adafruit Industries and Sam Johnston and Coffe & Beer. Please help
	to improve this code. 
	
	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.

	PLEASE KEEP THIS CODE IN LESS THAN 0XFF LINES. EACH LINE MAY CONTAIN ONE BUG !!!

---------------------------------------------------------------------------------*/

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "DHT22.h"
#include "tasks_common.h"

// == global defines =============================================

static const char* TAG = "DHT";

int DHTgpio[2];				
float humidity0;
float humidity1;
float temperature0;
float temperature1;

// == set the DHT used pin=========================================

void setDHTgpio( int gpio0, int gpio1 )
{
	DHTgpio[0] = gpio0;
	DHTgpio[1] = gpio1;
	
}

// == get temp & hum =============================================

float getHumidity(int idx)
{
	if (idx == 0)
	{
		return humidity0;
	}
	else
	{
		return humidity1;
	}
}
float getTemperature(int idx)
{
	if (idx == 0)
	{
		return temperature0;
	}
	else
	{
		return temperature1;
	}
}

// == error handler ===============================================

void errorHandler(int response)
{
	switch(response) {

		case DHT_READ_ERROR :
			ESP_LOGE(TAG, "Read error\n");
			break;
	
		case DHT_TIMEOUT_ERROR :
			ESP_LOGE( TAG, "Sensor Timeout\n" );
			break;

		case DHT_CHECKSUM_ERROR:
			ESP_LOGE( TAG, "CheckSum error\n" );
			break;

		case DHT_OK:
			break;

		default :
			ESP_LOGE( TAG, "Unknown error\n" );
	}
}

/*-------------------------------------------------------------------------------
;
;	get next state 
;
;	I don't like this logic. It needs some interrupt blocking / priority
;	to ensure it runs in realtime.
;
;--------------------------------------------------------------------------------*/

int getSignalLevel( int usTimeOut, bool state , int idx)
{

	int uSec = 0;
	while( gpio_get_level(DHTgpio[idx])==state ) {

		if( uSec > usTimeOut ) 
			return -1;
		
		++uSec;
		esp_rom_delay_us(1);		// uSec delay
	}
	
	return uSec;
}

/*----------------------------------------------------------------------------
;
;	read DHT22 sensor

copy/paste from AM2302/DHT22 Docu:

DATA: Hum = 16 bits, Temp = 16 Bits, check-sum = 8 Bits

Example: MCU has received 40 bits data from AM2302 as
0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
16 bits RH data + 16 bits T data + check sum

1) we convert 16 bits RH data from binary system to decimal system, 0000 0010 1000 1100 → 652
Binary system Decimal system: RH=652/10=65.2%RH

2) we convert 16 bits T data from binary system to decimal system, 0000 0001 0101 1111 → 351
Binary system Decimal system: T=351/10=35.1°C

When highest bit of temperature is 1, it means the temperature is below 0 degree Celsius. 
Example: 1000 0000 0110 0101, T= minus 10.1°C: 16 bits T data

3) Check Sum=0000 0010+1000 1100+0000 0001+0101 1111=1110 1110 Check-sum=the last 8 bits of Sum=11101110

Signal & Timings:

The interval of whole process must be beyond 2 seconds.

To request data from DHT:

1) Sent low pulse for > 1~10 ms (MILI SEC)
2) Sent high pulse for > 20~40 us (Micros).
3) When DHT detects the start signal, it will pull low the bus 80us as response signal, 
   then the DHT pulls up 80us for preparation to send data.
4) When DHT is sending data to MCU, every bit's transmission begin with low-voltage-level that last 50us, 
   the following high-voltage-level signal's length decide the bit is "1" or "0".
	0: 26~28 us
	1: 70 us

;----------------------------------------------------------------------------*/

#define MAXdhtData 5	// to complete 40 = 5*8 Bits



int readDHT(int idx)
{
int uSec = 0;

uint8_t dhtData[MAXdhtData];
uint8_t byteInx = 0;
uint8_t bitInx = 7;

if (idx == 0){
	humidity0=0.0;
	temperature0=0.0;
}else{
humidity1=0.0;
temperature1=0.0;
}

	for (int k = 0; k<MAXdhtData; k++) 
		dhtData[k] = 0;

	// == Send start signal to DHT sensor ===========

	gpio_set_direction( DHTgpio[idx], GPIO_MODE_OUTPUT );

	// pull down for 3 ms for a smooth and nice wake up 
	gpio_set_level( DHTgpio[idx], 0 );
	esp_rom_delay_us( 3000 );			

	// pull up for 25 us for a gentile asking for data
	gpio_set_level( DHTgpio[idx], 1 );
	esp_rom_delay_us( 25 );

	gpio_set_direction( DHTgpio[idx], GPIO_MODE_INPUT );		// change to input mode
  
	// == DHT will keep the line low for 80 us and then high for 80us ====

	uSec = getSignalLevel( 85, 0 , idx);
//	ESP_LOGI( TAG, "Response = %d", uSec );
	if( uSec<0 ) return DHT_TIMEOUT_ERROR; 

	// -- 80us up ------------------------

	uSec = getSignalLevel( 85, 1, idx );
//	ESP_LOGI( TAG, "Response = %d", uSec );
	if( uSec<0 ) return DHT_TIMEOUT_ERROR;

	// == No errors, read the 40 data bits ================
  
	for( int k = 0; k < 40; k++ ) {

		// -- starts new data transmission with >50us low signal

		uSec = getSignalLevel( 56, 0, idx);
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		// -- check to see if after >70us rx data is a 0 or a 1

		uSec = getSignalLevel( 75, 1, idx);
		if( uSec<0 ) return DHT_TIMEOUT_ERROR;

		// add the current read to the output data
		// since all dhtData array where set to 0 at the start, 
		// only look for "1" (>28us us)
	
		if (uSec > 40) {
			dhtData[ byteInx ] |= (1 << bitInx);
			}
		
		
	
		// index to next byte

		if (bitInx == 0) { bitInx = 7; ++byteInx; }
		else bitInx--;
	}

	// == get humidity from Data[0] and Data[1] ==========================
	if(idx == 0)
	{
		humidity0 = dhtData[0];
		humidity0 *= 0x100;					// >> 8
		humidity0 += dhtData[1];
		humidity0 /= 10;						// get the decimal

		// == get temp from Data[2] and Data[3]
		
		temperature0 = dhtData[2] & 0x7F;	
		temperature0 *= 0x100;				// >> 8
		temperature0 += dhtData[3];
		temperature0 /= 10;

		if( dhtData[2] & 0x80 ) 			// negative temp, brrr it's freezing
			temperature0 *= -1;
	}
	else
	{
		humidity1 = dhtData[0];
		humidity1 *= 0x100;					// >> 8
		humidity1 += dhtData[1];
		humidity1 /= 10;						// get the decimal

		// == get temp from Data[2] and Data[3]
		
		temperature1 = dhtData[2] & 0x7F;	
		temperature1 *= 0x100;				// >> 8
		temperature1 += dhtData[3];
		temperature1 /= 10;

		if( dhtData[2] & 0x80 ) 			// negative temp, brrr it's freezing
			temperature1 *= -1;
	}
	// == verify if checksum is ok ===========================================
	// Checksum is the sum of Data 8 bits masked out 0xFF
	
	if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF)) 
		return DHT_OK;

	else 
		return DHT_CHECKSUM_ERROR;
}

/**
 * DHT22 Sensor task
 */
static void DHT22_task0(void *pvParameter)
{
	setDHTgpio(DHT0_GPIO, DHT1_GPIO);
	printf("Starting DHT task\n\n");

	for (;;)
	{
		
		printf("=== Reading DHTs ===\n");

		int ret1 = readDHT(0);

		errorHandler(ret1);

		printf("Hum %.1f\t%.1f\n", getHumidity(0), getHumidity(1));
		printf("Tmp %.1f\t%.1f\n", getTemperature(0), getTemperature(1));

		// Wait at least 2 seconds before reading again
		// The interval of the whole process must be more than 2 seconds
		vTaskDelay(4000 / portTICK_PERIOD_MS);
	}
}
static void DHT22_task1(void *pvParameter)
{
	setDHTgpio(DHT0_GPIO, DHT1_GPIO);
	printf("Starting DHT task\n\n");

	for (;;)
	{
		
		printf("=== Reading DHTs ===\n");

		int ret1 = readDHT(1);

		errorHandler(ret1);

		printf("Hum %.1f\t%.1f\n", getHumidity(0), getHumidity(1));
		printf("Tmp %.1f\t%.1f\n", getTemperature(0), getTemperature(1));

		// Wait at least 2 seconds before reading again
		// The interval of the whole process must be more than 2 seconds
		vTaskDelay(4000 / portTICK_PERIOD_MS);
	}
}

void DHT22_task1_start(void)
{
	xTaskCreatePinnedToCore(&DHT22_task0, "DHT22_task0", DHT22_TASK_STACK_SIZE, NULL, DHT22_TASK_PRIORITY, NULL, DHT22_TASK_CORE_ID);
}

void DHT22_task0_start(void)
{
	xTaskCreatePinnedToCore(&DHT22_task1, "DHT22_task1", DHT22_TASK_STACK_SIZE, NULL, DHT22_TASK_PRIORITY, NULL, DHT22_TASK_CORE_ID);
}







































