/*
 * eeprom.c
 *
 *  Created on: 3.02.2017 �.
 *      Author: admin
 */
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "user_config.h"
#include "driver/eeprom.h"
#include "driver/i2c_master.h"

//extern mqttserver mqtt_Server;


static ICACHE_FLASH_ATTR init_I2C() {
	if (!status.i2c) {
		i2c_master_gpio_init();
		//i2c_master_init();
		ets_uart_printf("I2C Init... !\n\r");
		status.i2c = 1;
	}
	error = 0;
}

bool ICACHE_FLASH_ATTR I2C_WA(u8 address) {
	i2c_master_writeByte(address);
	if (!i2c_master_checkAck()) {
		error = 1;
		//ets_uart_printf("\n\r EERPOM Error from device address:0x%X",address);
		i2c_master_stop();
		return false;
	}
	//ets_uart_printf("\n\r EERPOM Ack from device address:0x%X",address);
	return true;
}

static bool ICACHE_FLASH_ATTR I2C_RA(u8 address) {
	i2c_master_writeByte(address | 0x1); //Set last bit - read
	if (!i2c_master_checkAck()) {
		error = 2;
		ets_uart_printf("\n\r EERPOM Error write read address:0x%X",
				address | 0x1);
		i2c_master_stop();
		return false;
	}
	//ets_uart_printf("\n\r EERPOM Write read address ok:0x%X",address|0x1);
	return true;
}

static bool ICACHE_FLASH_ATTR I2C_WD(u8 data) {
	i2c_master_writeByte(data);
	if (!i2c_master_checkAck()) {
		error = 3;
		ets_uart_printf("\n\r EERPOM Error write data:0x%X", data);
		i2c_master_stop();
		return false;
	}
	//ets_uart_printf("\n\r EEPROM Write data ok:0x%X",data);
	return true;
}

u8 ICACHE_FLASH_ATTR eepromReadByte(u16 address) {
	init_I2C();
	u8 data;
	u8 device_address = EEPROM_ADDRESS;
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	status.i2creaderror = 0;
	//Select addressin EEPROM;
	i2c_master_start();
	if (!I2C_WA(device_address)) {
		status.i2creaderror = 1;
		return false;
	}
	if (!I2C_WD(addrh)) {
		status.i2creaderror = 1;
		return false;
	}
	if (!I2C_WD(addrl)) {
		status.i2creaderror = 1;
		return false;
	}
	//i2c_master_stop();
	//Second Start and Read from address
	i2c_master_start();
	if (!I2C_RA(device_address)) {
		status.i2creaderror = 1;
		return false;
	}
	data = i2c_master_readByte();
	i2c_master_send_nack();
	i2c_master_stop();
	return data;
}

bool ICACHE_FLASH_ATTR eepromWriteByte(u16 address, u8 data) {
	//Select addressin EEPROM;
	u8 device_address = EEPROM_ADDRESS;
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	i2c_master_start();
	if (!I2C_WA(device_address))
		return false;
	if (!I2C_WD(addrh))
		return false;
	if (!I2C_WD(addrl))
		return false;
	if (!I2C_WD(data))
		return false;
	i2c_master_stop();
	system_soft_wdt_feed();
	os_delay_us(10000);
	return true;
}

bool eeprom_readStaus() {
	u8 eeprom = eepromReadByte(EEPROM_STATUS);
	if (!status.i2creaderror) {
		*(u8*) (&program) = (u8) eeprom;
		//ets_uart_printf("\n\r Read program from eerpom:0x%X",program);
	}
	return true;
}


bool ICACHE_FLASH_ATTR eeprom_read_themperature(){
	u8 eeprom = eepromReadByte(EEPROM_THEMPERATURE);
	if (!status.i2creaderror) {
		if(eeprom>41)eeprom=41;
		*(u8*) (&themperatureSet) = (u8) eeprom;
		//ets_uart_printf("\n\r Read program from eerpom:0x%X",program);
	}
	return true; 
}


bool ICACHE_FLASH_ATTR eeprom_write_themperature() {
	eepromWriteByte(EEPROM_THEMPERATURE, *(u8*) (&themperatureSet));
	return true;
}


