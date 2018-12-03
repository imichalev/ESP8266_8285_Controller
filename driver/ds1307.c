/*
 * ds1307.h
 *
 *  Created on: 3.02.2017 ã.
 *      Author: admin
 */

#ifndef DRIVER_DS1307_C_
#define DRIVER_DS1307_C_

#include "driver/i2c_master.h"
#include "user_interface.h"
#include "driver/ds1307.h"

#endif /* DRIVER_DS1307_C_ */

//volatile u8 error;

static bool ICACHE_FLASH_ATTR I2C_WA(u8 address)
{
  i2c_master_writeByte(address);
   if(!i2c_master_checkAck())
	  {
	    error=1;
	   	i2c_master_stop();
	   	ets_uart_printf("\n\r DS3231 Error from device:0x%X",address);
	   	return false;
      }
   //ets_uart_printf("\n\r DS3231 Ack from device:0x%X",address);
   return true;
}

static bool ICACHE_FLASH_ATTR I2C_RA(u8 address)
{
  i2c_master_writeByte(address|0x1); //Set last bit to  read
   if(!i2c_master_checkAck())
	  {
	    error=2;
	    ets_uart_printf("\n\r DS3231 Error write read address:0x%X",address|0x1);
	   	i2c_master_stop();
	   	return false;
      }
   //ets_uart_printf("\n\r DS3231 Write read address ok:0x%X",address|0x1);
   return true;
}


static bool ICACHE_FLASH_ATTR I2C_WD(u8 data)
{
	i2c_master_writeByte(data);
	   if(!i2c_master_checkAck())
		  {
		    error=3;
			ets_uart_printf("\n\r DS3231 Error write data:0x%X",data);
		   	i2c_master_stop();
		   	return false;
	      }
	   //ets_uart_printf("\n\r DS3231 Write data ok:0x%X",data);
     return true;
}

static u8 ICACHE_FLASH_ATTR hex2bcd(u8 data)
{
   u8 data_second_decimal=data/10;
   u8 data_first_decimal=data - 10*data_second_decimal;
   data=16*data_second_decimal + data_first_decimal;
  return data;
}

static u8 ICACHE_FLASH_ATTR bcd2hex(u8 bcd)
{
	u8 hex=0;
	hex=(bcd>>4)*10 + (bcd&0x0f);
	return hex;
}
static init_I2C()
{
	if(!status.i2c)
	 {
	  i2c_master_gpio_init();
	  //i2c_master_init();
	  ets_uart_printf("\n\r I2C Init... !");
	  status.i2c=1;
	 }
	error=0;
}


bool ICACHE_FLASH_ATTR ReadDS1307_Temperature()
{
	 ets_sprintf(temperature,"UNKNOW");
	 init_I2C();
	 i2c_master_start();
	 if(!I2C_WA(DS1307_ADDRESS)) return false;
	 if(!I2C_WD(TEMPERATURE_ADDRESS)) return false;  //Temp address
	 i2c_master_stop();
	 i2c_master_start();
	 if(!I2C_RA(DS1307_ADDRESS)) return false;
	 u8 msb=i2c_master_readByte();
	 i2c_master_send_ack();
	 u8 lsb=i2c_master_readByte();
	 i2c_master_send_nack();
	 i2c_master_stop();
	 //ets_uart_printf("\n\r Temperature: 0x%X 0x%X",msb,lsb);
	 u8 t1=0;
	 u8 t2=0;
	 char s='+';
	 if (msb&0x80) s='-';
	 switch(lsb>>6)
	 {
	 case 1:
	   t2=25;
	   break;
	 case 2:
	   t2=50;
	   break;
	 case 3:
		t2=75;
		break;
	 default :
   	     t2=0;
	 }
     t1=msb&0x7f;
     ets_sprintf(temperature,"%c%d.%d",s,t1,t2);
     //ets_uart_printf("\n\r Temperature:%s",temperature);
	 return true;
}

bool ICACHE_FLASH_ATTR SetTimeDate(u8 set_seconds,u8 set_minutes,u8 set_hours,u8 set_day,u8 set_date, u8 set_month,u8 set_year)
{
	if(set_seconds>59) set_seconds=0;
	if(set_minutes>59) set_minutes=0;
	if(set_hours>23)  set_hours=0;
	if(set_day>7) set_day=1;
	if(set_date==0 || set_date>31) set_date=1;
	if(set_month==0 || set_month>12) set_month=1;
	if(set_year>99) set_year=0;
	init_I2C();
	i2c_master_start();
	if(!I2C_WA(DS1307_ADDRESS)) return false;
	if(!I2C_WD(0x00)) return false;  //Set first position
    if(!I2C_WD(hex2bcd(set_seconds))) return false;
    if(!I2C_WD(hex2bcd(set_minutes))) return false;
    if(!I2C_WD(hex2bcd(set_hours))) return false;
    if(!I2C_WD(hex2bcd(set_day))) return false;
    if(!I2C_WD(hex2bcd(set_date))) return false;
    if(!I2C_WD(hex2bcd(set_month))) return false;
    if(!I2C_WD(hex2bcd(set_year))) return false;
	i2c_master_stop();
	return true;
}

bool ICACHE_FLASH_ATTR ReadDS1307()
{

   //Init I2C
	//ETS_INTR_LOCK();
   init_I2C();
   i2c_master_start();
   if(!I2C_WA(DS1307_ADDRESS)) return false;
   if(!I2C_WD(0x00)) return false;
    //i2c_master_stop();
   i2c_master_start();
    //return true;
   if(!I2C_RA(DS1307_ADDRESS)) return false;
        //read first byte second
   seconds=bcd2hex(i2c_master_readByte());
   i2c_master_send_ack();
   minutes=bcd2hex(i2c_master_readByte());
   i2c_master_send_ack();
   hours=bcd2hex(i2c_master_readByte()&0x3F);
   i2c_master_send_ack();
   days=bcd2hex(i2c_master_readByte());
   i2c_master_send_ack();
   date=bcd2hex(i2c_master_readByte());
   i2c_master_send_ack();
   month=bcd2hex(i2c_master_readByte()&0x1F);
   i2c_master_send_ack();
   year= bcd2hex(i2c_master_readByte());
   i2c_master_send_ack();
   u8 a1m1= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a1m2= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a1m3= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a1m4= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a2m2= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a2m3= i2c_master_readByte();
   i2c_master_send_ack();
   u8 a2m4= i2c_master_readByte();
   i2c_master_send_ack();
   u8 control= i2c_master_readByte();
   i2c_master_send_ack();
   u8 status= i2c_master_readByte();
   i2c_master_send_ack();
   u8 agingoffset=i2c_master_readByte();
   i2c_master_send_ack();
   u8 msbtemp= i2c_master_readByte();
   i2c_master_send_ack();
   u8 lsbtemp= i2c_master_readByte();
   i2c_master_send_nack();
   i2c_master_stop();
   //ETS_INTR_UNLOCK();
   //ets_uart_printf("\n\r Data from timerKeeper:%d/%d/%d/ %d %d:%d:%d",year,month,date,days,hours,minutes,seconds);

  return true;

}
