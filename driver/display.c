/*

 * display.c
 *
 *  Created on: 03.12.2018 �.
 *      Author: admin
 */

//void ICACHE_FLASH_ATTR
//Display (void) {
//  return;
//}


//static void ICACHE_FLASH_ATTR LcdPutChar(char ch)
//{
//	Lcd(ch);
//}




//void ICACHE_FLASH_ATTR
//DisplayMsg(char* msg){
//	if (clearDisplayFlag == 1) ClearDisplay();
//	if(!oled.on) oled_on();
//   displayTimer=DISPLAYTIMER;
//   displayMode = 99;
//   displayString(msg, 4, OFFSETDISPLAY);
//   os_timer_disarm(&menuTimer);
//   os_timer_setfn(&menuTimer,TimerMenu,NULL);
//   os_timer_arm(&menuTimer,2*EXITTIME,0);
//}

/*
static ICACHE_FLASH_ATTR
ModeMenu(void)
 {
	 ets_uart_printf("Menu Mode... \n");
	 if(displayMode==1)return; //Now is in this mode
	 displayMode=1;
	 clearDisplayFlag=1;
	 //Start timer menuMode after that return in Main Menu
	 os_timer_disarm(&menuTimer);
	 os_timer_setfn(&menuTimer,TimerMenu,NULL);
	 os_timer_arm(&menuTimer,WAITTIME,0);
	 Display();
 }


static ICACHE_FLASH_ATTR
ProgramMenu(void){
   ets_uart_printf("Program Menu... \n");
   if(displayMode==2)return; //Now is in this mode
		 displayMode=2;
		 clearDisplayFlag=1;
		 //Start timer menuMode after that return in Main Menu
		 os_timer_disarm(&menuTimer);
		 os_timer_setfn(&menuTimer,TimerMenu,NULL);
		 os_timer_arm(&menuTimer,WAITTIME,0);
		 Display();
}
*/

//static ICACHE_FLASH_ATTR
// AutoProgramMenu(u8 programNumber){
//	if(programNumber<0 || programNumber>10)return;
//	if (clearDisplayFlag == 1) {
//			ClearDisplay();
//			selectMenu = 0;
//		}
//	char str[20];
//	u8 programMode=programer[programNumber].mode;
//	u16 timeOn=programer[programNumber].timeOn;
//	u8  hourOn=timeOn/60;
//	u8  minuteOn=timeOn-60*hourOn;
//	u16 timeOff=programer[programNumber].timeOff;
//	u8  hourOff=timeOff/60;
//	u8  minuteOff=timeOff-60*hourOff;
//	DisplayString("Auto Programm Menu",0,OFFSETDISPLAY);
//	ets_sprintf(str, "Programm Number:%d",programNumber);
//	DisplayString(str,1,OFFSETDISPLAY);
//	ets_sprintf(str, "Active Passive:%d",programMode);
//	DisplayString(str,2,OFFSETDISPLAY);
//	ets_sprintf(str, "Time On %d:%d",hourOn,minuteOn);
//	DisplayString(str,2,OFFSETDISPLAY);
//	ets_sprintf(str, "Time Off %d:%d",hourOff,minuteOff);
//	DisplayString(str,3,OFFSETDISPLAY);
//	DisplayString("Next Programm",4,OFFSETDISPLAY);
//	DisplayString("Exit",5,OFFSETDISPLAY);
//
//}

//void ICACHE_FLASH_ATTR
//ErrorTimer(void){
//	DEBUG("Error Count SET COUNTER \r\n");
//	os_timer_disarm(&errorTimer);
//	//oled.countErr=COUNTERR;
//	oled.countErr=1;
//
//}

