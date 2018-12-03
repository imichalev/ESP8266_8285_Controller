/*
 *
 *  Created on: 31.12.2016 �.
 *      Author: admin
 */


#include <os_type.h>
#include "osapi.h"
#include "user_interface.h"
//#include "driver/hd44780.h"
#include "driver/keyboard.h"
#include "driver/uart.h"
//#include "driver/oled.h"
#include "driver/ds1307.h"
#include "driver/eeprom.h"
#include "ets_sys.h"
#include "driver/wifi.h"
#include "user_config.h"
#include "driver/gpio16.h"
#include "driver/beep.h"
#include "driver/tcpServer.h"
//#include "c_types.h"
//#include "mqtt/mqtt.h"


//#define printf(...) os_printf( __VA_ARGS__ )
#define DELAY 100
 os_timer_t mainTimer;
 os_timer_t menuTimer;
 os_timer_t errorTimer;
rboot_config config_rboot;
mqttserver mqtt_Server;
//static os_timer_t beepTimer;
static u8 tick;
#define OFFSETDISPLAY 0
//extern tcp_sent_flash_status status_sent_flash;





//extern int os_printf_pl;us(const char * format, ...);
//extern int __mingw_stdio_redirect__(printf)(const char*, ...);

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}


static ICACHE_FLASH_ATTR
check_sleep()
{
	if (sleep <= 0){
	  sleep = 0;
	  program.on= 0;
	  program.mode= lastState;
	  eeprom_saveStatus();
	  mqttPublishStatus();
//	  displayMode=80;
//	  clearDisplayFlag = 1;
//	  Display();
	}
	if(sleep!=currentSleep){
		currentSleep=sleep;
		mqttPublishStatus();
	}

}

void ICACHE_FLASH_ATTR
check_auto()
{
	//Check for time
	u16 time_on;
	u16 time_off;
	u16 time_now=hours*60+minutes;
	u8 currentState=program.on;
	programNumberPower=0;
	program.on=0;
	u8 i=0;
	for (i=0;i<9;i++)
	{
	  if(programer[i].mode==1)
	  {
		  //ets_uart_printf("mode %d",program.mode);
	    //program.on=0;
	    time_on=programer[i].timeOn;
	    time_off=programer[i].timeOff;
	    while(time_on !=time_off)
	    {
         if(time_on==time_now)
         {
          program.on=1;
          programNumberPower=programer[i].number;
          //Update for Display
          timeOn=programer[i].timeOn;
          timeOff=programer[i].timeOff;
          //mqttPublishStatus();
          break;
         }
	     time_on++;
	     if(time_on==1441)time_on=0;
	    }
	  }
	}

	if(currentState!=program.on){
		mqttPublishStatus();
//		if(program.on){
//			displayMode=81;
//			clearDisplayFlag = 1;
//			Display();
//		} else {
//			displayMode=80;
//			clearDisplayFlag = 1;
//			Display();
//		}
	}
	//ets_uart_printf("ProgramNumberPower %d\r\n",programNumberPower);
}




static ICACHE_FLASH_ATTR
WifiLanOn() {
//	if (clearDisplayFlag == 1) {
//		ClearDisplay();
//		selectMenu = 0;   //On entry reset select
//		//   maxSelect=MAXMENU;
//	}

	//char str[20];
	//ets_sprintf(str, "     Wifi LAN is On !");
	//displayString(str, 5, OFFSETDISPLAY);
	if (program.wifiLan != 1) {
		program.wifiLan = 1;
		eeprom_saveStatus();
		//system_restart();
		//wifi_Server();
		InitWifi();

	}
}

static ICACHE_FLASH_ATTR
WifiLanOff() {
//	if (clearDisplayFlag == 1) {
//		ClearDisplay();
//		selectMenu = 0;   //On entry reset select
//		//   maxSelect=MAXMENU;
//	}

	//char str[20];
	//ets_sprintf(str, "     Wifi LAN is Off !");
	//displayString(str, 5, OFFSETDISPLAY);
	if (program.wifiLan != 0) {
		program.wifiLan = 0;
		eeprom_saveStatus();
		//wifiStatus.count=0;
		//system_restart();
		//wifi_station_disconnect();
		InitWifi();
	}
}