////bool ICACHE_FLASH_ATTR eeprom_readTimer() {
////	u8 eeprom;
////	eeprom = eepromReadByte(EEPROM_HOUR_ON);
////	if (!status.i2creaderror) {
////		*(u8*) (&on_hour) = (u8) eeprom;
////		if (on_hour > 23)
////			on_hour = 0;
////		//ets_uart_printf("\n\r Read program from eerpom:0x%X",eeprom);
////	}
//
////	eeprom = eepromReadByte(EEPROM_MINUTE_ON);
////	if (!status.i2creaderror) {
////		*(u8*) (&on_minute) = (u8) eeprom;
////		if (on_minute > 59)
////			on_minute = 0;
////		//ets_uart_printf("\n\r Read program from eerpom:0x%X",eeprom);
////	}
//
////	eeprom = eepromReadByte(EEPROM_MINUTE_OFF);
////	if (!status.i2creaderror) {
////		*(u8*) (&off_minute) = (u8) eeprom;
////		if (off_hour > 23)
////			off_hour = 0;
////		//ets_uart_printf("\n\r Read program from eerpom:0x%X",eeprom);
////	}
//
//	eeprom = eepromReadByte(EEPROM_HOUR_OFF);
//	if (!status.i2creaderror) {
//		*(u8*) (&off_hour) = (u8) eeprom;
//		if (off_minute > 59)
//			off_minute = 0;
//		//ets_uart_printf("\n\r Read program from eerpom:0x%X",eeprom);
//	}
//	return true;
//}









bool ICACHE_FLASH_ATTR eeprom_saveStatus() {
	eepromWriteByte(EEPROM_STATUS, *(u8*) (&program));
	return true;
}

//bool ICACHE_FLASH_ATTR eeprom_saveTimer() {
//	if (on_hour > 23)
//		on_hour = 0;
//	eepromWriteByte(EEPROM_HOUR_ON, on_hour);
//	if (on_minute > 59)
//		on_minute = 0;
//	eepromWriteByte(EEPROM_MINUTE_ON, on_minute);
//	if (off_hour > 23)
//		off_hour = 0;
//	eepromWriteByte(EEPROM_HOUR_OFF, off_hour);
//	if (off_minute > 59)
//		off_minute = 0;
//	eepromWriteByte(EEPROM_MINUTE_OFF, off_minute);
//	return true;
//}


void ICACHE_FLASH_ATTR
eeprom_readAray(u16 start, u16 len) {
	i2c_master_start();
	I2C_WA(EEPROM_ADDRESS);
	I2C_WD((u8) (start >> 8));
	I2C_WD((u8) (start & 0x0f));
	i2c_master_start();
	I2C_RA(EEPROM_ADDRESS);
	u16 i;
	u8 byte;
	char row[134];
	char value[4];
	u16 address = start;
	ets_uart_printf(
			"        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F  \n");
	ets_sprintf(row, "00%X:", address);
	for (i = 1; i <= len; i++) {
		byte = i2c_master_readByte();
		if (i == len) {
			i2c_master_send_nack();
			i2c_master_stop();
		} else
			i2c_master_send_ack();
		if (byte < 16) {
			ets_sprintf(value, "0%X ", byte);
		} else {
			ets_sprintf(value, "%X ", byte);
		}
		strcat(row, value);
		if ( ((start + i) % 16 == 0  && i > 0) || i==len ) {
			//ets_uart_printf("address is :%X \n", (start + i));
			address = start + i;
			//print row
			strcat(row, "\n");
			ets_uart_printf(row);
			if (address < 16) {
				ets_sprintf(row, "00%X:", address);
			} else if (address < 256) {
				ets_sprintf(row, "0%X:", address);
			}
		}

		//ets_uart_printf("data: 0x%X:0x%X_%c \n",(start+i),byte,byte);
	}
	//ets_uart_printf("data: 0x%X:0x%X_%c \n",(start+i),byte,byte);

//	byte = i2c_master_readByte();
//	i2c_master_send_nack();
//	i2c_master_stop();
//	ets_uart_printf("data: 0x%X:0x%X_%c \n", (start + len), byte, byte);
}

void ICACHE_FLASH_ATTR displayBuffer(u8 *data,u8 len){
	u16 address=0;
	u8 i,j;
	char row[16];
	ets_uart_printf("0 1 2 3 4 5 6 7 8 9 A B C D E F \n");
	for(j=0;j<(len%16);j++){

	ets_sprintf(row,"%H:",address);
	for(i=0 ;i<16;i++){
	ets_sprintf(row,"%H:",data[address]);

	 }
	ets_sprintf(row,"\n");
	address+=0x10;
	ets_uart_printf(row);
	}
	//os_free(dataBuf);
}


