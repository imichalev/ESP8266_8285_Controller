/*
 * tcpClient.c
 *
 *  Created on: 8.01.2017 �.
 *      Author: admin
 */

#include "driver/tcpServer.h"
#include "user_interface.h"
#include "espconn.h"
#include "osapi.h"
#include "string.h"
#include "driver/wifi.h"
#include "user_config.h"
//#include "ping.h"
#include "driver/eeprom.h"
#include "mqtt.h"
#include "mem.h"
#include "driver/pgmspace.h"
#include "spi_flash.h"
#include "driver/oled.h"
#include "driver/eeprom.h"

static struct espconn esp_conn;
static esp_tcp esptcp;
//struct ping_option pingopt;
char *ok = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
char *dataHTML;
//char *buffer;
//Temporary for espconn
static struct epsconn *esp_conn_temp;
MQTT_Client mqttClient;
int post_len=0;
int post_remote_port=0;
uint32 len_post_content=0;
u8 update_check_sum=0;
char *post_remoteip_address;
rboot_write_status status_write;
//tcp_sent_flash_status status_sent_flash;
u8 reboot=0;
extern  os_timer_t mainTimer;
extern  os_timer_t menuTimer;
extern  mqttserver mqtt_Server;

//region Rboot
uint32 ICACHE_FLASH_ATTR
ControlSum(uint32 len ){

	//u8 *byte;
	rboot_config config;
	config = rboot_read_config();
	uint8 controlSum=0;
    #define BUFER_SIZE 0x1000
	u8 rom=config.current_rom;
	//Replace current boot rom
	if(rom==0)rom=1;
	else rom=0;
	uint32 i=0;
	uint32 j=0;
	uint32 readlen;
	uint32 readpos;

	uint32 remaining;
	if(len==0){
	 len=config.len[rom];
	}
	remaining=len;
	//ets_uart_printf("ROM: %d \n" ,rom);
	//ets_uart_printf("Config len: %d \n" ,remaining);
	//ets_uart_printf("Config len_0: 0x%X \n" ,config.len[0]);
	readlen=(remaining<BUFER_SIZE)? remaining:SECTOR_SIZE;
	//ets_uart_printf("Read len: 0x%X \n" ,readlen);

    readpos=config.roms[rom];
    //ets_uart_printf("Readpos: 0x%X \n" ,readpos);
    //ets_uart_printf("Free of RAM <0x%X> \n",system_get_free_heap_size());

    uint8 *buffer;
    buffer=(uint8*)os_malloc(BUFER_SIZE);
    //ets_uart_printf("Free of RAM <0x%X> \n",system_get_free_heap_size());
	while(remaining > 0){
	readlen=(remaining<BUFER_SIZE)? remaining:BUFER_SIZE;
	//ets_uart_printf("Read len: 0x%X \n" ,readlen);
	spi_flash_read(readpos, (uint32*)(void*)buffer,readlen);
    remaining-=readlen;
	readpos+=readlen;
	system_soft_wdt_feed();
	 for(j=0;j<readlen;j++){
	   controlSum=controlSum^(buffer[j]);
	   //controlSum=controlSum^(u8)(buffer[j]&0xff00);

	 }
	}
	os_free(buffer);
	ets_uart_printf("ControlSum: %d \n" ,controlSum);
	return controlSum;
}

rboot_config ICACHE_FLASH_ATTR
rboot_read_config(){

	rboot_config config;
	spi_flash_read(BOOT_CONFIG_SECTOR * SECTOR_SIZE, (uint32*)&config, sizeof(rboot_config));
	return config;
}

bool ICACHE_FLASH_ATTR
rboot_write_config(rboot_config *config){
   uint8 *buffer;
   buffer=(uint8*)os_malloc(SECTOR_SIZE);
   if(!buffer) return false;
   //read current state
   spi_flash_read(BOOT_CONFIG_SECTOR * SECTOR_SIZE, (uint32*)((void*)buffer), SECTOR_SIZE);
   //change  new state in structure
   memcpy(buffer,config,sizeof(rboot_config));
   //erase flash sector
   spi_flash_erase_sector(BOOT_CONFIG_SECTOR);
   //save state to flash
   spi_flash_write(BOOT_CONFIG_SECTOR * SECTOR_SIZE,(uint32*)((void*)buffer), SECTOR_SIZE);
   os_free(buffer);
   return true;
}

void ICACHE_FLASH_ATTR
chaged_boot_rom(){
	rboot_config config;
	config = rboot_read_config();
	if(config.current_rom == 0){
		config.current_rom=1;
		config.len[1]=(uint32)len_post_content;
		ets_uart_printf("Config len_1: 0x%X \n" ,config.len[1]);
	} else {
		config.current_rom=0;
		config.len[0]=(uint32)len_post_content;
		ets_uart_printf("Config len_0: 0x%X \n" ,config.len[0]);

	}
	rboot_write_config(&config);
	//restar esp8266
	os_printf("Restarting...\r\n\r\n");
	//GPIO_OUTPUT_SET(POWER_PIN,0);
    POWER_OFF;
	system_soft_wdt_feed();
	system_restart();
}

void ICACHE_FLASH_ATTR
save_boot_rom_len(){
  rboot_config config;
  config = rboot_read_config();
  config.len[config.current_rom]=(uint32)len_post_content;
  rboot_write_config(&config);
}

bool ICACHE_FLASH_ATTR
rboot_write_flash(rboot_write_status *status, uint8 *data, uint16 len){

	bool ret= false;
    u8 *buffer;
    int32 lastsec;
    system_soft_wdt_feed();
	if ( data == NULL || len==0) {
		return true;
	}

	buffer = (u8 *)os_malloc(len+status->extra_count);
	if(!buffer){
		//os_printf("No free RAM \n");
		return false;
	}

	//copy if any remain bytes from last transffer
	if(status->extra_count !=0){
		memcpy(buffer,status->extra_bytes,status->extra_count);
	}
	//copy in new data
	memcpy(buffer+status->extra_count,data,len);

	// Now calculate new data.If must by multiple of 4
	// any renaing save to status for next action
	len+=status->extra_count;
	status->extra_count = len % 4;
    len -=status->extra_count;
    memcpy(status->extra_bytes,buffer+len,status->extra_count);

    // test if will FIT
    //erase sectors need for save data
    lastsec=((status->start_addr+len)-1) / SECTOR_SIZE;
    if(lastsec>=252){
        os_free(buffer);
//    	ClearDisplay();
//    	if(!oled.on){oled_on();}
//   		displayString("E R R O R ! ! !", 4, 1);
   		os_delay_us(1000);
   		//Restore Main Thread
   		os_timer_arm(&mainTimer,10,0);
       //os_printf("Error in address range it is: 0x%X \n" ,lastsec);
       return false;
    }
     while(lastsec > status->last_sector_erased){
    	status->last_sector_erased++;
    	spi_flash_erase_sector(status->last_sector_erased);
        //os_printf("Section is: 0x%d \n" ,lastsec);
    }

    //Now Write current buffer
     if(spi_flash_write(status->start_addr,(uint32 *)((void*)buffer),len)==SPI_FLASH_RESULT_OK){
     	 ret=true;
    	 status->start_addr+=len;
     }
       //status->start_addr+=len;
    os_free(buffer);
	return ret;
}

