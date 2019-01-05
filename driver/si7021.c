/*
 * si7021.c
 *
 *  Created on: 15.08.2018 г.
 *      Author: imihalev
 */

// #include "driver/i2c.h"
// #include "esp_log.h"
// #include "esp_system.h"
// #include "esp_err.h"

#include "user_interface.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "driver/si7021.h"
#include "driver/i2c_master.h"
#include "stdlib.h"
//#include "stdlib_noniso.h"

void ICACHE_FLASH_ATTR
floatToString(float value, char *buffer)
{
    int val1 = (int)value;
    unsigned int val2;
    if (value < 0)
    {
        val2 = (int)(-10.0 * value) % 10;
    }
    else
    {
        val2 = (int)(10.0 * value) % 10;
    }
   
    //  if (value < 0)
    // {
    //     val2 = (int)(-100.0 * value) % 100;
    // }
    // else
    // {
    //     val2 = (int)(100.0 * value) % 100;
    // }
    

    //ets_sprintf(buffer, "%i.%02u", val1, val2);
    ets_sprintf(buffer, "%i.%01u", val1, val2);
   
}

// void ICACHE_FLASH_ATTR
//  ftoa(float Value, char* Buffer)
//  {
//      union
//      {
//          float f;
     
//          struct
//          {
//              unsigned int    mantissa_lo : 16;
//              unsigned int    mantissa_hi : 7;    
//              unsigned int     exponent : 8;
//              unsigned int     sign : 1;
//          };
//      } helper;
     
//      unsigned long mantissa;
//      signed char exponent;
//      unsigned int int_part;
//      char frac_part[3];
//      int i, count = 0;
     
//      helper.f = Value;
//      //ets_uart_printf("helper.f:b%b\n",helper.f);
//      //mantissa is LS 23 bits
//      mantissa = helper.mantissa_lo;
//       //ets_uart_printf("mantissa:0x%x\n",mantissa);
//       //ets_uart_printf("mantissa_hi:0x%x\n",helper.mantissa_hi);
//      mantissa += ((unsigned long) helper.mantissa_hi << 16);
//      //add the 24th bit to get 1.mmmm^eeee format
//      mantissa += 0x00800000;
//      //ets_uart_printf("mantissa_finish:0x%x\n",mantissa);
//      //exponent is biased by 127
//      //ets_uart_printf("helper.exponent:0x%x\n",helper.exponent);
//      exponent = (signed char) helper.exponent - 127;
//      //ets_uart_printf("exponent:0x%x\n",exponent);
//      //too big to shove into 8 chars
//      if (exponent > 18)
//      {
//          Buffer[0] = 'I';
//          Buffer[1] = 'n';
//          Buffer[2] = 'f';
//          Buffer[3] = '\0';
//          return;
//      }
     
//      //too small to resolve (resolution of 1/8)
//      if (exponent < -3)
//      {
//          Buffer[0] = '0';
//          Buffer[1] = '\0';
//          return;
//      }
     
//      count = 0;
     
//      //add negative sign (if applicable)
//      if (helper.sign)
//      {
//          Buffer[0] = '-';
//          count++;
//      }
     
//      //get the integer part
//      int_part = mantissa >> (23 - exponent);    
//      //convert to string
//      //itoa(int_part, &Buffer[count]);
//      ets_sprintf(&Buffer[count],"%d",int_part);
     
//      //find the end of the integer
//      for (i = 0; i < 8; i++)
//          if (Buffer[i] == '\0')
//          {
//              count = i;
//              break;
//          }        
 
//      //not enough room in the buffer for the frac part    
//      if (count > 5)
//          return;
     
//      //add the decimal point    
//      Buffer[count++] = '.';
     