//void ICACHE_FLASH_ATTR
//Display (void) {
//    system_soft_wdt_feed();
//    //if() display off return...
//      //DEBUG("Display.... : %d:%d \r\n",oled.i2c,oled.countErr);
//    if(!oled.on){
//    	//DEBUG("Oled is off i am return... \r\n");
//    	return;
//    }
//    if(!oled.i2c && oled.countErr<=0){
//    	//DEBUG("Error Start Timer ... \r\n");
//    	oled.on=0;
//    	//start err timer
//    	os_timer_disarm(&errorTimer);
//    	os_timer_setfn(&errorTimer,ErrorTimer,NULL);
//    	os_timer_arm(&errorTimer,ERROREXITTIME,0);
//    	return;
//    }
//	if(!oled.i2c || oled.countErr==1){
//		//DEBUG("From Display i2c oled error I am goining to Init it... \r\n");
//		if(!initOledDisplay()) return ;
//	}
//	if(displayMode == 0)         DisplayNormal();
//	else if(displayMode == 1)    DisplayMenu();
//	else if(displayMode == 11)   LanMenu();
//    else if(displayMode == 12)   WanMenu();
//	else if(displayMode == 13)   SetupMenu();
//    else if(displayMode == 112)  LanOnOff();
//	else if(displayMode == 1121) WifiLanOn();
//	else if(displayMode == 1122) WifiLanOff();
//	else if(displayMode ==2)     DisplayProgramMenu();
//	else if(displayMode ==21)    DisplayAutoOn();
//	else if(displayMode ==22)    DisplayAutoOff();
//	else if(displayMode ==23)    DisplaySleep();
//  	else if(displayMode ==24)    DisplayExitProgram();
//	else if(displayMode == 9)    DisplayExit();
//	else if(displayMode == 81)   DisplayMsg("       P O W E R  ON");
//	else if(displayMode ==80)    DisplayMsg("      P O W E R  OFF");
//	else if(displayMode == 99)   return; //user mode
//	else {
//	displayMode=0;
//	DisplayNormal();
//	}
//}

//static ICACHE_FLASH_ATTR
//ReadChipId(void){
//	 ets_uart_printf("Chip id <0x%X>  \n",system_get_chip_id());
//}