static ICACHE_FLASH_ATTR
Main(void){
    //disarm timer
	os_timer_disarm(&mainTimer);
	//Check for scaned wifi
 	if(wifiStatus.scaned==1)
	{
	 //send_respond for scaned wifi
	  respond_for_lan_scanwifi();
	  wifiStatus.scaned=0;
	}

// 	if(oled.countErr<=0 && status.errtimer ){
// 		//start error timer
// 	 DEBUG("Err Count is zero... \r\n");
// 	 os_timer_disarm(&errorTimer);
// 	 os_timer_setfn(&errorTimer,ErrorTimer,NULL);
// 	 os_timer_arm(&errorTimer,ERROREXITTIME,0);
// 	 status.errtimer=0;
// 	}

  //Simple Clock
   tick++;
	if (tick == 100) {
		tick = 0;
		wifiStatus.count--;
		if (wifiStatus.count < 0)
			wifiStatus.count = 0;
		if(status.mqttCount!=0)
		{
		 status.mqttCount--;
		 if(status.mqttCount<0)status.mqttCount=0;
		}
		//softapStatus();
		//Display();
		seconds++;
		//Sync local time
		u8 minuteNow;
		minuteNow=minutes;
		if (seconds % 15 == 0)ReadDS1307();
		if(minuteNow!=minutes){
		 if (sleep > 0)sleep--;
		}
		if (program.mode == 2)check_auto();
		if (program.mode == 1)check_sleep();


		if (seconds > 59) {
			minutes++;
			if (sleep > 0)sleep--;
			seconds = 0;
			//Display();
			//status.tcp=0;
			if (minutes > 59) {
				minutes = 0;
				hours++;
				if (hours > 24)
					hours = 0;
			}
		}

//		if(displayTimer<=0 && oled.on){
//			//oled_contrast(0x00);
//			oled_off();
//			displayTimer=0;
//		}
//		else displayTimer--;


		//Check program status
		if (program.on) POWER_ON;
		else  POWER_OFF;



}

        //Key 1 Show scanning
	//u8 key = ScanKey();
	//ets_uart_printf("Key is: %d\n",key);
	if(key!=0){
		keypush=key;
		key=0;
	if (keypush == 1) {
		//key=0;
		ets_uart_printf("Button 1 \n");
		 if(program.on==1){
			 program.on=0;
			 program.mode=0;
		 }else{
			 program.mode=0;
			 program.on=1;
		 }
		 eeprom_saveStatus();
		//eeprom_readAray(0,256);
		//menuTimer.timer_expire=WAITTIME*10000;
 /*
		os_timer_arm(&menuTimer, WAITTIME, 0);
		//Default Mode On Power
		if(displayMode==0 || oled.i2c==0){
			program.mode = 0;
		    program.on = 1;
		    eeprom_saveStatus();
		    POWER_ON;
		    mqttPublishStatus();
//		    displayMode=81;
//		   	clearDisplayFlag = 1;
//		   	Display();
		}
		if (displayMode == 1) {       //Enter Selected Menu
			 if(selectMenu !=0){
			displayMode = 10*displayMode + selectMenu;
			//selectMenu=0;
			 }
			 clearDisplayFlag = 1;

		} else if (displayMode > 10 && displayMode<99) {
			 if(selectMenu !=0){
			displayMode = 10*displayMode + selectMenu;
			//selectMenu=0;
			 }
			clearDisplayFlag = 1;
		} else if (displayMode > 100 && displayMode<999) {
			 if(selectMenu !=0){
			displayMode = 10*displayMode + selectMenu;
			//selectMenu=0;
			 }
			clearDisplayFlag = 1;
		} else if (displayMode==2){
			if(selectMenu !=0){
			 displayMode=10*displayMode+selectMenu;
			}
			clearDisplayFlag=1;
		}

		ets_uart_printf("Display Mode: 0x%X \n", displayMode);
		Display();
		//ets_uart_printf("menuTimer  period  : 0x%X\n",menuTimer.timer_period);
		//ets_uart_printf("menuTimer  expire  : 0x%X\n",menuTimer.timer_expire);

		//oled_on();

		 */
	} else if (keypush == 2) {
		//key = 0;
		oled_contrast(0xCF);
		ets_uart_printf("Button 2 \n");
		u8 mode=wifi_get_opmode();
		//Button 2 Turn On/Off Wifi
		if (program.wifiLan == 0){
			//Wifi is ON goto Turun Off
			if(mode!=3){
			 WifiLanOn();
			 ets_uart_printf("WifiLan On \n");
			}
		}else
		{
			//Wifi is Off goto Turun On
			if(mode!=1){
		    WifiLanOff();
		    ets_uart_printf("WifiLan Off \n");
			}
		}

//		os_timer_arm(&menuTimer, WAITTIME, 0);
//		if(displayMode==0){
//			ProgramMenu();
//		}
//		if (displayMode != 0) {
//			selectMenu++;
//			changeMenu=true;
//			if (selectMenu > maxSelect){
//				selectMenu = 1;
//			}
//		}
//
//		Display();
		//InverseRow(0);
		//ets_uart_printf("menuTimer  period  : 0x%X\n",menuTimer.timer_period);
		//ets_uart_printf("menuTimer  expire  : 0x%X\n",menuTimer.timer_expire);
		//oled_off();
	} else if (keypush == 3) {
		//key = 0;
		oled_contrast(0x00);
		ets_uart_printf("Button 3 \n");
		if(program.mode==0){
			         program.on=0;
					 program.mode=2;
				 }else{
					 program.mode=0;
					 program.on=0;
				 }
		eeprom_saveStatus();
/*
		os_timer_arm(&menuTimer, WAITTIME, 0);
		if(displayMode==0){
			DisplayUser();
   		  // exit;
		}
		else if (displayMode != 0) {
			selectMenu--;
			changeMenu=true;
			if (selectMenu <= 0){
				selectMenu = maxSelect;
			}
		}
		Display();
*/
	} else if (keypush == 4) {
		//key = 0;
		ets_uart_printf("Button 4 \n");
//		os_timer_arm(&menuTimer, WAITTIME, 0);
//		if (displayMode == 0 || oled.i2c==0) {
//			program.mode = 0;
//			program.on = 0;
//			eeprom_saveStatus();
//			POWER_OFF;
//			mqttPublishStatus();
////			displayMode=80;
////		   	clearDisplayFlag = 1;
////		   	Display();
//		} else if (displayMode != 0) {
//			clearDisplayFlag=1;
//			displayMode=9; //Exit
//			Display();
//		}


		//Display();
	} else if (keypush == 23) {
		//key=0;
		ets_uart_printf("Button 23 Menu \n");
		//ModeMenu();
		//Display();
	}
}


       if (wifiStatus.connect) {
         if(!status.tcp)
           {
           	status.tcp=1;
    	    TcpInit();
    	    //initICMP();
           }
		if (!status.mqtt) {
			if(status.mqttCount==0){
				status.mqtt = 1;
				status.mqttCount=20;
				MqttInit();

			}

		}
	}

	if (wifiStatus.lan) {
		if (!status.tcp) {
			status.tcp = 1;
			TcpInit();
			//initICMP();
		}

//		 //Debug error mqtt on Lan setting
//		if (!status.mqtt) {
//					status.mqtt = 1;
//					MqttInit();
//		}
	}


      if(!wifiStatus.tryconnet)
      {
    	 if(wifiStatus.count==0)
    	 {
    		InitWifi();
    		wifiStatus.count=10;
    		//wifi_Server();

     	 }

      }

      //Key 3 Scanning
//	  if (ScanKey()==3)
//	   {
//	     WifiStationScan();
//	   }

      //feed dog
      system_soft_wdt_feed();

      //re-armed timer to Main
      os_timer_setfn(&mainTimer,Main,NULL);
      os_timer_arm(&mainTimer,10,0);
 }