bool ICACHE_FLASH_ATTR
eepromReadData(u16 address, char* data) {
	u8 len = 254;
	data[0] = 0;
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	i2c_master_start();
	I2C_WA(EEPROM_ADDRESS);
	I2C_WD(addrh);
	I2C_WD(addrl);
	i2c_master_start();
	I2C_RA(EEPROM_ADDRESS);
	u8 i;
	u8 byte;
	for (i = 0; i < len; i++) {
		byte = i2c_master_readByte();
		i2c_master_send_ack();
		//ets_uart_printf("Read data: 0x%X%X:%x_%c\n\r", addrh, (addrl + i), byte,	byte);
		if (i == 0) {
			if (byte == 0 || byte == 0xff)
				break;
			len = byte + 1;
			continue;
		}

		*data++ = byte;
		if (byte == 0)
			break;
	}
	byte = i2c_master_readByte();
	i2c_master_send_nack();
	i2c_master_stop();
	//ets_uart_printf("\n\r data: 0x%X:0x%X_%c",(addrh+len),byte,byte);
	return true;
}

bool ICACHE_FLASH_ATTR
eepromReadDataLen(u16 address, char* data, u8 len) {
	//data[0] = 0;
	//ets_uart_printf("Eeprom len: %d\n\r", len);
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	u8 i;
	u8 byte;
	//u8 eeprom_page=EEPROM_PAGE;
	i2c_master_start();
	I2C_WA(EEPROM_ADDRESS);
	I2C_WD(addrh);
	I2C_WD(addrl);
	i2c_master_start();
	I2C_RA(EEPROM_ADDRESS);
	for (i = 0; i < len-1; i++) {
		byte = i2c_master_readByte();
		i2c_master_send_ack();
		 //ets_uart_printf("Read data: 0x%02X%02X:%02x_%c\n\r", addrh, (addrl + i), byte,	byte);
		*data++ = byte;

	}
	byte = i2c_master_readByte();
	*data = byte;
	i2c_master_send_nack();
	i2c_master_stop();
	//ets_uart_printf("\n\r data: 0x%X:0x%X_%c",(addrh+len),byte,byte);

	return true;
}

bool ICACHE_FLASH_ATTR
eepromWriteDataLen(uint16 address,char *data,u8 len) {
	//u16 len = strlen(data);
	if (len > 256) return false;
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	init_I2C();
	u8 eeprom_page=EEPROM_PAGE;
	u8 i;
	//Select addressin EEPROM;

	i2c_master_start();
	if (!I2C_WA(EEPROM_ADDRESS))
		return false;
	if (!I2C_WD(addrh))
		return false;
	if (!I2C_WD(addrl))
		return false;
	for (i = 0; i < len + 1; i++) {
		if (!I2C_WD((u8) *data))
			return false;
		 //ets_uart_printf("Write data: 0x%02X%02X:%02x_%c\n\r", addrh, addrl ,	*data, *data);
		data++;
		addrl++;
		eeprom_page--;
		if(eeprom_page<=0){
		   // ets_uart_printf("Page count \r\n");
			eeprom_page=EEPROM_PAGE;
			i2c_master_stop();
			system_soft_wdt_feed();
			os_delay_us(10000);
			i2c_master_start();
			if (!I2C_WA(EEPROM_ADDRESS))
			 return false;
			if (!I2C_WD(addrh))
			 return false;
			if (!I2C_WD(addrl))
			 return false;
		}
	}
	i2c_master_stop();
    system_soft_wdt_feed();
	os_delay_us(10000);
	return true;
}


/*
bool ICACHE_FLASH_ATTR eeprom_writePassword(char *password, u16 address) {
	u8 len = os_strlen(password);
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	//ets_uart_printf("\n\r len: 0x%X",len);
	if (len == 0 || len > 32)
		return false;
	//first byte is len
	init_I2C();
	//Select addressin EEPROM;
	i2c_master_start();
	if (!I2C_WA(EEPROM_ADDRESS))
		return false;
	if (!I2C_WD(addrh))
		return false;
	if (!I2C_WD(addrl))
		return false;
	if (!I2C_WD((u8) len ^ EEPROM_CRYPTO))
		return false;
	u8 i;
	for (i = 0; i < len; i++) {
		if (!I2C_WD((u8) *password ^ EEPROM_CRYPTO))
			return false;
		//ets_uart_printf("\n\r len: %c",*name);
		password++;
	}

	i2c_master_stop();
	os_delay_us(10000);
	return true;
}

bool ICACHE_FLASH_ATTR eeprom_writeSSID(char *name, u16 address) {
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	//ets_uart_printf("Adrh:%X \n\r ",addrh);
	//ets_uart_printf("Adrl:%X \n\r ",addrl);
	//return true;
	u32 len = os_strlen(name);
	//ets_uart_printf("\n\r len: 0x%X",len);
	if (len == 0 || len > 32)
		return false;
	//first byte is len
	init_I2C();
	//Select addressin EEPROM;
	i2c_master_start();
	if (!I2C_WA(EEPROM_ADDRESS))
		return false;
	if (!I2C_WD(addrh))
		return false;
	if (!I2C_WD(addrl))
		return false;
	if (!I2C_WD((u8) len))
		return false;
	u8 i;
	for (i = 0; i < len; i++) {
		if (!I2C_WD((u8) *name))
			return false;
		//ets_uart_printf("\n\r len: %c",*name);
		name++;
	}

	i2c_master_stop();
	os_delay_us(10000);
	return true;
}
*/

