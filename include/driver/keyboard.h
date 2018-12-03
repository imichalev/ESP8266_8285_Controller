/*
 * keyboard.h
 *
 *  Created on: 5.01.2017 ã.
 *      Author: admin
 */

#ifndef INCLUDE_DRIVER_KEYBOARD_H_
#define INCLUDE_DRIVER_KEYBOARD_H_

//Read level from ADC pin
#define LEVEL_KEY_1  271
#define LEVEL_KEY_2  521
#define LEVEL_KEY_3  752
#define LEVEL_KEY_4  978
#define LEVEL_KEY_12 300
#define LEVEL_KEY_13 328
#define LEVEL_KEY_14 362
#define LEVEL_KEY_23 567
#define LEVEL_KEY_24 622
#define LEVEL_KEY_34 822

#define TOLERANSE 20
#define TIME_KEY  300  //100
#define DEFAULT_ZERO_LEVEL 100

//u8  ScanKey(void);
#endif /* INCLUDE_DRIVER_KEYBOARD_H_ */

//#define INFOKEY  ets_uart_printf
#define INFOKEY
void ScanKey();