/*
static ICACHE_FLASH_ATTR
LanMenu(void) {
	//ClearDisplay
	if (clearDisplayFlag == 1) {
		ClearDisplay();
		selectMenu = 0;   //On entry reset select
		maxSelect = MAXMENULAN;
	}
	
	if(changeMenu==true){
	char lan[20];
	ets_sprintf(lan, "          L A N");
	displayString(lan, 0, OFFSETDISPLAY);
	displayString("1.SET TO DEAFAUT", 1, OFFSETDISPLAY);
	ets_sprintf(lan, "2.LAN ON/OFF:%d", program.wifiLan);
	displayString(lan, 2, OFFSETDISPLAY);
	ets_sprintf(lan, "SSID:%s", lan_ssid);
	displayString(lan, 3, OFFSETDISPLAY);
	ets_sprintf(lan, "PWD :%s", lan_pwd);
	displayString(lan, 4, OFFSETDISPLAY);
	ets_sprintf(lan, "TYPE:%d", lan_type);
	displayString(lan, 5, OFFSETDISPLAY);
	 if (selectMenu != 0) {InverseRow(selectMenu);}
      changeMenu=false;
	}
}

static ICACHE_FLASH_ATTR
LanOnOff(void){
	if (clearDisplayFlag == 1){
		 ClearDisplay();
		 selectMenu=0;   //On entry reset select
		 maxSelect=MAXMENULAN;
	   }

	if(changeMenu==true){
	//char lan[20];
	displayString("     W I F I  L A N ", 0, OFFSETDISPLAY);
	displayString("1.ON", 1, OFFSETDISPLAY);
	displayString("2.OFF", 2, OFFSETDISPLAY);
	 if (selectMenu != 0) {InverseRow(selectMenu);}
      changeMenu=false;
	}
}

static ICACHE_FLASH_ATTR
WanMenu(void){
	//ClearDisplay
  if (clearDisplayFlag == 1){
	 ClearDisplay();
	 //selectMenu=11;   //On entry reset select
   }
  //char lan[20];
  displayString("          W A N", 0, OFFSETDISPLAY);
}

static ICACHE_FLASH_ATTR
TimerMenu(void) {
   os_timer_disarm(&menuTimer);
   if(displayMode==0) return;
   displayMode=0;
   Beep(100);
   displayTimer=DISPLAYTIMER;
   ClearDisplay();
   Display();
 }


static ICACHE_FLASH_ATTR
DisplayExit(){
if (clearDisplayFlag == 1){
	ClearDisplay();
 }
  displayString("      EXIT FROM MENU",4,OFFSETDISPLAY);
  os_timer_disarm(&menuTimer);
  os_timer_setfn(&menuTimer,TimerMenu,NULL);
  os_timer_arm(&menuTimer,EXITTIME,0);
}

static ICACHE_FLASH_ATTR
DisplayProgramMenu(){
	if (clearDisplayFlag == 1){
			 ClearDisplay();
			 selectMenu=0;   //On entry reset select
			 changeMenu=true;
			 maxSelect=4;
		   }
	if(changeMenu==true){
	//char buffer[20];
	displayString("       Program Menu",0,OFFSETDISPLAY);
	displayString("1. Auto on",1,OFFSETDISPLAY);
	displayString("2. Auto off",2,OFFSETDISPLAY);
	displayString("3. Sleep",3,OFFSETDISPLAY);
	displayString("4. Exit",4,OFFSETDISPLAY);
	 if (selectMenu != 0) {InverseRow(selectMenu);}
      changeMenu=false;
	 }
}

static ICACHE_FLASH_ATTR
DisplayAutoOn(){
	if (clearDisplayFlag == 1){
	 ClearDisplay();
	}
	 displayString("   AUTO ON",4,OFFSETDISPLAY);
	 program.mode=2;
	 eeprom_saveStatus();
	 os_timer_arm(&menuTimer, EXITTIME, 0);
}

static ICACHE_FLASH_ATTR
DisplayAutoOff(){
	if (clearDisplayFlag == 1){
	 ClearDisplay();
	}
	 displayString("   AUTO OFF",4,OFFSETDISPLAY);
	 program.mode=0;
	 eeprom_saveStatus();
	 os_timer_arm(&menuTimer, EXITTIME, 0);
}

static ICACHE_FLASH_ATTR
DisplaySleep(){
	if (clearDisplayFlag == 1){
	ClearDisplay();
	}
    displayString("   SLEEP",4,OFFSETDISPLAY);
	program.mode=1;
	eeprom_saveStatus();
	os_timer_arm(&menuTimer, EXITTIME, 0);

}



static ICACHE_FLASH_ATTR
DisplayExitProgram(){
	if (clearDisplayFlag == 1){
	ClearDisplay();
	}
    displayString("   EXIT PROGRAM MENU",4,OFFSETDISPLAY);

	os_timer_arm(&menuTimer, EXITTIME, 0);
}

static ICACHE_FLASH_ATTR
SetupMenu(void){
	//ClearDisplay
  if (clearDisplayFlag == 1){
	 ClearDisplay();
	 //selectMenu=11;   //On entry reset select
   }
  char lan[20];
  ets_sprintf(lan, "        S E T U P");
  displayString(lan, 0, OFFSETDISPLAY);

}

static ICACHE_FLASH_ATTR
DisplayMenu(void)
{
	//Check for Display OK ?
	//if (!oled.i2c) initOledDisplay();
	//ClearDisplay
	if (clearDisplayFlag == 1){
		ClearDisplay();
	    selectMenu=0;   //On entry reset select
	    maxSelect=MAXMENU;
	}
	//Display Menu
	char menu[20];
	ets_sprintf(menu, "          M E N U");
	displayString(menu, 0, OFFSETDISPLAY);
	if (selectMenu == 1)
		InverseRow(1);
	else {
		ets_sprintf(menu, "1. LAN");
		displayString(menu, 1, OFFSETDISPLAY);
	}
	if (selectMenu == 2)
		InverseRow(2);
	else {
		ets_sprintf(menu, "2. WAN");
		displayString(menu, 2, OFFSETDISPLAY);
	}
	if (selectMenu == 3)
		InverseRow(3);
	else {
		ets_sprintf(menu, "3. SETUP");
		displayString(menu, 3, OFFSETDISPLAY);
	}

	char str[20];
	ets_sprintf(str, "\n20%02d/%02d/%02d %02d %02d:%02d:%02d", year, month,
			date, days, hours, minutes, seconds);
	//ets_uart_printf(str);
	displayString(str, 4, 5);
	//Select Option
}


*/

