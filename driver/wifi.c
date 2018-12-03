/*
 * wifi.c
 *
 *  Created on: 6.01.2017 ã.
 *      Author: admin
 */


#include "user_interface.h"
#include "user_config.h"
#include "driver/wifi.h"
#include "osapi.h"
//#include "stdio.h"
//#include "console.h"
//static struct ip_info ipinfo;
struct station_config stationConfig;
//char ssid[33]=;
char password[33];
char macaddress[17];
char info[150];

void ICACHE_FLASH_ATTR
WifiServer() {
	//wifi_mode

//

//	04.04.2018  if(wifi_get_opmode() != STATIONAP_MODE){
//	 wifi_set_opmode(STATIONAP_MODE);
//	 //after esp_iot_sdk_v0.9.2, need not to restart
//	 //system_restart();
//    }

	//char *ssid[33];
	//char password[33];
	//Check if server connect  if connect disconect it and set new parameter and run .

	ets_uart_printf("ESP8266 SOFTAP mode...\r\n");
	//char macaddress[17];
	//char info[150];
	struct softap_config apConfig;
	struct ip_info ipinfo;
	struct dhcps_lease dhcpslease;

	//On the first read config to sure not to skip same think

	wifi_softap_set_config_current(&apConfig);

	if (wifi_softap_dhcps_stop()) {
		ets_uart_printf("Dhcp stop :) ...\n\r");
	} else {
		ets_uart_printf("Dhcp won't stop :( ...\n\r");
	}

	//First
	IP_ADDR(&ipinfo.ip, 10, 10, 10, 1);
	IP_ADDR(&ipinfo.gw, 10, 10, 10, 1);
	IP_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
	//ets_uart_printf("\n\r SetIP info ip:%X",ipinfo.ip.addr);
	//ets_uart_printf("\n\r SetIP info gw:%X",ipinfo.gw.addr);
	//ets_uart_printf("\n\r SetIP info netmask:%X",ipinfo.netmask.addr);
	wifi_set_ip_info(SOFTAP_IF, &ipinfo);
	//Second
	dhcpslease.enable = true;
	IP_ADDR(&dhcpslease.start_ip, 10, 10, 10, 10);
	IP_ADDR(&dhcpslease.end_ip, 10, 10, 10, 20);
	if (wifi_softap_set_dhcps_lease(&dhcpslease)) {
		ets_uart_printf("Dhcps lease :) ...\n\r");
	} else {
		ets_uart_printf("Dhcps won't lease :( ...\n\r");
	}
	if (wifi_softap_dhcps_start()) {
		ets_uart_printf("Dhcp start :) ...\n\r");
	} else {
		ets_uart_printf("Dhcp won't start :( ...\n\r");
	}

	//Thirth Fill with zero
	os_memset(apConfig.ssid, 0, 32);
	os_memset(apConfig.password, 0, 64);
	switch (lan_type) {
	case 0:
		apConfig.authmode = AUTH_OPEN;
		break;
	case 1:
		apConfig.authmode = AUTH_WEP;
		break;
	case 2:
		apConfig.authmode = AUTH_WPA_PSK;
		break;
	case 3:
		apConfig.authmode = AUTH_WPA2_PSK;
		break;
	case 4:
		apConfig.authmode = AUTH_WPA_WPA2_PSK;
		break;
	case 5:
		apConfig.authmode = AUTH_MAX;
		break;
	default:
		apConfig.authmode = AUTH_OPEN;
	}

	apConfig.max_connection = 4;
	apConfig.ssid_hidden = 0;
	apConfig.beacon_interval = 100;
	//Init SSID
	u8 len = strlen((char*) lan_ssid);
	if (len == 0) {
		ets_sprintf(lan_ssid, "%s", LAN_WIFI);
	}
	ets_uart_printf("lan_ssid:%s\n\r", lan_ssid);
	apConfig.ssid_len = strlen((char*) lan_ssid);
	//os_memcpy(apConfig.ssid, lan_ssid, os_strlen(lan_ssid));
	ets_sprintf(apConfig.ssid, "%s", lan_ssid);

	//Init PWR
	ets_uart_printf("lan_pwd:%s\n\r", lan_pwd);
	len = strlen((char*) lan_pwd);
	//ets_uart_printf("lan_pwd_len:%x\n\r",len);
	if (len == 0) {
		ets_sprintf(lan_pwd, "%s", "");
	}

	ets_sprintf(apConfig.password, "%s", lan_pwd);

	//os_memcpy(apConfig.password, lan_pwd, len);

	ets_uart_printf("ApConfig password:<%s>\n\r", apConfig.password);
	ets_uart_printf(
			"Init Wifi Server.... SSID:<%s>\n\r PASSWORD:<%s>\n\r WIFI_TYPE:<%d> \n\r",
			apConfig.ssid, apConfig.password, apConfig.authmode);

	if (wifi_softap_set_config_current(&apConfig)) {
		ets_uart_printf("ApConfig succesufull :) ....\n");
	} else {
		ets_uart_printf("ApConfig fail :( ... \n");
	}
	wifi_softap_get_config(&apConfig);
	ets_uart_printf(
			"After Init Wifi Server.... SSID:<%s>\n\r PASSWORD:<%s>\n\r WIFI_TYPE:<%d> \n\r",
			apConfig.ssid, apConfig.password, apConfig.authmode);
	//wifi_set_ip_info(SOFTAP_IF,&ipinfo);
	//wifi_softap_dhcps_start();

     return;

}