//      //use switch to resolve the fractional part
//      switch (0x7 & (mantissa  >> (20 - exponent)))
//      {
//          case 0:
//              frac_part[0] = '0';
//              frac_part[1] = '0';
//              frac_part[2] = '0';
//              break;
//          case 1:
//              frac_part[0] = '1';
//              frac_part[1] = '2';
//              frac_part[2] = '5';            
//              break;
//          case 2:
//              frac_part[0] = '2';
//              frac_part[1] = '5';
//              frac_part[2] = '0';            
//              break;
//          case 3:
//              frac_part[0] = '3';
//              frac_part[1] = '7';
//              frac_part[2] = '5';            
//              break;
//          case 4:
//              frac_part[0] = '5';
//              frac_part[1] = '0';
//              frac_part[2] = '0';            
//              break;
//          case 5:
//              frac_part[0] = '6';
//              frac_part[1] = '2';
//              frac_part[2] = '5';            
//              break;
//          case 6:
//              frac_part[0] = '7';
//              frac_part[1] = '5';
//              frac_part[2] = '0';            
//              break;
//          case 7:
//              frac_part[0] = '8';
//              frac_part[1] = '7';
//              frac_part[2] = '5';                    
//              break;
//      }
     
//      //add the fractional part to the output string
//      for (i = 0; i < 3; i++)
//          if (count < 7)
//              Buffer[count++] = frac_part[i];
     
//      //make sure the output is terminated
//      Buffer[count] = '\0';
//  }




static bool ICACHE_FLASH_ATTR
 check_crc(uint16_t data,uint8_t crc){

	uint32_t raw=(uint32_t)data <<8;
	//CRC generator polynomial of x^8 + x^5 + x^4 + 1 from 0x00--> 100110001
	uint32_t divisor = 0b100110001<<15;
    raw |= crc;
    int i;
      for(i=0 ; i<16;i++){
    	   if( raw &(uint32_t)1<<(23-i)){ //check last righ bit in raw
              raw ^=divisor;
    	   }
             divisor>>=1;
      }
         return(raw==0);

}



void ICACHE_FLASH_ATTR
 readCommand(uint8_t command,uint8_t data)
 {
	
    i2c_master_start();
    i2c_master_writeByte(SI7021_ADDR<<1 | I2C_MASTER_WRITE);
    //Check for Ack 
    i2c_master_writeByte(command);
    //Check for Ack
    i2c_master_start();
    i2c_master_writeByte(SI7021_ADDR<<1 | I2C_MASTER_READ);
    //Check for Ack
    data=i2c_master_readByte();
    i2c_master_stop();		
}

void ICACHE_FLASH_ATTR
writeCommand(uint8_t command, uint8_t data)
{

    i2c_master_start();
    i2c_master_writeByte(SI7021_ADDR << 1 | I2C_MASTER_WRITE);
    //Check for Ack
    i2c_master_writeByte(command);
    //Check for Ack
    i2c_master_writeByte(data);
    //Check for Ack
    i2c_master_stop();
}

bool ICACHE_FLASH_ATTR
readTemperature(char *themperature) //double *themperature
{
    system_soft_wdt_feed();
    //ets_uart_printf("Read Temperature from si7021 \r\n");
    bool err = false;
    float themp;
    uint8_t msb, lsb, crc;
    uint8_t count_wait=0xFF;
    //Start Measure
    i2c_master_start();
    i2c_master_writeByte((SI7021_ADDR << 1) | I2C_MASTER_WRITE);
    //Check for Ack
    if (!i2c_master_checkAck())
    {
        ets_uart_printf("I2C si7021 master write address error:0x%x \r\n",((SI7021_ADDR << 1) | I2C_MASTER_WRITE));
        i2c_master_stop();
        goto END;
    }
    i2c_master_writeByte(MTNHMM);
    //Check for Ack
    if (!i2c_master_checkAck())
    {
        ets_uart_printf("I2C si7021 command MTNHMM error:0x%x \r\n",MTNHMM);
        i2c_master_stop();
        goto END;
    }
    i2c_master_stop();
    //Waiting some time
WAIT_SI7021: 
    count_wait--;
    os_delay_us(1000);
    //Read Data from sensor  
    i2c_master_start();
    i2c_master_writeByte((SI7021_ADDR << 1) | I2C_MASTER_READ);
    //Check for Ack
    if (!i2c_master_checkAck())
    {   
        if(count_wait==0)
        {
        ets_uart_printf("I2C si7021 master read  address error:0x%x ;count_wait:%0x%x\r\n",((SI7021_ADDR << 1) | I2C_MASTER_READ),count_wait);
        i2c_master_stop();
        goto END;
        } else goto WAIT_SI7021;

    }
    //ets_uart_printf("count_wait:0x%x \n",count_wait);
    msb = i2c_master_readByte(); //ack
    i2c_master_send_ack();
    lsb = i2c_master_readByte(); //ack
    i2c_master_send_ack();
    crc = i2c_master_readByte(); // No ack
    i2c_master_send_nack();
    i2c_master_stop();

      //ets_uart_printf("Temperature msb:0x%x \n", msb);
      //ets_uart_printf("Temperature lsb:0x%x \n", lsb);
      //ets_uart_printf("Temperature crc:0x%x \n", crc);
    

    uint16_t raw_value = (uint16_t)msb << 8 | (uint16_t)lsb;
      //ets_uart_printf("Temperature raw_value:0x%x \n", raw_value);
    if (check_crc(raw_value, crc))
    {
        themp = (raw_value * 175.72)/65536.0-46.85; 
        char charVal[6]; 
        floatToString(themp,charVal);   
        ets_sprintf(themperature,"%s", charVal);
        ets_uart_printf("Temperature is:%s\n", themperature);
        err = TRUE;
    }
END:
    system_soft_wdt_feed();
    return err;
}

