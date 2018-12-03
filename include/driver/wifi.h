/*
 * wifi.h
 *
 *  Created on: 6.01.2017 ã.
 *      Author: admin
 */

#ifndef INCLUDE_DRIVER_WIFI_H_
#define INCLUDE_DRIVER_WIFI_H_



//#define SSID "Michalev"
//#define PASSWORD "mich@l40"

#define LAN_WIFI "alabala"

//#define SSID "MikroTik"
//#define PASSWORD "primorski"


#define IP_ADDR(ipaddr,a,b,c,d) (ipaddr)->addr=(u32)((u32)d<<24 |(u32)c<<16|(u32)b<<8|(u32)a)


void InitWifi();
void WifiStationScan();
void eventHandler(System_Event_t *event);


char stationWifi[150];
volatile u32 ipadr;
char ipadrchar[20];
struct wifi
{
	 unsigned scan:1;
	 unsigned tryconnet:1;
	 unsigned connect:1;
	 unsigned scaned:1;
	 unsigned ping:1;
	 unsigned count:5;
	 unsigned lan:1;  // Set if client connected to wifi esp8266
	 //unsigned enableLan:1; //Enable or Disable LAN wifi in esp8266

} volatile wifiStatus;

#endif /* INCLUDE_DRIVER_WIFI_H_ */
