/*
 * keyboard.c
 *
 *  Created on: 5.01.2017 �.
 *      Author: admin
 */

#include "driver/keyboard.h"
#include "user_interface.h"
#include "osapi.h"
#include "user_config.h"
#include "driver/oled.h"

//bool keyPress=false;
//bool keyTime=false;
static os_timer_t keyTimer;
//bool showMe=true;
//u16 adcValue;
//u16 adcValueCurrent;
uint16 level=0;
//extern bool keypush;
//uint16 curentLevel=0;
#define KEY4_MUX  PERIPHS_IO_MUX_MTDI_U
#define KEY4_FUNC FUNC_GPIO12
#define KEY4_PIN  12
#define KEY3_MUX  PERIPHS_IO_MUX_MTCK_U
#define KEY3_FUNC FUNC_GPIO13
#define KEY3_PIN  13
#define KEY2_MUX  PERIPHS_IO_MUX_GPIO0_U
#define KEY2_FUNC FUNC_GPIO0
#define KEY2_PIN  0
//#define KEY1_MUX  PERIPHS_IO_MUX_GPIO2_U
//#define KEY1_FUNC FUNC_GPIO2
//#define KEY1_PIN  2
#define KEY1_MUX  PERIPHS_IO_MUX_MTCK_U
#define KEY1_FUNC FUNC_GPIO14
#define KEY1_PIN  14

//#define KEYMASK 1<<GPIO_ID_PIN(KEY1_PIN) || 1<<GPIO_ID_PIN(KEY2_PIN) || 1<<GPIO_ID_PIN(KEY3_PIN) || 1<<GPIO_ID_PIN(KEY4_PIN)

void
ICACHE_FLASH_ATTR
keyIsrTimer(void){
	 os_timer_disarm(&keyTimer);
	 //INFOKEY("Key Push..........\r\n");
	 keypush=0;
	 ScanKey();
}





void ICACHE_FLASH_ATTR
ScanKey(void)
{
  //disable INT
  ETS_GPIO_INTR_DISABLE();
 if(keypush!=0){
 	  os_timer_disarm(&keyTimer);
 	  os_timer_setfn(&keyTimer,keyIsrTimer,NULL);
 	  os_timer_arm(&keyTimer,TIME_KEY,0);
 	  return;
   }

  uint32 gpio_status=GPIO_REG_READ(GPIO_STATUS_ADDRESS);
  //uint8  level=0;
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,gpio_status);
  keypush=1;
  key=0;
  uint16 KEYMASK= (uint16) (1<< GPIO_ID_PIN(KEY1_PIN) | 1<< GPIO_ID_PIN(KEY2_PIN) | 1<< GPIO_ID_PIN(KEY3_PIN) | 1<< GPIO_ID_PIN(KEY4_PIN));
  //INFOKEY("Scan Key.... <0x%X> \n",level);
  level=gpio_input_get();

//    if(showMe==true){
//    	showMe=false;
//    INFOKEY("KEY1 0x%X <0x%X> \n",GPIO_ID_PIN(KEY1_PIN), 1<< GPIO_ID_PIN(KEY1_PIN));
//    INFOKEY("KEY2 0x%X <0x%X> \n",GPIO_ID_PIN(KEY2_PIN),1<< GPIO_ID_PIN(KEY2_PIN));
//    INFOKEY("KEY3 0x%X <0x%X> \n",GPIO_ID_PIN(KEY3_PIN),1<< GPIO_ID_PIN(KEY3_PIN));
//    INFOKEY("KEY4 0x%X <0x%X> \n",GPIO_ID_PIN(KEY4_PIN),1<< GPIO_ID_PIN(KEY4_PIN));
//
//	 INFOKEY("Key Mask 0x%X \n",KEYMASK);
//
//	 INFOKEY("Key Scan 0x%X \n",level);
//
//	 INFOKEY("Level && KEYMASK  0x%X \n",level&KEYMASK);
//
//    }

    //return 0 ;
   //Check if some of this bit is set KEY1_PIN;KEY2_PIN;KEY3_PIN;KEY4_PIN ; bit mask for this bits is
   //uint32 bitMask=GPIO_ID_PIN(KEY1_PIN)||GPIO_ID_PIN(KEY2_PIN)||GPIO_ID_PIN(KEY3_PIN)||GPIO_ID_PIN(KEY4_PIN);

   //level=GPIO_INPUT_GET(GPIO_ID_PIN(KEY1_PIN));
    displayTimer=DISPLAYTIMER;
    level=level&KEYMASK;
    if((level&KEYMASK)==KEYMASK){
    	//keyPress=false;
    	key=0;
//    	ETS_GPIO_INTR_ENABLE();
//    	return;
    }

