/*
 * max11612Jonas.h
 *
 *  Created on: 14/04/2017
 *      Author: jonas
 */

#ifndef max1161Jonas_H_
#define max1161Jonas_H_
#include "sdkconfig.h"
#include"driver/i2c.h"
#include </home/jonas/esp32/esp-idf/components/log/include/esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#define SDA_IOpin	21
//#define SCL_IOpin	22
#define SDA_IOpin	19
#define SCL_IOpin	23
#define max11612_ADDRESS 0x34 //SDO pin held high
//#define max11612_ADDRESS 0x76  // SDO pin held low
#define funcTimeMiliSecs 5000 // how long this function will wait


typedef enum {
	max11612_ULTRALOWPOWER = 0,
	max11612_STANDARD = 1,
	max11612_HIGHRES = 2,
	max11612_ULTRAHIGHRES = 3
}   foing;

/************************************************/
/**\name	SETUP BYTE enums       */
/***********************************************/
 enum {
	Max11612reset_not = 0x02,
	Max11612reset = 0x00,
	Max11612bipolar = 0x04,
	Max11612unipolar = 0x00,
	Max11612Clock_Int = 0x00,
	Max11612Clock_Ext = 0x08,
	Max11612sel0= 0x10,
	Max11612sel1= 0x20,
	Max11612sel2 = 0x40,
	Max11612setupByteFormatBit = 0x80,
};

//#define max11612_SLEEP_MODE                    (0x00)

/************************************************/
/************************************************/
/**\name	CONFIGURATION BYTE enums*/
 enum{
	 Max11612differential=0x00,
	 Max11612SingleEnded=0x01,
	 Max11612_cs0=0x02,
	 Max11612_cs1=0x04,
	 Max11612_cs2=0x08,
	 Max11612_cs3=0x10,
	 Max11612_scan0=0x20,
	 Max11612_scan1=0x40,
	 Max11612_configurationByteBit=0x00,
 };
/***********************************************/
/*calibration parameters */

/************************************************/
/************************************************/
/**\name	REGISTER ADDRESS DEFINITION       */
/***********************************************/

/************************************************/
/************************************************/
/**\name	POWER MODE DEFINITION       */
/***********************************************/
/* Sensor Specific constants */

/************************************************/
/**\name	STANDBY TIME DEFINITION       */
/***********************************************/

/************************************************/
/**\name	OVERSAMPLING DEFINITION       */
/***********************************************/

/************************************************/
/**\name	WORKING MODE DEFINITION       */
/***********************************************/
//#define max11612_ULTRA_LOW_POWER_MODE          (0x00)

/**\name	FILTER DEFINITION       */
/***********************************************/

/************************************************/
/**\name	DELAY TIME DEFINITION       */
/***********************************************/

/************************************************/




typedef struct {
	int ChannelA;
	int channelB;
} structChannel_AB;

structChannel_AB structTempPressUncomp1;

void init_max11612Jonas(void);
uint8_t send_command2register(uint8_t, uint8_t);
void send_byte2config_setup(uint8_t, uint8_t);
uint8_t readRegister8(uint8_t);
short readRegister16(uint8_t);
int readMax11612(uint8_t );
structChannel_AB readRegister24with20bits(uint8_t);

float x(int);
void task_max11612Jonas(void *);

#endif /* max1161Jonas_H_ */