void ICACHE_FLASH_ATTR
WifiStationConnect(){
    if(strlen((char *)wan_ssid)==0) return;
    ets_uart_printf("Try to connect \n\r");
    wifiStatus.connect=0;
    wifiStatus.tryconnet=1;
	//strncpy(ssid,SSID,32);
      //clear
    os_memset(stationConfig.ssid, 0, 32);
   	os_memset(stationConfig.password, 0, 64);
   	 //set
    ets_sprintf(stationConfig.ssid,"%s",wan_ssid);
    ets_sprintf(stationConfig.password,"%s",wan_pwd);
    stationConfig.bssid_set=0;
	//os_memcpy(stationConfig.ssid ,wan_ssid,32);
	//os_memcpy(stationConfig.password,wan_pwd,64);
    wifi_station_set_config(&stationConfig);
    wifi_set_sleep_type(0);
     if(wifi_station_get_connect_status() != STATION_CONNECTING )
     {
    	  //Try Connect;
       ets_uart_printf("Try to connect...\n\r");
       wifi_station_connect();
     }
     return;
 }

void ICACHE_FLASH_ATTR
InitWifi()
{
	ets_uart_printf("Station Mode: <0x%X> \n",wifi_get_opmode());

	if(program.wifiLan==0 && program.wifiWan==0){
		wifi_set_opmode(NULL_MODE);
		ets_uart_printf("NULL_MODE  \n");
		return;
	} else if (program.wifiLan == 0 && program.wifiWan==1) {
		if (wifi_get_opmode() != STATION_MODE) {
			wifi_set_opmode(STATION_MODE);
			ets_uart_printf("STATION_MODE \n");
			WifiStationConnect();
			//after esp_iot_sdk_v0.9.2, need not to restart
			//system_restart();
		}
	}else if(program.wifiLan == 1 && program.wifiWan==0){
		if(program.wifiServer==0){
			    ets_uart_printf("SOFTAP_MODE_DHCP_SERVER \n");
			  	WifiServer();
			   	program.wifiServer=1;
			  }
		if (wifi_get_opmode() != SOFTAP_MODE) {
			wifi_set_opmode(SOFTAP_MODE);
			ets_uart_printf("SOFTAP_MODE \n");
		}

	}else if (program.wifiLan == 1 && program.wifiWan==1){
		if(program.wifiServer==0){
			ets_uart_printf("SOFTAP_MODE_DHCP_SERVER \n");
			WifiServer();
			program.wifiServer=1;
		}
		if(!wifiStatus.connect){
			WifiStationConnect();
		}
		if (wifi_get_opmode() != STATIONAP_MODE) {
			wifi_set_opmode(STATIONAP_MODE);
			ets_uart_printf("STATIONAP_MODE \n");
			WifiStationConnect();
			WifiServer();
			//after esp_iot_sdk_v0.9.2, need not to restart
			//system_restart();
		}
	}
}

 void ICACHE_FLASH_ATTR