bool ICACHE_FLASH_ATTR
eeprom_readWifi() {

	eepromReadData(EEPROM_LAN_SSID, (char*) lan_ssid);
	//ets_uart_printf("Read eeprom LAN_SSID:<%s>\n\r", lan_ssid);
	eepromReadData(EEPROM_LAN_PWD, (char*) lan_pwd);
	//ets_uart_printf("Read eeprom LAN_PWD:<%s>\n\r", lan_pwd);
	eepromReadData(EEPROM_WAN_SSID, (char*) wan_ssid);
	//ets_uart_printf("Read eeprom WAN_SSID:<%s>\n\r", wan_ssid);
	eepromReadData(EEPROM_WAN_PWD, (char*) wan_pwd);
	//ets_uart_printf("Read eeprom WAN_PWD:<%s>\n\r", wan_pwd);
	u8 byte=eepromReadByte(EEPROM_LAN_TYPE);
	lan_type=byte;
	if(lan_type>5)lan_type=0;
	return true;
}

mqttserver ICACHE_FLASH_ATTR
eeprom_readMqttServer(){
	mqttserver Server;
	eepromReadDataLen(EEPROM_MQTT_SERVER,(u8*)(&Server),sizeof(mqttserver));
	if(Server.ok!=0xAA){
		eeprom_initMqttServer(Server);
		eepromReadDataLen(EEPROM_MQTT_SERVER,(u8*)(&Server),sizeof(mqttserver));
	}
	//ets_uart_printf("Server.host: %s \n\r",Server.mqttHost);
	return Server;
}

bool ICACHE_FLASH_ATTR
eeprom_writeMqttServer(mqttserver wServer){
	//mqttserver wServer;
	if(eepromWriteDataLen(EEPROM_MQTT_SERVER,(u8 *)(&wServer),sizeof(wServer)))
      return true;
	else
	  return false;
}

bool ICACHE_FLASH_ATTR
eeprom_initMqttServer(mqttserver mqtt_server){
	mqtt_server.ok=0xAA;
	mqtt_server.mqttPort=12137;
	 strcpy(mqtt_server.mqttHost,"54.75.92.193");
     strcpy(mqtt_server.mqttUser,"root");
     strcpy(mqtt_server.mqttPassword,"1234567");
     strcpy(mqtt_server.mqttTopicRead,"/homelab/request");
     strcpy(mqtt_server.mqttTopicWrite,"/homelab/respond");
     if(eeprom_writeMqttServer(mqtt_server))
      return true;
     else
      return false;
}

bool ICACHE_FLASH_ATTR eeprom_readProgramer()
{
	u8 size=sizeof(programer[0]);
	u8 i,j;
	u8 eeprom;
	u8 eeprom_adr=EEPROM_PROGRAMER;
	u8 *ptr;

	for (i = 0; i < 10; i++) {
	 ptr = (u8 *) &programer[i];
		for (j = 0; j < size; j++) {
			//ets_uart_printf("Data of Progrmer [0]: 0x%X:0x%X  \n\r",eeprom_adr,*(ptr));
			eeprom=eepromReadByte(eeprom_adr++);
			*ptr++ = eeprom;
			//ets_uart_printf("Data of Progrmer [0]: 0x%X:0x%X:0x%X \n\r",eeprom_adr-1,*ptr,eeprom);
		}
		if(programer[i].timeOff>1440 || programer[i].timeOn>1440){
			ets_uart_printf("Program_%d_Off:%d\n",i,programer[i].timeOff);
			ets_uart_printf("Program_%d_On:%d\n",i,programer[i].timeOn);

			eeprom_initProgramer();
			i--; //
		}
	}

//	for(i=0;i<10;i++){
//		ets_uart_printf("----------------------------------------------------\n\r");
//		ets_uart_printf("Progrmer.number    [%X]:0x%X\n\r",i,programer[i].number);
//		ets_uart_printf("Progrmer.mode      [%X]:0x%X\n\r",i,programer[i].mode);
//		ets_uart_printf("Progrmer.timerOn   [%X]:0x%X\n\r",i,programer[i].timerOn);
//		ets_uart_printf("Progrmer.timerOff  [%X]:0x%X\n\r",i,programer[i].timerOff);
//
//
//	}
//
//	ets_uart_printf("Programer eeprom last address:0x%X\n\r",eeprom_adr);

	return true;
}