// On the End ensure any remaning bytes get written (needed for files not a multiple of 4 bytes)
bool ICACHE_FLASH_ATTR
rboot_write_end(rboot_write_status *status) {
	uint8 i;
	if (status->extra_count != 0) {
		for (i = status->extra_count; i < 4; i++) {
			status->extra_bytes[i] = 0xff;
		}
		return rboot_write_flash(status, status->extra_bytes, 4);
	}
	return true;
}

rboot_write_status ICACHE_FLASH_ATTR
rboot_write_init(uint32 start_addr) {
	rboot_write_status status = {0};
	status.start_addr = start_addr;
	status.start_sector = start_addr / SECTOR_SIZE;
	status.last_sector_erased = status.start_sector - 1;
	//status.max_sector_count = 200;
	//os_printf("I am here ! 1");
	return status;
}

//endregion;

static void ICACHE_FLASH_ATTR
tcp_server_sent_cb(void *arg) {
	//data sent successfully close connection
	struct espconn *pesp_conn = (struct espconn *) arg;
	DEBUG("tcp_server_sent_cb... \r\n");
	espconn_disconnect(pesp_conn);
	//DEBUG("from Sent cb Free of RAM <0x%X> \n",system_get_free_heap_size());
}

static void ICACHE_FLASH_ATTR
modeManual() {
	//save program ....
	ets_uart_printf("Mode Manu;Power:%d\r\n", program.on);
	programNumberPower = 0;
	eeprom_saveStatus();
}

static void ICACHE_FLASH_ATTR
modeSleep() {
	//sleep=atoi(token[2]);
	//check if sleep ok
	if (sleep == 0) {
		ets_uart_printf("Error in sleep parameter:%d\r\n", sleep);
		return;
	}
	program.mode = 1;
	program.on = 1;   //Power on
	programNumberPower = 0;
	ets_uart_printf("Mode Sleep timer:%d\r\n", sleep);
	//Save programer
	eeprom_saveStatus();
//	displayMode=81;
//	clearDisplayFlag = 1;
//	Display();
}

static void ICACHE_FLASH_ATTR
modeAuto() {
	programer[programNumber - 1].number = programNumber;
	programer[programNumber - 1].mode = programMode;
	programer[programNumber - 1].timeOn = timeOn;
	programer[programNumber - 1].timeOff = timeOff;
	//check auto
	check_auto();
	//save program,save programer
	eeprom_saveStatus();
	eeprom_writeProgramer(programNumber - 1);

}

static void ICACHE_FLASH_ATTR
program_status(void *arg, char *data){
	//ets_uart_printf("Respond POST STATUS... <%s>\n",data);
	char respond [100];
	char delimiter[2] = "&";
	char *token;
	u8 count = 0;
	//Check For status?&2&1
	bool numberOK = false;
	token = strtok(data, delimiter);
	while (token != NULL && count < 5) {
		if (count == 2) {   //Second position in status&?&2
			//ets_uart_printf("Second position in status %d \n",atoi(token));
			if (atoi(token) == 2) {
				numberOK = true;
			}
		} else if (count == 3) { //Third position in status
			//ets_uart_printf("Third position in status %d \n",atoi(token));
			if (numberOK) {
				programNumber = atoi(token);
			} else
				programNumber = 1;
		}
		count++;
		token = strtok(NULL, delimiter);
	}

	if (numberOK) {  //Read program Data

		programMode = programer[programNumber - 1].mode;
		timeOn = programer[programNumber - 1].timeOn;
		timeOff = programer[programNumber - 1].timeOff;
		ets_sprintf(respond, "%sprogram&%d&%d&%d&%d&%d&%s",
				ok,
				programNumber,
		        programMode,
				timeOn,
				timeOff,
				programNumberPower,
				temperature);
		//MQTT_Publish(client,mqtt_Server.mqttTopicWrite,respond, strlen(respond),0, 0);

	} else {

		if (program.mode == 0) {
			ets_sprintf(respond, "%sstatus&%d&%d&%s&\n",
					ok,
					program.mode,
					program.on,
					temperature);
		} else if (program.mode == 1) {
			ets_sprintf(respond, "%sstatus&%d&%d&%d&%s&\n",
					ok,
					program.mode,
					program.on,
					sleep,
					temperature);
		} else if (program.mode == 2) {
			programMode = programer[programNumber - 1].mode;
			timeOn = programer[programNumber - 1].timeOn;
			timeOff = programer[programNumber - 1].timeOff;
			ets_sprintf(respond, "%sstatus&%d&%d&%d&%d&%d&%d&%d&%s&\n",
					ok,
					program.mode,
					program.on,
					programNumber,
					programMode,
					timeOn,
					timeOff,
					programNumberPower,
					temperature);
		}
	}
	espconn_sent(arg, respond, os_strlen(respond));
	return;
}

void ICACHE_FLASH_ATTR
http_command(void *arg,char *data) {
	DEBUG("Mqtt Command received ... \r\n");
	MQTT_Client* client;
	char delimiter[2] = "&";
	char *token;
	u8 count = 0;
	bool end = false;
	token = strtok(data, "&");
	while (token != NULL && end != true) {
		//ets_uart_printf("Number:%d Token:<%s>\r\n",count,token);
		switch (count) {
		case 1:
			lastState = program.mode;
			program.mode = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 2:
			if (program.mode == 0) {
				program.on = atoi(token);
//				if(atoi(token)){
//					displayMode=81;
//				   	clearDisplayFlag = 1;
//				   	Display();
//				}else{
//					displayMode=80;
//				  	clearDisplayFlag = 1;
//				   	Display();
//				}
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				end = true;
				break;
			} else if (program.mode == 1) {
				sleep = atoi(token);
				currentSleep=sleep;
				end = true;
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				break;
			} else if (program.mode == 2) {
				programNumber = atoi(token);
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				break;
			}
			break;
		case 3:
			programMode = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 4:
			timeOn = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 5:
			timeOff = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			end = true;
			break;

		}
		count++;
		token = strtok(NULL, delimiter);
	}

	//ets_uart_printf("Comm for me is:0x%X\r\n",count);
	//ets_uart_printf("Mode: %s\r\n",token[1]);
	//Split comm by &
	//if param > 2 go to action
	//ets_uart_printf("Mode: %s\r\n",token);
	ets_uart_printf("Mode:%d\r\n", program.mode);
	switch (program.mode) {
	case 0:
		modeManual();
		break;
	case 1:
		modeSleep();
		break;
	case 2:
		modeAuto();
		break;
	}

	//postStatusWan
	//mqttPublishStatus();
	program_status(arg,"");
}