eventHandler(System_Event_t *event)
{
	 //  ets_uart_printf("\n\r Wifi Event: %01x ",event->event);

	  switch(event->event)
	  {
	  case EVENT_STAMODE_CONNECTED:
		 //wifiStatus.connect=1;
	     //lcd_line=0;
	     ets_uart_printf("Wifi OK \n\r ");
	     break;
	  case EVENT_STAMODE_DISCONNECTED:
		   wifiStatus.connect=0;
		   wifiStatus.tryconnet=0;
		   ipadrchar[0]=0;
		   //wifi_station_connect();
		   wifi_station_disconnect();
		   ets_uart_printf("Wifi Down \n\r ");
		   status.gotIP=0;
		   status.mqtt=0;
		   status.mqttOK=0;
	       break;
	  case EVENT_STAMODE_GOT_IP:
		   wifiStatus.connect=1;
		   struct ip_info ipconfig;
		   status.gotIP=1;
		   wifi_get_ip_info(STATION_IF,&ipconfig);
		   // char result[16];
		   ipadr =ipconfig.ip.addr;
		   ipadrchar[20];
		   os_sprintf(ipadrchar,"%d.%d.%d.%d",ipadr&0xff,(ipadr>>8)&0xff,(ipadr>>16)&0xff,(ipadr>>24)&0xff);
		   //ets_uart_printf("\n\r New connection I have got IP: %s",ipadrchar);
	       break;
	  case EVENT_SOFTAPMODE_STACONNECTED:
		  ets_uart_printf("SOFTAPMODE_CONNECTED \n\r");
		  //wifi_get_ip_info(SOFTAP_IF,&ipconfig);
		  //ipadr =ipconfig.ip.addr;
		  //char ipadrchar_ap[20];
		  //os_sprintf(ipadrchar_ap,"%d.%d.%d.%d",ipadr&0xff,(ipadr>>8)&0xff,(ipadr>>16)&0xff,(ipadr>>24)&0xff);
		  //ets_uart_printf("New station add got IP: %s \n\r",ipadrchar_ap);
		  wifiStatus.lan=1;
		  break;
	  case EVENT_SOFTAPMODE_STADISCONNECTED:
		  ets_uart_printf("SOFTAPMODE_DISCONNECTED \n\r");
		  wifiStatus.lan=0;
		  break;
	  case EVENT_SOFTAPMODE_PROBEREQRECVED:
		  ets_uart_printf("SOFTAPMODE_PROBEREQRECVED \n\r");
	      break;
	  default:
	  wifiStatus.connect=0;
	  wifiStatus.tryconnet=0;
	  ets_uart_printf("Unknow status \n\r");
	  }
}

 void  ICACHE_FLASH_ATTR scanCB(void *arg, STATUS status)
{

	struct bss_info *bssInfo;
	bssInfo=(struct bss_info *)arg;
	//bssInfo=STAILQ_NEXT(bssInfo,next);
	char* ssid;
	scan_wifi[0]=0;
	strcat((char*)scan_wifi,"scanwifi=");
	while (bssInfo !=NULL)
	 {
	    ssid=bssInfo->ssid;
	    strcat((char*)scan_wifi,"&");
	    strcat((char*)scan_wifi,ssid);

	  //("nssid: %s\n\r",ssid);
	    bssInfo=STAILQ_NEXT(bssInfo,next);
	 }
	 ets_uart_printf("Scaned Wifi: <%s> \n\r Len: <%d> \n\r",scan_wifi,strlen((char*)scan_wifi));
     wifiStatus.scan=0;
     wifiStatus.scaned=1;

}

void ICACHE_FLASH_ATTR WifiStationScan()
{
	if (wifiStatus.scan) return;
	switch(wifi_get_opmode()){
	case NULL_MODE:
	  wifi_set_opmode(STATION_MODE);
	 break;
	case STATION_MODE:
	 break;
	case SOFTAP_MODE:
	  wifi_set_opmode(STATIONAP_MODE);
	 break;
	case STATIONAP_MODE:
	 break;
	}
	wifiStatus.scan=1;
	wifiStatus.scaned=0;
	ets_uart_printf("Scanning Wifi.....\n\r");
	//lcd_line=0;
	//printf("\nScanning");
	wifi_station_scan(NULL, scanCB);
}

void ICACHE_FLASH_ATTR WifiStatus()
{
	 struct ip_info ipconfig;
	 ipadrchar[0]=0;
	 wifi_get_ip_info(STATION_IF,&ipconfig);
	 //char ipadrchar[20];
	 os_sprintf(ipadrchar,"%d.%d.%d.%d",ipadr&0xff,(ipadr>>8)&0xff,(ipadr>>16)&0xff,(ipadr>>24)&0xff);
	 // ets_uart_printf("\n\rI have got IP: %s",ipadrchar);
	 // ets_uart_printf("\n\r Sleep status: 0x%X",wifi_get_sleep_type());
	 //return ipadrchar;
	 //wifi_get_sleep_type(void)

}



void ICACHE_FLASH_ATTR softapStatus()
{
	 struct softap_config apConfig;
	 wifi_softap_get_config(&apConfig);
	  ets_sprintf(lan_ssid,"%s",apConfig.ssid);
	  ets_sprintf(lan_pwd,"%s",apConfig.password);
	  lan_type=apConfig.authmode;
}

