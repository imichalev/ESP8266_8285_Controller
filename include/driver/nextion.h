/*

 * nextion.c
 *
 *  Created on: 22.12.2018 �.
 *      Author: imihalev
 */

#ifndef INCLUDE_DRIVER_NEXTION_H_
#define INCLUDE_DRIVER_NEXTION_H_

void soft_serial_init(void);
void Check_For_Nextion_Data(void);
void display_themperature(char* themperature);
void display_time(void);
void display_date(void);
void display_wifi_rssi( void);



#endif