static  ICACHE_FLASH_ATTR
power_init_pin()
{
	 //Pin Power
	PIN_FUNC_SELECT(POWER_MUX,FUNC_GPIO15);
	 //Enable Pull DOWN
	PIN_PULLUP_EN(POWER_MUX);
	 //Set as output
    // GPIO_DIS_OUTPUT(POWER_PIN);
}

void ICACHE_FLASH_ATTR
SystemInitOk()
{
	 program.on=0;
	 program.mode=0;
	 program.wifiServer=0;
	 programNumber=1;
	 wifiStatus.scan=0;
	 wifiStatus.tryconnet=0;
	 wifiStatus.connect=0;
	 wifiStatus.count=0;
	 wifiStatus.lan=0;
	 //wifiStatus.enableLan=1;
	 status.tcp=0;
	 status.i2c=0;
	 status.tcpscaned=0;
	 status.gotIP=0;
	 status.mqtt=0;
	 status.errtimer=1;
	 displayMode=0;
	 selectMenu=0;
	 changeMenu=false;
	 maxSelect=MAXMENU;
	 //keyPress=false;
	 power_init_pin();
	 InitKeys();
	 //beep_init_pin();
	 //beep();
	 //timer1_isr_init();
	 Beep(0);
	 u8 eeprom;
     //INFO("Hello I am Here \r\n");

      //Test eeprom
	 eeprom=eepromReadByte(0);
	 // ets_uart_printf("\n\r Eerpom data:0x%X",eeprom);
	   if(!status.i2creaderror)
	     {
	    	 //ets_uart_printf("\n\r Eerpom  is OK data:0x%X",eeprom);
	    	 if(eeprom==0xAA)
	    	 {
	    		 //eeprom is init read status /ssid/password
	    		 eeprom_readStaus();
	    		 // POWER_ON;
                 //eeprom_readTimer();
                 // READ Wifi
                 eeprom_readWifi();
                 //mqttserver mqtt_Server;
                 mqtt_Server = eeprom_readMqttServer();
//                 if(!eeprom_readMqttServer()){
//                     eeprom_initMqttServer();
//                 }

	    	 }
	    	 else
	    	 {
	    		 //new eeprom init it
	    	     eeprom_init();
	    	 }
	     }
	      else
	     {
	   	  ets_uart_printf("\n\r Eeprom not found !");
	     }

	     //test saveSSID
	     //eeprom_readAray(0,64);
	     /*
	     ets_sprintf(lan_ssid,"%s",LAN_WIFI);
	     ets_uart_printf("Test save 1 LAN_SSID:<%s>\n\r",lan_ssid);
	     eepromWriteData(EEPROM_LAN_SSID,lan_ssid);
	      //ets_uart_printf("Test save 2 LAN_SSID:<%s>\n\r",lan_ssid);
	       //write len of string in first byte
	     lan_ssid[0]=32;
	     eepromReadData(EEPROM_LAN_SSID,lan_ssid);
	     ets_uart_printf("Test save 3 LAN_SSID:<%s>\n\r",lan_ssid);

	      //eeprom_writePassword("zaq12wsx!");
	      //eeprom_readAray(0,128);
         */
	 program.wifiServer=0;

	 //displayTimer=DISPLAYTIMER;
     //initOledDisplay();



     if(ReadDS1307())
      {
    	 //ets_uart_printf("\n\r Timekeeper  is OK !");
    	  //Read Temperature is found it
    	 ReadDS1307_Temperature();
      }
     else
      {
    	 ets_uart_printf("\n\r TimerKeeper not found!");

      }

//	eeprom = eepromReadByte(0);
//	if (eeprom) {
//		//ets_uart_printf("\n\r Eerpom  is OK data:0x%X",eeprom);
//	} else {
//		// ets_uart_printf("\n\r Eeprom not found !");
//	}

        //SetTimeDate(0,52,15,3,8,2,17);
	  // Init Maih Thread
     //Clear display
     config_rboot = rboot_read_config();

//     ClearDisplay();
//     Display();


     if(program.wifiLan){
    	 InitWifi();
     }
	 os_timer_disarm(&mainTimer);
	 os_timer_setfn(&mainTimer,Main,NULL);
	 os_timer_arm(&mainTimer,10,0);

     //eeprom_initProgramer();
	 eeprom_readProgramer();

}

void ICACHE_FLASH_ATTR
user_init(void){
   uart_init(BIT_RATE_115200,BIT_RATE_115200);
   //system_soft_wdt_stop();
   wifi_set_event_handler_cb(eventHandler);
   //if program.wifiLan=1 set wifi_set_opmode(STATION_MODE); else set (STATION_MODE)

//   if(wifi_get_opmode()!=STATIONAP_MODE)
//   {
//    wifi_set_opmode(STATIONAP_MODE);
//   }
   //ets_uart_printf("Stattion Mode: <0x%X> \n",wifi_get_opmode());
   wifi_station_set_auto_connect(FALSE);
   system_init_done_cb(SystemInitOk);
}




