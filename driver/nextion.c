/*

 * nextion.c
 *
 *  Created on: 22.12.2018 �.
 *      Author: imihalev
 */

#include "driver/nextion.h"
#include "user_interface.h"
#include "osapi.h"
#include "driver/softuart.h"
#include "user_config.h"

Softuart softuart;


void ICACHE_FLASH_ATTR
 soft_serial_init(void)
 {
   //GPIO2 RX GPIO0 TX baud 9600
    Softuart_SetPinRx(&softuart,2);
	Softuart_SetPinTx(&softuart,15);
	Softuart_Init(&softuart,9600);
 }





void ICACHE_FLASH_ATTR
 Check_For_Nextion_Data(void)
{
	 if(Softuart_Available(&softuart))
      {
		uint8_t data=Softuart_Read(&softuart);
		ets_uart_printf("Softuart 1: %c 0x%x \r\n",data,data);
	}

}



void ICACHE_FLASH_ATTR
 display_themperature(char* temperature)
 {
   volatile char send_toNextion [28];
		 //os_memset(send_toNextion, 0, 28);
		 //send_toNextion=(char*)malloc(28);
		 //ets_sprintf(temperature,"%c%d.%d",s,t1,t2);
		 ets_sprintf(send_toNextion,"ThermoControl.t2.txt=\"%s\"%c%c%c",temperature,0xFF,0xFF,0xFF);
		 Softuart_Puts(&softuart,(const char*)send_toNextion);
		 uint8_t thermo=atoi(temperature);
		 thermo=(thermo-8)*3;
         ets_sprintf(send_toNextion,"ThermoControl.j1.val=%d%c%c%c",thermo,0xFF,0xFF,0xFF);
		 Softuart_Puts(&softuart,(const char*)send_toNextion);
		 //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
		 //os_delay_us(100);
		 Check_For_Nextion_Data();
  

 }


 void ICACHE_FLASH_ATTR
 display_time(void)
 {
 volatile char send_toNextion [60];
	   ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d:%02d:%02d\"%c%c%c",hours,minutes,seconds,0xFF,0xFF,0xFF);
       //ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d:%02d:%02d\"",hours,minutes,seconds);
       //ets_sprintf(send_toNextion,"%s\rThermoControl.t4.txt=\"%02d.%02d.%02d\"%c%c%c",send_toNextion,date,month,year,0xFF,0xFF,0xFF);
		 Softuart_Puts(&softuart,(const char*)send_toNextion);
		// ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
		  //os_delay_us(100);
		 Check_For_Nextion_Data();

 }

 void ICACHE_FLASH_ATTR
 display_date(void)
 {
    char send_toNextion [28];  
    ets_sprintf(send_toNextion,"ThermoControl.t4.txt=\"%02d.%02d.%02d\"%c%c%c",date,month,year,0xFF,0xFF,0xFF);
    Softuart_Puts(&softuart,(const char*)send_toNextion);
	//ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
   //os_delay_us(100);
   Check_For_Nextion_Data();
     
  }

  void ICACHE_FLASH_ATTR
 display_wifi_rssi( void)
 {
   char send_toNextion [28]; 
   sint8 rssi=wifi_station_get_rssi();
   if(rssi==31)
   {
       rssi=0;
   }else
   {
      rssi+=130;
   }
   ets_sprintf(send_toNextion,"ThermoControl.j2.val=%d%c%c%c",rssi,0xFF,0xFF,0xFF);
   Softuart_Puts(&softuart,(const char*)send_toNextion);
   //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
   //os_delay_us(100);
   Check_For_Nextion_Data();

 }