static void ICACHE_FLASH_ATTR
info_status(void *arg){
	ets_uart_printf("Respond POST INFO...\n");
	char respond[150];
	u8 numberProg=1;
	//Search for first  active program or current Program On
	 if (programNumberPower!=0){
		 numberProg=programNumberPower;
	 }else{
		//Search for first active program
			u8 i=0;
			for (i=0;i<9;i++)
			{
			  if(programer[i].mode==1){
				  numberProg=i+1;
				  break;
			  }
			}
	 }
	sint8 rssi;
	char dateTimeNow[25];
	u16 timeOnInfo=programer[numberProg-1].timeOn;
	u16 timeOffInfo=programer[numberProg-1].timeOff;
	ets_sprintf(dateTimeNow,"%d/%d/%d %d:%d:%d",year,month,date,hours,minutes,seconds);
	//ets_uart_printf("Respond POST INFO DATE_TIME: %s\r\n", dateTimeNow);
	rssi=wifi_station_get_rssi();
	ets_sprintf(respond, "info=%s&%s&%s&%d&%d&%d&%d&%d&%d&%s&%s&%s&%d",
			ok,
			dateTimeNow,
			temperature,
			status.mqttOK,
			numberProg,
			timeOnInfo,
			timeOffInfo,
			program.mode,
			program.on,
			ipadrchar,
			wan_ssid,
			wan_pwd,
			rssi);
		//lan_pwd, lan_type,   wifiStatus.connect,
		//ipadrchar,rssi,lan_ssid,);
		//ets_uart_printf("Respond lan_status -->: %s\r\n", respond);
		espconn_sent(arg, respond, os_strlen(respond));

}

static void ICACHE_FLASH_ATTR
respond_lan(void *arg) {
	char respond[100];
	sint8 rssi =wifi_station_get_rssi();
	ets_sprintf(respond, "%sstatus=&%s&%s&%d&%d&%s&%s&%d&%d&%s&%d",ok,
			lan_ssid,
			lan_pwd,
			lan_type,
			program.wifiLan,
			wan_ssid,
			wan_pwd,
			program.wifiWan,
			wifiStatus.connect,
			ipadrchar,
			rssi);
	//ets_uart_printf("Respond lan_status -->: %s\r\n", respond);
	espconn_sent(arg, respond, os_strlen(respond));

}

static void ICACHE_FLASH_ATTR
respond_lan_date(void *arg) {
	char respond[80];
	//ets_uart_printf("Lan Date Subrotine\r\n");
	//Load date from timekeeper
	ReadDS1307();
	//ets_uart_printf("Lan Date Subrotine read DS1307\r\n");
	ets_sprintf(respond, "%sdate=&%d&%d&%d&%d&%d&%d&%d&\n", ok, year, month,
			date, hours, minutes, seconds, days);
	//ets_uart_printf("Respond lan_date -->: %s\r\n len:%d\r\n", respond,strlen(respond));
	espconn_sent(arg, respond, os_strlen(respond));
}

static void ICACHE_FLASH_ATTR
respond_lan_set_date(void *arg, char *setdate) {

	//ets_uart_printf("Set Date: %s\r\n", setdate);
	char *token;
	u8 count = 0;
	u8 set_year;
	u8 set_month;
	u8 set_date;
	u8 set_hour;
	u8 set_minute;
	u8 set_second;
	u8 set_day;
	int data = 0;
	token = strtok(setdate, "&");
	while (token != NULL) {
		data = atoi(token);
		//ets_uart_printf("Split date: %d \r\n",data);
		switch (count) {
		case 1:
			set_year = data;
			break;
		case 2:
			set_month = data;
			break;
		case 3:
			set_date = data;
			break;
		case 4:
			set_hour = data;
			break;
		case 5:
			set_minute = data;
			break;
		case 6:
			set_second = data;
			break;
		case 7:
			set_day = data++;
			break;
		}
		count++;
		token = strtok(NULL, "&");
	}
	//ets_uart_printf("Set date&time:%d/%d/%d %d:%d:%d %d \r\n", set_year,set_month, set_date, set_hour, set_minute, set_second, set_day);
	SetTimeDate(set_second, set_minute, set_hour, set_day, set_date, set_month,
			set_year);
	respond_lan_date(arg);
}

static void ICACHE_FLASH_ATTR
respond_ok(void *arg,char *body) {
	  //if(!body)
	char *buffer;
	buffer=(char*)os_malloc(strlen(ok)+strlen(body)+2);
	//char respond[100];
	ets_sprintf(buffer, "%s%s", ok,body);
	//ets_uart_printf("Respond OK: <%s>", buffer);
	espconn_sent(arg, buffer, os_strlen(buffer));
	os_free(buffer);
}

void ICACHE_FLASH_ATTR
respond_for_lan_scanwifi() {
	status.tcpscaned = 0;
	wifiStatus.scaned = 0;
	//ets_uart_printf("Respond wifi scan : <%s>\n\r",(char*)scan_wifi);
	espconn_sent((struct espconn*) esp_conn_temp, (char*) scan_wifi,
	os_strlen((char*) scan_wifi));

}

static void ICACHE_FLASH_ATTR
respond_lan_scanwifi(void *arg) {
	strcat((char*) scan_wifi, ok);
	//Need to save arg for future responding
	//struct  espconn *esp_conn_temp;
	memcpy(&esp_conn_temp, &arg, sizeof(arg));
	wifiStatus.scan=0;
	WifiStationScan();

	//Need to wait for scan !!! but this will stop system make call back function to put result... make semafor for it...
	status.tcpscaned = 0;
	//ets_sprintf(respond,"%s scanwifi=&%s",scan_wifi);
	// ets_uart_printf("Respond scan Wifi -->: <%s>\r\n",scan_wifi);
	//espconn_sent(arg, respond, os_strlen(respond));
	return;
}

 static ICACHE_FLASH_ATTR
respond_lan_mqttServerRead(void *arg) {
    char str[150];
    ets_sprintf(str,"mqttserver=%d&host=%s&port=%d&user=%s&pwd=%s&rtopic=%s&wtopic=%s",
    		status.mqttOK,
    		mqtt_Server.mqttHost,
    		mqtt_Server.mqttPort,
			mqtt_Server.mqttUser,
			mqtt_Server.mqttPassword,
			mqtt_Server.mqttTopicRead,
			mqtt_Server.mqttTopicWrite);
	respond_ok(arg,str);
	return;
}

 static ICACHE_FLASH_ATTR