//    if(keyPress==false){
//     //INFOKEY("Press Any Key ...\n");
//     //INFOKEY("KeyTime %d \n",keyTime);
//     INFOKEY("Scan Key.... \n");
//   	 //adcValue=system_adc_read();
//     os_timer_disarm(&keyTimer);
//     os_timer_setfn(&keyTimer,keyIsrTimer,NULL);
//     os_timer_arm(&keyTimer,TIME_KEY,0);
//     keyTime=false;
//     keyPress=true;
//    }
//    if(keyTime==false){
//    	 	return 0;
//    }
//    keyPress=false;
//    keyTime=false;
//	 // os_delay_us(TIME_KEY);
//	//adcValueCurrent=system_adc_read();;
//    curentLevel=gpio_input_get();
//	INFOKEY("curent value: %X,<%X>,<%X> \n",level&KEYMASK,curentLevel&KEYMASK,(~(1<< GPIO_ID_PIN(KEY1_PIN))&KEYMASK)  );
//	  if( level != curentLevel){
//		 //Key_error
//		 // INFOKEY("Key error ...\n");
//		  keyPress=false;
//		  keyTime=false;
//		 return 0 ;
//	  }


      //Wake Up Display
    else  if(oled.on==0){
        displayTimer=DISPLAYTIMER;
    	oled_on();
     	key=0;
//    	return;
   		}

    else  if(level==(~(1<< GPIO_ID_PIN(KEY1_PIN))&KEYMASK) ){
		    Beep(60);
		    INFOKEY("Key 1\n");
		    key=1;
           // return; //Key_1
	   }

   else if (level==(~(1<< GPIO_ID_PIN(KEY2_PIN))&KEYMASK)){
		   Beep(60);
		   INFOKEY("Key 2\n");
		   key=2;
		   //return;  //Key_2
	   }

   else if (level==(~(1<< GPIO_ID_PIN(KEY3_PIN))&KEYMASK)){
		   Beep(60);
		   INFOKEY("Key 3\n");
		   key=3;
		   //return; //Key_3
	   }

   else if (level==(~(1<< GPIO_ID_PIN(KEY4_PIN))&KEYMASK)){
		   Beep(60);
		   INFOKEY("Key 4\n");
		   key=4;
		  // return; //Key_4
	   }

   else if (level==(  (~(1<< GPIO_ID_PIN(KEY2_PIN))&KEYMASK)) | (~(1<< GPIO_ID_PIN(KEY3_PIN) & KEYMASK)) ){
	   		   Beep(60);
	   		   INFOKEY("Key 23\n");
	   		   key=23;
	   		   //return; //Key_4
	   	   }

	else if (level==(  (~(1<< GPIO_ID_PIN(KEY2_PIN))&KEYMASK) | (~(1<< GPIO_ID_PIN(KEY3_PIN))&KEYMASK) )){
		       Beep(100);
		       INFOKEY("Key 14\n");
		       key=14;
	  		  // return; //Key_4
	  	   }
	   ETS_GPIO_INTR_ENABLE();
       return ; //No-key
}

void ICACHE_FLASH_ATTR
InitKeys(void){
	//Key 1 on Left GPIO12;
	PIN_FUNC_SELECT(KEY1_MUX,KEY1_FUNC);
	PIN_PULLUP_EN(KEY1_MUX);
	GPIO_DIS_OUTPUT(KEY1_PIN);

	//Key 2 GPIO13;
	PIN_FUNC_SELECT(KEY2_MUX,KEY2_FUNC);
	PIN_PULLUP_EN(KEY2_MUX);
	GPIO_DIS_OUTPUT(KEY2_PIN);
	//Key 3 GPIO0 + DTR ;
	PIN_FUNC_SELECT(KEY3_MUX,KEY3_FUNC);
	PIN_PULLUP_EN(KEY3_MUX);
	GPIO_DIS_OUTPUT(KEY3_PIN);
	//Key 4 GPIO14;
	PIN_FUNC_SELECT(KEY4_MUX,KEY4_FUNC);
	PIN_PULLUP_EN(KEY4_MUX);
	GPIO_DIS_OUTPUT(KEY4_PIN);


	ETS_GPIO_INTR_DISABLE();
	ETS_GPIO_INTR_ATTACH(ScanKey,NULL);
	gpio_pin_intr_state_set(KEY1_PIN,GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(KEY2_PIN,GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(KEY3_PIN,GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(KEY4_PIN,GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();
}