//bool ICACHE_FLASH_ATTR
//DisplayNormal(void)
//{
//	//lcd_line=1;
//	 if(!oled.i2c) initOledDisplay();
//	 char str[20];
//	 if( clearDisplayFlag==1) ClearDisplay();
//	 ets_sprintf(str,"\n20%02d/%02d/%02d %01d %02d:%02d:%02d",year,month,date,config_rboot.current_rom,hours,minutes,seconds);
//     //ets_uart_printf(str);
//     if(!displayString(str,0,OFFSETDISPLAY)) return false;
//     ReadDS1307_Temperature();
//     ets_sprintf(str,"T:%s Mqtt:%d",temperature,status.mqttOK);
//     // ets_sprintf(str,"ADC:%d     VDD:%d",ScanKey(),system_get_vdd33());
//     if(!displayString(str,1,OFFSETDISPLAY)) return false;
//     if(programNumberPower!=0){
//       ets_sprintf(str,"AUTO %d",programNumberPower);
//      }  else  ets_sprintf(str,"AUTO      ");
//    if(!displayString(str,2,50))return false;
//     u8 on_hour=timeOn/60;
//     u8 on_minute=timeOn-on_hour*60;
//     u8 off_hour=timeOff/60;
//     u8 off_minute=timeOff-off_hour*60;
//     ets_sprintf(str,"     On:%02d:%02d Off:%02d:%02d",on_hour,on_minute,off_hour,off_minute);
//    if(!displayString(str,3,OFFSETDISPLAY))return false;
//     //ets_sprintf(str,"%s\r\n",str);
//     //DEBUG(str);
//
//         switch(program.mode){
//         case 0:
//        	 ets_sprintf(str,"Status:  MANUAL         ");
//        	 break;
//         case 1:
//        	 ets_sprintf(str,"Status:   SLEEP: %d  ",sleep);
//        	 break;
//         case 2:
//        	 ets_sprintf(str, "Status:   AUTO          ");
//        	 break;
//         }
//
//     if(!displayString(str,4,OFFSETDISPLAY))return false;
//
//       if(program.on){
//          ets_sprintf(str,"Power:     ON ");
//        }
//       else{
//    	 ets_sprintf(str,"Power:     OFF");
//       }
//       if(!displayString(str,5,OFFSETDISPLAY)) return false;
//       sint8 rssi;
//       rssi=wifi_station_get_rssi();
//       if(wifiStatus.connect){
//        WifiStatus();
//        ets_sprintf(str,"IP:%s:<%d>",ipadrchar,rssi);
//        if(!displayString(str,6,OFFSETDISPLAY)) return false;
//       }
//       else{
//    	ets_sprintf(str,"IP:%s:<%d>","                ",rssi);
//    	if(!displayString(str,6,OFFSETDISPLAY)) return false;
//       }
//       ets_sprintf(str,"%s:%s:%x    ",lan_ssid,lan_pwd,lan_type);
//       if(!displayString(str,7,OFFSETDISPLAY)) return false;
//
//	   //ets_uart_printf("\n\r Free heap size = %d", system_get_free_heap_size());
//
//      // ets_uart_printf("wifiStatus count: %d\n\r",wifiStatus.count);
//	 //WifiStatus();
//	 //initICMP();
//	 //lcdString(str);
//       return true;
//}

/*
static ICACHE_FLASH_ATTR
DisplayUser(){
	ClearDisplay();
	char str[20];
//	ets_sprintf(str,"user: %d",strlen(mqtt_Server.mqttUser));
//	displayString(str, 1, OFFSETDISPLAY);
//	ets_sprintf(str,"pwd: %d",strlen(mqtt_Server.mqttPassword));
//		displayString(str, 2, OFFSETDISPLAY);
//		ets_sprintf(str,"tRead: %d",strlen(mqtt_Server.mqttTopicRead));
//			displayString(str, 3, OFFSETDISPLAY);
//
//	ets_sprintf(str,"Server: %d",sizeof(mqtt_Server));
//	displayString(str, 4, OFFSETDISPLAY);

	ets_sprintf(str,"user: %s",mqtt_Server.mqttUser);
	displayString(str, 1, OFFSETDISPLAY);
	ets_sprintf(str,"pwd: %s",mqtt_Server.mqttPassword);
	displayString(str, 2, OFFSETDISPLAY);
	ets_sprintf(str,"tRead: %s",mqtt_Server.mqttTopicRead);
	displayString(str, 3, OFFSETDISPLAY);
	ets_sprintf(str,"tWrite: %s",mqtt_Server.mqttTopicWrite);
	displayString(str, 4, OFFSETDISPLAY);
	ets_sprintf(str,"port: %d",mqtt_Server.mqttPort);
	displayString(str, 5, OFFSETDISPLAY);
    displayTimer=DISPLAYTIMER;
	displayMode=99;
	return;
}
*/