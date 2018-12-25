/*
 * ds1307.h
 *
 *  Created on: 3.02.2017 �.
 *      Author: admin
 */

#ifndef INCLUDE_DRIVER_DS1307_H_
#define INCLUDE_DRIVER_DS1307_H_

#define DS1307_ADDRESS 0xD0
#define TEMPERATURE_ADDRESS  0x11

bool ReadDS1307(void);
bool ReadDS1307_Temperature(void);
void DS13007_Temperature(void);
bool SetTimeDate(u8 second,u8 minute,u8 hours,u8 day,u8 date, u8 month,u8 year);
#endif /* INCLUDE_DRIVER_DS1307_H_ */