respond_lan_mqttServerTest(void *arg,char *setdata){

	  //split data by & and set mqtt parameter
	 u8 count=0;
	 u8 len;
	 char *token = strtok(setdata, "&");
	 while (token != NULL && count < 7) {
	 		//data = atoi(token);
	 		//ets_uart_printf("Split date from save mqttServer: %s \r\n",token);
	 		len=strlen(token);
	 		switch (count) {
	 		case 1:
	 			 if(len<31){
	 			ets_sprintf(mqtt_Server.mqttHost,"%s",token);
	 			//strncpy(mqtt_Server.mqttHost,token,len);
	 			 }
	 			break;
	 		case 2:
	 			mqtt_Server.mqttPort=atoi(token);
	 			break;
	 		case 3:
	 			if(len<15){
	 				ets_sprintf(mqtt_Server.mqttUser,"%s",token);
	 			//strncpy(mqtt_Server.mqttUser,token,len);
	 			}
	 			break;
	 		case 4:
	 			if(len<15){
	 				ets_sprintf(mqtt_Server.mqttPassword,"%s",token);
	 			//strncpy(mqtt_Server.mqttPassword,token,len);
	 			}
	 			break;
	 		case 5:
	 			if(len<31){
	 				ets_sprintf(mqtt_Server.mqttTopicRead,"%s",token);
	 			//strncpy(mqtt_Server.mqttTopicRead,token,len);
	 			}
	 			break;
	 		case 6:
	 			if(len<31){
	 				ets_sprintf(mqtt_Server.mqttTopicWrite,"%s",token);
	 			//strncpy(mqtt_Server.mqttTopicWrite,token,len);
	 			}
	 			break;
	 		}
	 		count++;
	 		token = strtok(NULL, "&");
	 	}


	// respond_lan_mqttServerRead(arg);
	 if (wifiStatus.connect){
		 ets_uart_printf("Wifi is Connected :)....! \r\n");
		 //Disconect curent client and try new
		 MQTT_Disconnect(&mqttClient);
		 status.mqtt=0;
		 status.mqttOK=0;
		 status.mqttCount=0;
		 respond_lan_mqttServerRead(arg);

		 //reboot=3;

//		 if(status.mqttOK==0){
//			 //status.mqtt=0;
//			 respond_lan_mqttServerRead(arg);
//		 }
//		 else {
//			 status.mqttOK=0;
//			 respond_lan_mqttServerRead(arg);
//			 reboot=3;
//			// wifi_station_disconnect();
//			 //status.mqtt=0;
//		 }

	 }
	 else{
		 ets_uart_printf("Wifi is not Connected :(..! \r\n");
		 MQTT_Disconnect(&mqttClient);
		 status.mqttOK=0;
		 respond_lan_mqttServerRead(arg);
	 }
	 return;
 }

static ICACHE_FLASH_ATTR
respond_lan_mqttServerSave(void *arg){
	 if(eeprom_writeMqttServer(mqtt_Server)){
		 respond_ok(arg,"Mqtt Server Saved!");
	 }
	 else
	 {
		 respond_ok(arg,"Mqtt Server Not Saved :(... !");
	 }

 }

void ICACHE_FLASH_ATTR
respond_lan_update(void *arg, char *lanform){
	//ets_uart_printf("Update Call me from lan lanForm: <%s> \n\r",lanform);
    //if name not rom1.bin close connection
	struct espconn *conn=arg;
	//Read rboot config ....
	rboot_config config;
	config = rboot_read_config();
	if(config.current_rom==0){
	if ((strstr(lanform,"name=user2.1024.new.2.bin")== NULL)){
		//ets_uart_printf("Wrong file name .... \n");
		respond_ok(arg,"Wrong file name .... \n");
		return;
	   }
	status_write=rboot_write_init(START_ADDRESS_ROM_2);
	} else if (config.current_rom==1){
		if ((strstr(lanform,"name=user1.1024.new.2.bin")== NULL)){
		//ets_uart_printf("Wrong file name .... \n");
		respond_ok(arg,"Wrong file name .... \n");
		return;
	  }
	 status_write=rboot_write_init(START_ADDRESS_ROM_1);
	}

	char  *check_sum=strstr(lanform, "sum=");
	if(check_sum != NULL){
		check_sum+=4;
		update_check_sum=atoi(check_sum);
		ets_uart_printf("Check Sum: 0%X \n",update_check_sum);
	}

	post_len=0;
	//if(config.current_rom==0) status_write=rboot_write_init(START_ADDRESS_ROM_2);
	//if(config.current_rom==1) status_write=rboot_write_init(START_ADDRESS_ROM_1);
	//config.len[config.current_rom]=len_post_content;
	//For Check in Other trafic that is valid POST data
	//os_printf("I am here ! 2");
	//u32 start_addr=START_ADDRESS_ROM_2;
	post_remote_port=conn->proto.tcp->remote_port;
	post_remoteip_address=conn->proto.tcp->remote_ip;
	//Stop Main Thread
	os_timer_disarm(&mainTimer);
	os_timer_disarm(&menuTimer);

//	ClearDisplay();
//	if(!oled.on){oled_on();}
//	displayString("    D o w n L o a d", 1, 1);
//	displayString("  F i r m w a r e ...", 2, 1);

	//displayString("  D o w n l o a d", 5, 1);
	//Next recived data from tcp POST body
	 return;

}

void ICACHE_FLASH_ATTR
respond_lan_boot( void *arg,char *data){
	rboot_config config;
	config = rboot_read_config();
	char respond[80];
	ets_sprintf(respond, "%sboot=&%d&version=&%d \r\n", ok, config.current_rom, config.version);
    //art_printf("Respond lan_date -->: %s\r\n len:%d\r\n", respond,strlen(respond));
	espconn_sent(arg, respond, os_strlen(respond));
}

static void ICACHE_FLASH_ATTR
respond_lan_form(void *arg, char *lanform) {
	//retrun status
	//ets_uart_printf("Call me from lan lanForm: <<%s>>\n\r",lanform);
	char delimiter[5] = "=&? ";
	char *token;
	char *http_lan_ssid;
	char *http_lan_pwd;
	u8 http_lan_type;
	unsigned count = 0;
	u8 len;
	token = strtok(lanform, delimiter);
	while (token != NULL && count < 7) {
		//ets_uart_printf("LANFORM: %s \r\n ", token);
		switch (count) {
		case 1:
			len = strlen(token);
			if (len < 33 && len > 0) {
				ets_sprintf(lan_ssid, "%s", token);
				//strncpy(lan_ssid,token,len);
				//lan_ssid[len]=0;
			} else {
				lan_ssid[0] = 0;
			}
			//ets_uart_printf("Lan ssid:%s\n\r ",token);
			//ets_uart_printf("Lan ssid:%s\n\r ",lan_ssid);
			break;
		case 2:
			len = strlen(token);
			if (len < 17 && len > 0) {
				ets_sprintf(lan_pwd, "%s", token);
				//strncpy(lan_pwd,token,len);
				//lan_pwd[len]=0;
			} else {
				lan_pwd[0] = 0;
			}
			// ets_uart_printf("Lan token <pwd>:<%s>\n\r",token);
			// ets_uart_printf("Lan lan_pwd <pwd>:<%s>\n\r",lan_pwd);
			break;
		case 3:
			len = strlen(token);
			if (len < 2 && len > 0) {
				lan_type = atoi(token);
			} else {
				lan_type = 0;
			}
			//ets_uart_printf("Lan wifi type:%s\n\r", token);
			//ets_uart_printf("Lan wifi type:%d\n\r", lan_type);
			break;
		case 4:
			len=strlen(token);
			if(len>0 && len<2){
			   //char chk[2];
			//	ets_sprintf(chk,"%s",token);
				if(token[0]=='0'){
					program.wifiLan=0;
				}else{
					program.wifiLan=1;
				}
				eeprom_saveStatus();
			}
			break;
		}

		count++;
		token = strtok(NULL, delimiter);
	}

	 respond_lan(arg);
	 if(program.wifiLan==0){
		 InitWifi();
		 return;
	 }
	if (lan_ssid != "" && lan_pwd != "") {
		//Save to EEPROM
		eepromWriteData(EEPROM_LAN_SSID, lan_ssid);
		eepromWriteData(EEPROM_LAN_PWD, lan_pwd);
		eepromWriteByte(EEPROM_LAN_TYPE,lan_type);
		//Set this parameter to Wifi Server and restart it !!
		program.wifiServer=0;
		InitWifi();
	}
}

