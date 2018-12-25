/*

 * nextion.c
 *
 *  Created on: 22.12.2018 �.
 *      Author: imihalev
 */

#ifndef INCLUDE_DRIVER_NEXTION_H_
#define INCLUDE_DRIVER_NEXTION_H_

#include "user_interface.h"

typedef struct
{
  uint8_t pageNumber;
  uint8_t command;
  uint8_t displayOk:1;
  uint8_t error:1;
  


} nextionDisplay_t;




#define NEXTION_INVALID_INSTRUCTION 0x00
#define NEXTION_SUCCESSUUL_EXECUTION 0x01
#define NEXTION_NAME_INVALID 0x1A


void PageNextionDisplay(nextionDisplay_t* nextion);
void PageNextion(nextionDisplay_t* nextion);
void soft_serial_init(void);
void Check_For_Nextion_Data(nextionDisplay_t* nextion);
void display_themperature(nextionDisplay_t* nextion,char* themperature);
void display_time(nextionDisplay_t* nextion);
void display_date(nextionDisplay_t* nextion);
void display_wifi_rssi(nextionDisplay_t* nextion);



#endif