/*

 * nextion.h
 *
 *  Created on: 22.12.2018 �.
 *      Author: imihalev
 */

#ifndef INCLUDE_DRIVER_NEXTION_H_
#define INCLUDE_DRIVER_NEXTION_H_

#include "user_interface.h"
#include "driver/softuart.h"

typedef struct 
{
  uint8_t pageNumber;
  uint8_t displayOk:1;
  uint8_t error:1;
  char*   command;
  Softuart* softuart;

} nextionDisplay_t;



#define NEXTION_INVALID_INSTRUCTION 0x00
#define NEXTION_SUCCESSUUL_EXECUTION 0x01
#define NEXTION_NAME_INVALID 0x1A
#define THERMO_BLUE 21
#define THERMO_VIOLET 22
#define THERMO_ORANGE 23
#define THERMO_RED 24


#define RED 63488
#define ORANGE 64520
#define VIOLET 51603
#define BLUE 31
#define BLACK 0

#define MAINPAGE 1

void PageNextionDisplay();
void PageNextion(uint8_t page);
void nextion_display_init();
void Check_For_Nextion_Data();
void display_themperature(char* themperature);
void display_time();
void display_date();
void display_wifi_rssi();
void display_day();
void nextion_power_off();
void nextion_power_on();
void display_humidity();



#endif