void ICACHE_FLASH_ATTR
respond_wan_form(void *arg, char *wanform) {
	ets_uart_printf("Call wan form\n\r");
	//ets_uart_printf("Call wan form:<%s>\n\r",wanform);
	char delimiter[5] = "=&? ";
	char *token;
	//char *http_lan_ssid;
	//char *http_lan_pwd;
	unsigned count = 0;
	u8 len;
	token = strtok(wanform, delimiter);
	//ets_uart_printf("Call wan form token:<%s>\n\r",token);
	while (token != NULL && count < 4) {
		switch (count) {
		case 1:
			len = strlen(token);
			if (len > 0 && len < 32) {
				ets_sprintf(wan_ssid, "%s", token);
				eepromWriteData(EEPROM_WAN_SSID, token);
				//ets_uart_printf("Wan ssid:<%s>\n\r", wan_ssid);
			}
			break;
		case 2:
			len = strlen(token);
			if (len > 0 && len < 24) {
				ets_sprintf(wan_pwd, "%s", token);
				eepromWriteData(EEPROM_WAN_PWD, token);
				//ets_uart_printf("Wan pwd:<%s>\n\r", wan_pwd);
			}
			break;
		case 3:
			len=strlen(token);
			if(len>0 && len<2){
				//char wanOnOff[1];
				//ets_sprintf(wanOnOff,"%s",token);
				ets_uart_printf("WiFi is:<%s>\n\r",token);
				if(token[0]=='0'){
					program.wifiWan=0;
					ets_uart_printf("WiFi is Off\n\r");
				}else{
					program.wifiWan=1;
					ets_uart_printf("WiFi is On\n\r");
				}
				eeprom_saveStatus();
			}
			break;
		}
		count++;
		token = strtok(NULL, delimiter);
	}

	respond_lan(arg);
	InitWifi();
}

