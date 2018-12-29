/*
 * si7021.h
 *
 *  Created on: 15.08.2018 г.
 *      Author: imihalev
 */

#ifndef MAIN_SI7021_H_
#define MAIN_SI7021_H_

// #include "esp_system.h"
// #include "esp_err.h"
// #include "driver/i2c.h"

#include "user_interface.h"


#define I2C_MASTER_WRITE 0
#define I2C_MASTER_READ  1

typedef struct {
	uint8_t RES1:1;
	uint8_t VDSS:1;
	uint8_t RSVD0:3;
	uint8_t HTRE:1;
	uint8_t RSVD1:1;
	uint8_t RES0:1;
}user_register_t;

#define SI7021_ADDR   0x40

#define	MRHHMM  0xE5   // Measure Relative Humidity, Hold Master Mode
#define MRHNHMM 0xF5   // Measure Relative Humidity, No Hold Master Mode
#define MTHMM   0xE3   // Measure Temperature, Hold Master Mode
#define MTNHMM  0xF3   // Measure Temperature, No Hold Master Mode
#define RTVPRH  0xE0   // Read Temperature Value from Previous RH Measurement
#define RESET   0xFE   // Reset
#define WUR1 0xE6   // Write RH/T User Register 1
#define RUR1 0xE7   // Read RH/T User Register 1
#define WCR    0x51   // Write Heater Control Register
#define RCR    0x11   // Read Heater Control Register
#define REID1_1 0xFA   // Read Electronic ID 1st Byte_1
#define REID1_2 0x0F   // Read Electronic ID 1st Byte_2
#define REID2_1 0xFC   // Read Electronic ID 2nd Byte_1
#define REID2_2 0xC9   // Read Electronic ID 2nd Byte_2
#define RFR_1   0x84   // Read Firmware Revision_1
#define RFR_2   0xB8   // Read Firmware Revision_2



bool readTemperature(char *themperature);
bool readHumidity(double *humidity);
//void readUserRegister(user_register_t* data);
//void writeUserRegister(user_register_t* data);


#endif /* MAIN_SI7021_H_ */