bool ICACHE_FLASH_ATTR eepromWriteData(u16 address, char *data) {
	u16 len = strlen(data);
	if (len > 256)
		return false;
	u8 addrh = (u8) (address >> 8);
	u8 addrl = (u8) (address & 0x00ff);
	init_I2C();
	//Select addressin EEPROM;
	i2c_master_start();
	if (!I2C_WA(EEPROM_ADDRESS))
		return false;
	if (!I2C_WD(addrh))
		return false;
	if (!I2C_WD(addrl))
		return false;
	if (!I2C_WD((u8) len)) //First save len of string
		return false;
	addrl++;
	u8 i;
	for (i = 0; i < len + 1; i++) {
		if (!I2C_WD((u8) *data))
			return false;
		//ets_uart_printf("Write data: 0x%X%X:%x_%c\n\r", addrh, (addrl + i),
		//		*data, *data);
		data++;
	}
	i2c_master_stop();
    system_soft_wdt_feed();
	os_delay_us(10000);
	return true;
}

bool ICACHE_FLASH_ATTR eeprom_writeProgramer(u8 number)
{
	    u8 size=sizeof(programer[number]);
		u8 eeprom_adr=EEPROM_PROGRAMER;
		eeprom_adr=eeprom_adr+size*number;
		u8 i;
		u8 *ptr= (u8 *)&programer[number];
		for(i=0;i<size;i++)
		 {
		 ets_uart_printf("Data of Progrmer [0]: 0x%X:0x%X  \n\r",eeprom_adr,*(ptr));
		  eepromWriteByte(eeprom_adr++,*(ptr++));
		 }

	return true;
}

bool ICACHE_FLASH_ATTR eeprom_initProgramer()
{
	ets_uart_printf("Init eeprom Programmer.Erase all! \n");
   u8 i;
   for(i=0;i<10;i++){
	   programer[i].number=i+1;
	   programer[i].mode=0;
	   programer[i].timeOn=0;
	   programer[i].timeOff=0;
	   eeprom_writeProgramer(i);
   }

	return true;
}

static ICACHE_FLASH_ATTR eeprom_init_Wifi() {

	ets_sprintf(lan_ssid,"AlaBala");
	eepromWriteData(EEPROM_LAN_SSID, lan_ssid);
	//ets_uart_printf("Read eeprom LAN_SSID:<%s>\n\r", lan_ssid);
	ets_sprintf(lan_pwd,"");
	eepromWriteData(EEPROM_LAN_PWD, lan_pwd);
	//ets_uart_printf("Read eeprom LAN_PWD:<%s>\n\r", lan_pwd);
	eepromWriteByte(EEPROM_LAN_TYPE,0);
	ets_sprintf(wan_ssid,"");
	eepromWriteData(EEPROM_WAN_SSID, wan_ssid);
	//ets_uart_printf("Read eeprom WAN_SSID:<%s>\n\r", wan_ssid);
	ets_sprintf(wan_pwd,"");
	eepromWriteData(EEPROM_WAN_PWD, wan_pwd);
	//ets_uart_printf("Read eeprom WAN_PWD:<%s>\n\r", wan_pwd);



}

bool ICACHE_FLASH_ATTR eeprom_init() {
	//ets_uart_printf("\n\r Init Eerpom first byte ....");
	//eeprom_writeByte(0,0xAA);
	//ets_uart_printf("\n\r Init Eerpom second byte ....");
	//eeprom_writeByte(EEPROM_STATUS,0x00);

	init_I2C();
	//Select addressin EEPROM;
	i2c_master_start();
	if (!I2C_WA(EEPROM_ADDRESS))
		return false;
	if (!I2C_WD(0))
		return false;
	if (!I2C_WD(0))
		return false;
	if (!I2C_WD(0xAA))
		return false;
	u8 i;
	for (i = 0; i < 5; i++) {
		if (!I2C_WD(0))
			return false;
	}

	i2c_master_stop();
	system_soft_wdt_feed();
	os_delay_us(10000);
	//Init Wifi WAN LAN
	eeprom_init_Wifi();
	//Init Programmer
	eeprom_initProgramer();
	return true;
}