static void ICACHE_FLASH_ATTR
lan_connection(void *arg, char *pusrdata) {
	//Call me from LAN
	//ets_uart_printf("TCP Data: <%s> \n",pusrdata);

	char *stat = strstr(pusrdata, "lan_status?");
	char *scanwifi = strstr(pusrdata, "scanwifi?");
	char *date = strstr(pusrdata, "date?");
	char *setdate = strstr(pusrdata, "date=");
	char *boot= strstr(pusrdata, "boot?");
	char *changeBoot= strstr(pusrdata,"changeBoot?");
	char *postInfo=strstr(pusrdata,"POST /info.html/info=?");
	char *postProramStatus=strstr(pusrdata,"POST /program.html/status");
	char *postCommand=strstr(pusrdata,"POST /program.html/comm");

	//int  update =strncmp(pusrdata,"POST /update",12);
	if (postCommand!=NULL){
		//return info status like Display
		http_command(arg,pusrdata);
		return;
	} else if (postInfo!=NULL){
		//return info status like Display
		info_status(arg);
		return;
	}
	else if (postProramStatus!=NULL){
			//return info status like Display
		program_status(arg,pusrdata);
			return;
		}

	else if (stat != NULL) {
		//retrun status
		// ets_uart_printf("Call me from lan Staus: %s",stat);
		respond_lan(arg);
		return;
	}

	else if (scanwifi != NULL) {
		ets_uart_printf("Call me from Scan Wifi..\n\r");
		respond_lan_scanwifi(arg);
		return;
	}

	else if (date != NULL) {
		ets_uart_printf("Call me from Lan Date?\n\r");
		respond_lan_date(arg);
		return;
	}

	else if (setdate != NULL) {
		ets_uart_printf("Call me from Lan Set Date?\n\r");
		respond_lan_set_date(arg, setdate);
		return;
	}

	else if (boot != NULL){
	 ets_uart_printf("Call me from Lan Boot?\n\r");
	 respond_lan_boot(arg, setdate);
	 return;
	}
	else if(changeBoot != NULL){
		os_timer_disarm(&mainTimer);
		os_timer_disarm(&menuTimer);
		ets_uart_printf("Changing Boot ..... \r\n \r\n");
		respond_ok(arg,"Changing Boot ..... \r\n \r\n");
		ClearDisplay();
		displayString("   C h a n g e ", 3, 1);
		displayString("     B O O T ", 4, 1);
		reboot=1;
		return;

	}
	else if(strstr(pusrdata,"mqttServer?")!=0 ){
	ets_uart_printf("Call me from mqttServer? \n\r");
	respond_lan_mqttServerRead(arg);
	return;
	}
	else if(strstr(pusrdata,"mqttServer=")!=0 ){
		//ets_uart_printf("Call me from mqttServer=%s \n\r",pusrdata);
		respond_lan_mqttServerTest(arg,pusrdata);
		return;
	}
	else if(strstr(pusrdata,"mqttServerSave?")!=0 ){
	 //ets_uart_printf("Call me from mqttServer=%s \n\r",pusrdata);
	    respond_lan_mqttServerSave(arg);
		return;
	}
	else if(strstr(pusrdata,"esp8266Reset?")!=0 ){
		 //ets_uart_printf("Call me from mqttServer=%s \n\r",pusrdata);
		 respond_ok(arg,"ESP rebooted ....");
		 reboot=2;
		return;
		}



	char *lanform = strstr(pusrdata, "lanform=");
	if (lanform != NULL) {
		//retrun status
		//ets_uart_printf("Call me from lan lanForm: %s\n\r",lanform);
		respond_lan_form(arg, lanform);
		return;
	}

	char *wanform = strstr(pusrdata, "wanform=");
	if (wanform != NULL) {
		respond_wan_form(arg, wanform);
		return;
	}

	char *gethttpProgram = strstr(pusrdata, "GET /program.html");
		if (gethttpProgram != NULL) {


			//ets_uart_printf("Respont from lan default get / http/1.1: <%s> \n\r",pusrdata);
			ets_uart_printf("Respont from lan default GET /program.html \n");
			uint16 len=strlen_P(html_program);
			//ets_uart_printf("Len of Lan String in Flash is: <0x%X> \n",len);
			//DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
	        if(len >0){
	          //dataHTML =(char*) os_zalloc(len + 1);
	          //memcpy_P(dataHTML,html_lan,len);
	          //dataHTML[len]=0;
	         //	system_soft_wdt_feed();
	        //	DEBUG("Len of html_lan: %d \r\n",len);
	        	if(len%4!=0){
	        		len-=len%4;
	        	}
	        //	DEBUG("Len of html_lan: %d \r\n",len);
	        	espconn_sent(arg,(u8*)html_program, len);
	//        	status_sent_flash.arg=arg;
	//        	status_sent_flash.src=html_lan;
	//        	status_sent_flash.len=len;
	//        	status.tcp_sent_flash=1;
	//        	status.tcp_get_http=0;
			    //tcp_sent_flash(status_sent_flash);
	        } else {
		    	espconn_sent(arg,ok,strlen(ok));
		    }
	        //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
			//os_free(dataHTML);
		    //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
	        return;
		}

		char *gethttpInfo = strstr(pusrdata, "GET /info.html");
				if (gethttpInfo != NULL) {


					//ets_uart_printf("Respont from lan default get / http/1.1: <%s> \n\r",pusrdata);
					ets_uart_printf("Respont from lan default GET /info.html \n");
					uint16 len=strlen_P(html_info);
					//ets_uart_printf("Len of Lan String in Flash is: <0x%X> \n",len);
					//DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
			        if(len >0){
			          //dataHTML =(char*) os_zalloc(len + 1);
			          //memcpy_P(dataHTML,html_lan,len);
			          //dataHTML[len]=0;
			         //	system_soft_wdt_feed();
			        //	DEBUG("Len of html_lan: %d \r\n",len);
			        	if(len%4!=0){
			        		len-=len%4;
			        	}
			        //	DEBUG("Len of html_lan: %d \r\n",len);
			        	espconn_sent(arg,(u8*)html_info, len);
			//        	status_sent_flash.arg=arg;
			//        	status_sent_flash.src=html_lan;
			//        	status_sent_flash.len=len;
			//        	status.tcp_sent_flash=1;
			//        	status.tcp_get_http=0;
					    //tcp_sent_flash(status_sent_flash);
			        } else {
				    	espconn_sent(arg,ok,strlen(ok));
				    }
			        //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
					//os_free(dataHTML);
				    //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
			        return;
				}

	char *gethttp = strstr(pusrdata, "GET / HTTP/1.1");
	if (gethttp != NULL) {


		//ets_uart_printf("Respont from lan default get / http/1.1: <%s> \n\r",pusrdata);
		ets_uart_printf("Respont from lan default get / http/1.1 \n");



		uint16 len=strlen_P(html_lan);
		//ets_uart_printf("Len of Lan String in Flash is: <0x%X> \n",len);
		//DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
        if(len >0){
          //dataHTML =(char*) os_zalloc(len + 1);
          //memcpy_P(dataHTML,html_lan,len);
          //dataHTML[len]=0;
         //	system_soft_wdt_feed();
        //	DEBUG("Len of html_lan: %d \r\n",len);
        	if(len%4!=0){
        		len-=len%4;
        	}
        //	DEBUG("Len of html_lan: %d \r\n",len);
        	espconn_sent(arg,(u8*)html_lan, len);
//        	status_sent_flash.arg=arg;
//        	status_sent_flash.src=html_lan;
//        	status_sent_flash.len=len;
//        	status.tcp_sent_flash=1;
//        	status.tcp_get_http=0;
		    //tcp_sent_flash(status_sent_flash);
        } else {
	    	espconn_sent(arg,ok,strlen(ok));
	    }
        //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
		//os_free(dataHTML);
	    //DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
        return;
	}

	respond_ok(arg,"");
	//ets_uart_printf("Respont from lan default: <%s> \n\r",pusrdata);
	//espconn_sent(arg, (char*) html_lan, os_strlen(html_lan));
    return;
}

static void ICACHE_FLASH_ATTR
tcp_server_recon_cb(void *arg, sint8 err) {
	//error occured , tcp connection broke.
	//ets_uart_printf("reconnect callback, error code %d !!! \r\n", err);
	struct espconn *pesp_conn = arg;
	//ets_uart_printf("tcp disconnect from err... \r\n");
	espconn_disconnect(pesp_conn);
}

static void ICACHE_FLASH_ATTR
tcp_server_discon_cb(void *arg) {
	//tcp disconnect successfully
	DEBUG("tcp disconnect succeed !!! \r\n");
	DEBUG("Free of RAM <0x%X> \n",system_get_free_heap_size());
	if (reboot==1) {
		displayString("   R E B O T I N G ", 4, 1);
		chaged_boot_rom();
	}
	else if (reboot==2) {
		displayString("   R E S T A R T I N G ", 4, 1);
		//GPIO_OUTPUT_SET(POWER_PIN,0);
		POWER_OFF;
		system_soft_wdt_feed();
		system_restart();
	} else if(reboot==3){
	  wifi_station_disconnect();
	  reboot=0;
	}
}

