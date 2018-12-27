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
#include "string.h"




Softuart softuart;
char nextionData[50]={0};
const  char *week_days[]={ "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

void ICACHE_FLASH_ATTR
soft_serial_init(nextionDisplay_t* nextion)
{
  //GPIO2 RX GPIO0 TX baud 9600
  Softuart_SetPinRx(nextion->softuart, 2);
  Softuart_SetPinTx(nextion->softuart, 15);
  Softuart_Init(nextion->softuart, 9600);
}



void ICACHE_FLASH_ATTR 
nextion_display_init(nextionDisplay_t* nextion)
{
  nextion->softuart=&softuart;
  nextion->command=nextionData;
  soft_serial_init(nextion);
  //Init Page 0
  // volatile char send_toNextion[28] = {0};
  // ets_sprintf(send_toNextion, "page 0%c%c%c", 0xFF, 0xFF, 0xFF);
  // Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  // ets_uart_printf("Send to Nextion Page 0:%s \n",send_toNextion);
  // Check_For_Nextion_Data(nextion);

}

void ICACHE_FLASH_ATTR
send_data(nextionDisplay_t* nextion)
{
    //check if end on 0xFF
    //ets_uart_printf("Call send_data.... \r\n");
    char* token=nextion->command;
    uint8_t len =strlen(nextion->command);
    token+=len-1;
    //ets_uart_printf("Call send_data.... token:%d;len:%d\r\n",*token,len);
    if(!(*token--==0xff && *token--==0xff && *token==0xff)) //No End Command '0xff','0xff','0xff' chars add its
    {
     // ets_uart_printf("No end char's.. Command:%s \r\n",nextion->command);
      ets_sprintf(nextion->command,"%s%c%c%c",nextion->command,0xFF,0xFF,0xFF);
    }

     //ets_uart_printf("Send to Nextion :%s \r\n",nextion->command);
     Softuart_Puts(nextion->softuart, (const char *)nextion->command);
}

void ICACHE_FLASH_ATTR
display_themperature(nextionDisplay_t *nextion, char *temperature)
{
  //volatile char send_toNextion[28] = {0};
  //ets_sprintf(send_toNextion, "ThermoControl.t2.txt=\"%s\"%c%c%c", temperature, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t2.txt=\"%s\"", temperature);
  send_data(nextion);
  uint8_t thermo = atoi(temperature);
  thermo = (thermo - 8) * 3;
  //ets_sprintf(send_toNextion, "ThermoControl.j1.val=%d%c%c%c", thermo, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.j1.val=%d", thermo);
  send_data(nextion);
  //Send color bar for thermometer
  uint8_t color;
  uint16_t colorText;
  if (thermo < 18)
  {
    //blue
    color = THERMO_BLUE;
    colorText=BLUE;
  }
  else if (thermo < 42) //22c
  {
    //violet
    color = THERMO_VIOLET;
    colorText=VIOLET;
  }
  else if (thermo < 78) //34C
  {
    //orange
    color = THERMO_ORANGE;
    colorText=ORANGE;
  }
  else if (thermo < 100)
  {
    //red
    color = THERMO_RED;
    colorText=RED;
  }
  //ets_sprintf(send_toNextion, "ThermoControl.j1.ppic=%d%c%c%c", color, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.j1.ppic=%d", color);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
  os_delay_us(10);
  //ets_sprintf(send_toNextion, "ThermoControl.t2.pco=%d%c%c%c", colorText, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t2.pco=%d", colorText);
  send_data(nextion);
  //os_delay_us(100);
  Check_For_Nextion_Data(nextion);
}


void ICACHE_FLASH_ATTR
display_themperature_set(nextionDisplay_t *nextion)
{
  //volatile char send_toNextion[28] = {0};
  //ets_sprintf(temperature,"%c%d.%d",s,t1,t2);
  //ets_sprintf(send_toNextion, "ThermoControl.t1.txt=\"%d\x9c\"%c%c%c", themperatureSet,0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t1.txt=\"%d\x9c\"", themperatureSet);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion_Themperature_Set:%s \n",send_toNextion);
  uint8_t thermo = themperatureSet;
  thermo = (thermo - 8) * 3;
  //ets_sprintf(send_toNextion, "ThermoControl.j0.val=%d%c%c%c", thermo, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.j0.val=%d", thermo);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion_Themperature_Set:%s \n",send_toNextion);
  //Send color bar for thermometer
  uint8_t color;
  uint16_t colorText;
  if (thermo < 18)
  {
    //blue
    color = THERMO_BLUE;
    colorText=BLUE;
  }
  else if (thermo < 42) //22c
  {
    //violet
    color = THERMO_VIOLET;
    colorText=VIOLET;
  }
  else if (thermo < 78) //34C
  {
    //orange
    color = THERMO_ORANGE;
    colorText=ORANGE;
  }
  else if (thermo < 100)
  {
    //red
    color = THERMO_RED;
    colorText=RED;
  }
  //ets_sprintf(send_toNextion, "ThermoControl.j0.ppic=%d%c%c%c", color, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.j0.ppic=%d", color);
  send_data(nextion);
  os_delay_us(10);
  //ets_sprintf(send_toNextion, "ThermoControl.t1.pco=%d%c%c%c", colorText, 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t1.pco=%d", colorText);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
  //os_delay_us(100);
  Check_For_Nextion_Data(nextion);
}

void ICACHE_FLASH_ATTR
display_time(nextionDisplay_t *nextion)
{
  //volatile char send_toNextion[60];
  if (nextion->displayOk && nextion->pageNumber == 0)
  {
    static bool sec = true;
    //ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d:%02d:%02d\"%c%c%c",hours,minutes,seconds,0xFF,0xFF,0xFF);
    if (sec)
    {
      //ets_sprintf(send_toNextion, "ThermoControl.t3.txt=\"%02d:%02d\"%c%c%c", hours, minutes, 0xFF, 0xFF, 0xFF);
      ets_sprintf(nextion->command, "ThermoControl.t3.txt=\"%02d:%02d\"", hours, minutes);
      sec = false;
    }
    else
    {
      //ets_sprintf(send_toNextion, "ThermoControl.t3.txt=\"%02d %02d\"%c%c%c", hours, minutes, 0xFF, 0xFF, 0xFF);
      ets_sprintf(nextion->command, "ThermoControl.t3.txt=\"%02d %02d\"", hours, minutes);
      sec = true;
    }
    //ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d:%02d\"%c%c%c",hours,minutes,0xFF,0xFF,0xFF);
    //ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d %02d\"%c%c%c",hours,minutes,0xFF,0xFF,0xFF);
    //ets_sprintf(send_toNextion,"ThermoControl.t3.txt=\"%02d:%02d:%02d\"",hours,minutes,seconds);
    //ets_sprintf(send_toNextion,"%s\rThermoControl.t4.txt=\"%02d.%02d.%02d\"%c%c%c",send_toNextion,date,month,year,0xFF,0xFF,0xFF);
    //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
    send_data(nextion);
    // ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
    //os_delay_us(100);
    Check_For_Nextion_Data(nextion);
  }
}

void ICACHE_FLASH_ATTR
display_date(nextionDisplay_t* nextion)
{
  //char send_toNextion[28]={0};
  //ets_sprintf(send_toNextion, "ThermoControl.t4.txt=\"%02d.%02d.%02d\"%c%c%c", date, month, year, 0xFF, 0xFF, 0xFF);
  //ets_sprintf(send_toNextion, "ThermoControl.t4.txt=\"%02d.%02d\"%c%c%c", date, month,0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t4.txt=\"%02d/%02d\"", date, month);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
  //os_delay_us(100);
  Check_For_Nextion_Data(nextion);
}

void ICACHE_FLASH_ATTR
display_day(nextionDisplay_t* nextion)
{
  //char send_toNextion[28]={0};

  //ets_sprintf(send_toNextion, "ThermoControl.t6.txt=\"%s\"%c%c%c", week_days[days-1], 0xFF, 0xFF, 0xFF);
  //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  ets_sprintf(nextion->command, "ThermoControl.t6.txt=\"%s\"",week_days[days-1]);
  send_data(nextion);
  //ets_uart_printf("Send to Nextion:%s \n",send_toNextion);
  //os_delay_us(100);
  Check_For_Nextion_Data(nextion);
}




void ICACHE_FLASH_ATTR
display_wifi_rssi(nextionDisplay_t *nextion)
{
    
  //check for page where rssi display
  if (nextion->displayOk && nextion->pageNumber == 0)
  {
    //ets_uart_printf("Display rssi:\r\n");
    //char send_toNextion[28];
    sint8 rssi = wifi_station_get_rssi();
    if (rssi == 31)
    {
      rssi = 0;
    }
    else
    {
      rssi += 130;
    }
    //ets_sprintf(send_toNextion, "ThermoControl.j2.val=%d%c%c%c", rssi, 0xFF, 0xFF, 0xFF);
    //Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
    ets_sprintf(nextion->command, "ThermoControl.j2.val=%d",rssi);
    send_data(nextion);
    //ets_uart_printf("Send to Nextion RSSI:%s \n", send_toNextion);
    //os_delay_us(100);
    Check_For_Nextion_Data(nextion);
  }
}

void ICACHE_FLASH_ATTR
Validate_Decode_Message(nextionDisplay_t *nextion, uint8_t *msg, uint8_t len)
{
  //Check for valid Command must end 0xff,0xff,0xff and Decode it.
  uint8_t *pmsg = msg;
  uint8_t i = 0;
  nextion->displayOk = false;
  pmsg += len;
  if (*pmsg-- == 0xFF)
  {
    if (*pmsg-- == 0xFF)
    {
      if (*pmsg-- == 0xFF)
      {
        //Command is valid
        //ets_uart_printf("Data is valid. \r\n");
        nextion->displayOk = true;
        pmsg = msg; //Start of Command Decode it
        switch (*pmsg)
        {
         case 0x66: //Number of Page
            pmsg++; //Page Number
            nextion->pageNumber = *pmsg;
            ets_uart_printf("Page Id:%d \r\n", *pmsg);
            break;
         case 0x65: //Component Id: 65 00 07 00 FF FF FF; First after 0x65 is Page 0x00 ;Second is id 0x07
          //Thirth is Event 0x00-release 0x01-press
            pmsg++;
            ets_uart_printf("Send Component Id:%d \r\n", *pmsg);
            break;
         case 0x71: //Digital Value in Hex like this: 00 00 00 00 ->0;00 00 00 01->1
            pmsg += 4;
            ets_uart_printf("Send Digital value:%d \r\n", *pmsg);
            break;
         case 0x70: //Text value
            pmsg++;
            ets_uart_printf("Send Text value:%s \r\n", *pmsg);         
            break;
         case 0x1c: //Failed to assign. skip this :).
            ets_uart_printf("Failed to assing \r\n");
            goto END;
            break;
         case 0x30: //*Themperature set
            pmsg++;
             themperatureSet=(*pmsg / 3) + 8;
            ets_uart_printf("Set Themperature to:%d \r\n", themperatureSet);
            eeprom_write_themperature();
            break;
         case 0x1A:
            ets_uart_printf("Variable name invalid:0x%x \r\n",*pmsg);
            ets_uart_printf("Nextion Command: %s\r\n",nextion->command);
            break;
         default:
            //Unknow Command
            ets_uart_printf("Unknow Command:%d \r\n", *pmsg);
            ets_uart_printf("Nextion Command: %s\r\n",nextion->command);
        }
      }
    }
  }
  PageNextionDisplay(nextion);
  END:
  return;
}

void ICACHE_FLASH_ATTR
Check_For_Nextion_Data(nextionDisplay_t* nextion)
{
  // if(Softuart_Available(&softuart)){
  //  	uint8_t byte=Softuart_Read(&softuart);
  //    ets_uart_printf("Softuart: %c 0x%x \r\n",byte,byte);
  // }
  //feed dog
	system_soft_wdt_feed();
  uint8_t data[10]={0};
  uint8_t i = 0;
  if (Softuart_Available(nextion->softuart))
  {
    do
    {
      uint8_t byte = Softuart_Read(nextion->softuart);
      data[i++] = byte;
      if (i > 10)
        break;
      os_delay_us((softuart.bit_time) * 2);
    } while (Softuart_Available(nextion->softuart));

    //Display buffer data
    // ets_uart_printf("data:");
    // uint8_t j;
    // for (j = 0; j < i; j++)
    // {
    //   ets_uart_printf("0x%x ", data[j]);
    // }
    // ets_uart_printf("\r\n");
    Validate_Decode_Message(nextion,data,i-1);   
  }
}

void ICACHE_FLASH_ATTR
PageNextion(nextionDisplay_t *nextion)
{
  //char send_toNextion[28]={0};
  //ets_sprintf(send_toNextion, "\"sendme\"%c%c%c", 0xFF, 0xFF, 0xFF);
 // Softuart_Puts(nextion->softuart, (const char *)send_toNextion);
  
  ets_sprintf(nextion->command, "page 0");
  send_data(nextion);
  ets_uart_printf("Nextion page 0. \r\n");
  Check_For_Nextion_Data(nextion);
}

void ICACHE_FLASH_ATTR
PageNextionDisplay(nextionDisplay_t *nextion)
{
  if (nextion->displayOk)
  {
    if (nextion->pageNumber == 0)
    {
      display_themperature_set(nextion);
      if (ReadDS1307())
      {
        ets_uart_printf("Send Page 0 \r\n");
        //Read Temperature is found it
        DS1307_Temperature();
        display_themperature(nextion,temperature);
        display_time(nextion);
        display_date(nextion);
        display_day(nextion);
        display_wifi_rssi(nextion);
      }
      else
      {
        ets_uart_printf("\n\r TimerKeeper not found!");
      }
    }
  }
}