bool readHumidity(char *humidity)
{  
    system_soft_wdt_feed();
    //ets_uart_printf("Read Humidity si7021 \r\n");
    bool err = FALSE;
    uint8_t msb, lsb, crc;
    uint32_t humidity_val;
    uint8_t count_wait = 0xff;
    //Start Measure
    i2c_master_start();
    i2c_master_writeByte(SI7021_ADDR << 1 | I2C_MASTER_WRITE);
    //Check for Ack
    if (!i2c_master_checkAck())
    {
        ets_uart_printf("I2C si7021 command SI7021_ADDR error:0x%x \r\n",MTNHMM);
        i2c_master_stop();
        goto END;
    }
    i2c_master_writeByte(MRHNHMM);
    //Check for Ack
    if (!i2c_master_checkAck())
    {
        ets_uart_printf("I2C si7021 command MRHNHMM error:0x%x \r\n",MTNHMM);
        i2c_master_stop();
        goto END;
    }
    i2c_master_stop();
    //Waiting sensor
WAIT_SI7021:
    count_wait--;
    os_delay_us(1000);
    i2c_master_start();
    i2c_master_writeByte((SI7021_ADDR << 1) | I2C_MASTER_READ);
    //Check for Ack
    if (!i2c_master_checkAck())
    {
        if (count_wait == 0)
        {
            ets_uart_printf("I2C si7021 master read  address error:0x%x ;count_wait:%0x%x\r\n", ((SI7021_ADDR << 1) | I2C_MASTER_READ), count_wait);
            i2c_master_stop();
            goto END;
        }
        else
            goto WAIT_SI7021;
    }
    //Read Data from sensor
    msb = i2c_master_readByte(); //ack
    i2c_master_send_ack();
    lsb = i2c_master_readByte(); //ack
    i2c_master_send_ack();
    crc = i2c_master_readByte(); // No ack
    i2c_master_send_nack();
    i2c_master_stop();

    uint16_t raw_value = (uint16_t)msb << 8 | (uint16_t)lsb;
    if (check_crc(raw_value, crc))
    {
        //ets_uart_printf("Humidity raw_value is:%d\n", raw_value);
        humidity_val = ((raw_value * 125)/65536 - 6);
        ets_sprintf(humidity, "%d", humidity_val);
        ets_uart_printf("Humidity is:%s\n", humidity);
        err = TRUE;
    }
END:
    system_soft_wdt_feed();
    return err;
}

// esp_err_t readUserRegister(user_register_t* data){
// 	esp_err_t err=ESP_OK;
// 	uint8_t command=RUR1;
// 	err=readCommand(command,(uint8_t*)data);
//    return err;
// }

// esp_err_t writeUserRegister(user_register_t* data){
// 	esp_err_t err=ESP_OK;
// 	uint8_t command=WUR1;
// 	err=writeCommand(command,(uint8_t*)data);
//     return err;
// }