static void ICACHE_FLASH_ATTR
tcp_post_data(void *arg,char* data,unsigned short len){
	//Recive raw data from rom1.bin  Need to save in Flash
	//First just Display and Counting it
	struct espconn *conn=arg;
	char *current_ip=conn->proto.tcp->remote_ip;
	//ets_uart_printf("Tcp Post Data... \n");
	//check for the same remote ip  and port
	//ets_uart_printf("Ip 1 Compare:%s \n",post_remoteip_address);
	//ets_uart_printf("Ip 2 Compare:%s \n ",conn->proto.tcp->remote_ip,4);
	//ets_uart_printf("Ip Compare: %d \n",(post_remoteip_address[0]==current_ip[0]));
	if(post_remote_port==conn->proto.tcp->remote_port  && (post_remoteip_address[0]==current_ip[0]) &&
	(post_remoteip_address[1]==current_ip[1]) &&
	(post_remoteip_address[2]==current_ip[2]) &&
	(post_remoteip_address[3]==current_ip[3])){
	   post_len+=len;
	   //ets_uart_printf("Rom1 len: %d \n",post_len);
	  if( rboot_write_flash(&status_write,data,len)== false){
		  respond_ok(conn,"File Uploaded Error. \r\n \r\n");
		  len_post_content=0;
		  os_printf("Error write Flash....\n");
		  //ClearDisplay();
		  displayString("     E R R O R", 3, 1);
		  displayString("   U P L O A D E D", 4, 1);
		  displayString("      F I L E ", 5, 1);
		  displayTimer=DISPLAYTIMER;
		  displayMode=99;
		  os_timer_arm(&mainTimer,10,0);
		  os_timer_arm(&menuTimer,EXITTIMEUPDATE,0);
		  //Only Restart whitout Change ROM
		  //reboot=2;
		  return;
	  }
	    //Check for end of file
	   if(post_len == len_post_content){
		   //Write last bites if need
		   rboot_write_end(&status_write);
		   //Calc Control Sum
           char msg[100];
           u8 controlSum=ControlSum(len_post_content);
           ets_sprintf(msg,"File Uploaded.LRC:Error. \r\n %d --> %d \r\n \r\n",update_check_sum,controlSum);
           //close connection
		   if(controlSum!=update_check_sum){
			   //update flag Down
		    respond_ok(conn,msg);
		   }else{
		      //update flag Up
		   respond_ok(conn,"File Uploaded done.LRC:OK. \r\n \r\n");
		   }
		   //ClearDisplay();
		   displayString("     F I L E ", 3, 1);
		   displayString("   U P L O A D E D ", 4, 1);
		   displayString("       D O N E ! ", 5, 1);
		   //displayString("   F L A S H", 5, 1);

		   displayTimer=DISPLAYTIMER;
		   displayMode=99;
		   os_timer_arm(&mainTimer,10,0);
		   os_timer_arm(&menuTimer,EXITTIMEUPDATE,0);
		   //len_post_content=0;
		   //change boot rom in rboot config and reboot device..
		   //chaged_boot_rom();
		   //reboot=1;
		 }
	   }
	}

static void ICACHE_FLASH_ATTR
tcp_server_recv_cb(void *arg, char *pusrdata, unsigned short length) {
	//received some data from tcp connection

	struct espconn *pespconn = arg;
	char  *remip=pespconn->proto.tcp->remote_ip;
	//ets_uart_printf("Remote IP-->%d.%d.%d.%d:%d\n",remip[0],remip[1],remip[2],remip[3],pespconn->proto.tcp->remote_port);
	//ets_uart_printf("Remote len:%d \n",length);
	//ets_uart_printf("Tcp recv : <-- %s--> \n ", pusrdata);
	//int res=strncmp(pusrdata, "10.10.10.1" , 10);
	//ets_uart_printf("Campare string res: <0x%X> \n",res);
	//if(strncmp(pusrdata,"GET ",4)==0) //Get Header
	//else if(strncmp(pusrdata,"POST",4)==0) //Post Header

	//ets_uart_printf("Campare string result: <%d> \n ",strncmp(pusrdata, "10.10.10.1" , 10));

	//if(remip[0]==10 && remip[1]==10 && remip[2]==10)
	//{
		//DEBUG("Call me from ip address:%d.%d.%d.%d \r\n",remip[0],remip[1],remip[2],remip[3]);
		 if(strncmp(pusrdata,"GET ",4)==0){
		   ets_uart_printf("Get header \n");
		   lan_connection(arg, pusrdata);
		   return;
		 } else if (strncmp(pusrdata,"POST",4)==0){
			ets_uart_printf("Post header \n");
			//Check Content-Length"
			//ets_uart_printf("Post request: <%s> \n",pusrdata);
			char  *content_len_pos=strstr(pusrdata, "Content-Length:");
			if(content_len_pos != NULL){
				content_len_pos+=15;
			   while (content_len_pos[0] == ' ') content_len_pos++;
			   //ets_uart_printf("String Content-Lenght: %s \n",content_len_pos);
			   len_post_content=atoi(content_len_pos);
             //ets_uart_printf("Post Content len: %d \n",len_post_content);
             if(len_post_content>0){
            	  	//Will sending update file
                   ets_uart_printf("Send Update ...\n\r");
                   respond_lan_update(arg,pusrdata);
                   return;
                }
            }
			lan_connection(arg, pusrdata); //if light of Post header is zero no data will be transmit
			return;
		 }
		 //ets_uart_printf("Other data... \n");
		 tcp_post_data(arg,pusrdata,length);
		  //Other TCP Post data
		return;
	}

static void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg) {
	struct espconn *pesp_conn = arg;
	ets_uart_printf("tcp_server_listen !!! \r\n");
	//espconn_regist_connectcb(pesp_conn,tcp_server_connect_cb);
	espconn_regist_recvcb(pesp_conn, tcp_server_recv_cb);
	espconn_regist_reconcb(pesp_conn, tcp_server_recon_cb);
	espconn_regist_disconcb(pesp_conn, tcp_server_discon_cb);
	espconn_regist_sentcb(pesp_conn, tcp_server_sent_cb);
	//tcp_server_multi_send();
}

void ICACHE_FLASH_ATTR
TcpInit() {
	esp_conn.type = ESPCONN_TCP;
	esp_conn.state = ESPCONN_NONE;
	esp_conn.proto.tcp = &esptcp;
	esp_conn.proto.tcp->local_port = LOCAL_PORT;
	//espconn_regist_time(&esp_conn,DISCON_TIME,0);
	sint8 ret1 = espconn_regist_connectcb(&esp_conn, tcp_server_listen);
	//Start server
	sint8 ret = espconn_accept(&esp_conn);
	ets_uart_printf("\nespconn_accept %d , %d!!! \r\n", ret, ret1);
}


void ICACHE_FLASH_ATTR
mqttPublishStatus(){
	char respond[100];
	 sint8 rssi;
	 rssi=wifi_station_get_rssi();
//	 if(program.on){
//		 displayMode=81;
//		 clearDisplayFlag = 1;
//		 Display();
//	 } else{
//		 displayMode=80;
//		 clearDisplayFlag = 1;
//		 Display();
//	 }
	if (program.mode == 0) {
			ets_sprintf(respond, "status&%d&%d&%s&%d&\n",program.mode,
					program.on, temperature,rssi);
		} else if (program.mode == 1) {
			ets_sprintf(respond, "status&%d&%d&%d&%s&%d&\n",program.mode,
					program.on, sleep, temperature,rssi);
		} else if (program.mode == 2) {
			programMode = programer[programNumber - 1].mode;
			timeOn = programer[programNumber - 1].timeOn;
			timeOff = programer[programNumber - 1].timeOff;
			ets_sprintf(respond, "status&%d&%d&%d&%d&%d&%d&%d&%s&%d&\n",program.mode, program.on, programNumber, programMode, timeOn,
			timeOff, programNumberPower, temperature,rssi);

		}
		MQTT_Publish(&mqttClient, mqtt_Server.mqttTopicWrite,respond, strlen(respond),0, 0);
		return;
}

void ICACHE_FLASH_ATTR
mqttConnectedCb(uint32_t *args) {

	MQTT_Client* client = (MQTT_Client*) args;
	ets_uart_printf("MQTT: Connected\r\n");
	MQTT_Subscribe(client, mqtt_Server.mqttTopicRead, 0);
	MQTT_Publish(client, mqtt_Server.mqttTopicWrite, "hello", 5, 0, 0);
	mqttPublishStatus();
	clearDisplayFlag=1;
	//DisplayMsg("Mqtt Server Connected :)!!!");
	status.mqttOK=1;

}

void ICACHE_FLASH_ATTR
mqttDisconnectedCb(uint32_t *args){
	MQTT_Client* client = (MQTT_Client*)args;
	ets_uart_printf("MQTT: Disconnected...\r\n");
	mqtt_client_delete(client);
	status.mqtt=0;
	status.mqttOK=0;
	clearDisplayFlag=1;
	//client->mqtt_state.in_buffer;
	//status.

	//DisplayMsg("Mqtt Server Disconnected");
}

void ICACHE_FLASH_ATTR
mqttPublishedCb(uint32_t *args){
	MQTT_Client* client = (MQTT_Client*)args;
	ets_uart_printf("MQTT: Published\r\n");
}

void ICACHE_FLASH_ATTR
mqttStatus(MQTT_Client* client, char* dataBuf) {
	char delimiter[2] = "&";
	char *token;
	char respond[100];
	u8 count = 0;
	//Check For status?&2&1
	bool numberOK = false;
	token = strtok(dataBuf, delimiter);
	while (token != NULL && count < 5) {
		if (count == 2) {   //Second position in status&?&2
			//ets_uart_printf("Second position in status %d \n",atoi(token));
			if (atoi(token) == 2) {
				numberOK = true;
			}
		} else if (count == 3) { //Third position in status
			//ets_uart_printf("Third position in status %d \n",atoi(token));
			if (numberOK) {
				programNumber = atoi(token);
			} else
				programNumber = 1;
		}
		count++;
		token = strtok(NULL, delimiter);
	}

	//request for Status programm
	if (numberOK) {  //Read program Data

		programMode = programer[programNumber - 1].mode;
		timeOn = programer[programNumber - 1].timeOn;
		timeOff = programer[programNumber - 1].timeOff;
		ets_sprintf(respond, "program&%d&%d&%d&%d&%d&%s",programNumber,
				programMode, timeOn, timeOff, programNumberPower, temperature);
		MQTT_Publish(client,mqtt_Server.mqttTopicWrite,respond, strlen(respond),0, 0);
		return;
	}
	mqttPublishStatus();
	return;
}

void ICACHE_FLASH_ATTR
mqttCommand(char* dataBuf) {
	DEBUG("Mqtt Command received ... \r\n");
	MQTT_Client* client;
	char delimiter[2] = "&";
	char *token;
	u8 count = 0;
	bool end = false;
	token = strtok(dataBuf, "&");
	while (token != NULL && end != true) {
		//ets_uart_printf("Number:%d Token:<%s>\r\n",count,token);
		switch (count) {
		case 1:
			lastState = program.mode;
			program.mode = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 2:
			if (program.mode == 0) {
				program.on = atoi(token);
//				if(atoi(token)){
//					displayMode=81;
//				   	clearDisplayFlag = 1;
//				   	Display();
//				}else{
//					displayMode=80;
//				  	clearDisplayFlag = 1;
//				   	Display();
//				}
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				end = true;
				break;
			} else if (program.mode == 1) {
				sleep = atoi(token);
				currentSleep=sleep;
				end = true;
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				break;
			} else if (program.mode == 2) {
				programNumber = atoi(token);
				//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
				break;
			}
			break;
		case 3:
			programMode = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 4:
			timeOn = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			break;
		case 5:
			timeOff = atoi(token);
			//ets_uart_printf("Number:%d Token:<%s>\r\n", count, token);
			end = true;
			break;

		}
		count++;
		token = strtok(NULL, delimiter);
	}

	//ets_uart_printf("Comm for me is:0x%X\r\n",count);
	//ets_uart_printf("Mode: %s\r\n",token[1]);
	//Split comm by &
	//if param > 2 go to action
	//ets_uart_printf("Mode: %s\r\n",token);
	ets_uart_printf("Mode:%d\r\n", program.mode);
	switch (program.mode) {
	case 0:
		modeManual();
		break;
	case 1:
		modeSleep();
		break;
	case 2:
		modeAuto();
		break;
	}

	//postStatusWan
	mqttPublishStatus();
}

void ICACHE_FLASH_ATTR
mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len,
		const char *data, uint32_t data_len) {
	char *topicBuf = (char*) os_zalloc(topic_len + 1), *dataBuf =
			(char*) os_zalloc(data_len + 1);

	MQTT_Client* client = (MQTT_Client*) args;

	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;

	os_memcpy(dataBuf, data, data_len);
	dataBuf[data_len] = 0;


	//ets_uart_printf("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
	//MQTT_Publish(client, "/homelab/respond", "OK", 2, 0, 0);
	MQTT_Publish(client, mqtt_Server.mqttTopicWrite, "OK", 2, 0, 0);
	os_free(topicBuf);

	char *stat = strstr(dataBuf, "status");
	if (stat != NULL) {
		mqttStatus(client, dataBuf);
		os_free(dataBuf);
		return;
	}

	char *comm = strstr(dataBuf, "comm");
	if (comm != NULL) {
		mqttCommand(dataBuf);
		os_free(dataBuf);
		return;
	}

	os_free(dataBuf);
}

void ICACHE_FLASH_ATTR
MqttInit() {
	//mqtt_Server = eeprom_readMqttServer();
    //mqtt_client_delete(&mqttClient);
	ets_uart_printf("Try Init Mqtt ..... \r\n");
	//INFO ("Init Mqtt ..... \r\n");
    //MQTT_InitConnection(&mqttClient,MQTT_HOST,MQTT_PORT,0);
    MQTT_InitConnection(&mqttClient,mqtt_Server.mqttHost,mqtt_Server.mqttPort,0);
    //MQTT_InitClient(&mqttClient, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, MQTT_KEEPALIVE, 0);
    MQTT_InitClient(&mqttClient, MQTT_CLIENT_ID, mqtt_Server.mqttUser, mqtt_Server.mqttPassword, MQTT_KEEPALIVE, 0);

    //MQTT_InitClient(&mqttClient, "client_id", "user", "pass", 120, 1);

  	//MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
   	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
   	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
    MQTT_OnPublished(&mqttClient, mqttPublishedCb);
   	MQTT_OnData(&mqttClient, mqttDataCb);


	if(status.gotIP){
	  ets_uart_printf("I have got IP now Init Mqtt ..... \r\n");
	  MQTT_Connect(&mqttClient);
	} else {
	  MQTT_Disconnect(&mqttClient);
	  ets_uart_printf("Mqtt disconected ..... \r\n");
	  status.mqtt=0;
	}

}



