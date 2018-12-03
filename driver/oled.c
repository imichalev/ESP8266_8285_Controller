
#include "driver/oled.h"
#include "driver/i2c_master.h"




//static  bool ICACHE_FLASH_ATTR sendCommand(u8 data)
//{
//   i2c_master_start();
//   i2c_master_writeByte(DISPLAY_ADDRESS);  // Clear A0 write in Control register
//   if(!i2c_master_checkAck())
//   {
//   	i2c_master_stop();
//   	 DEBUG_OLED_OLED("OLED Error from device address:0x%X \n\r",DISPLAY_ADDRESS);
//   	 oled.i2c=0;
//   	return false;
//   }
//
//   i2c_master_writeByte(0x00);  // set Command mode 0x80
//    if(!i2c_master_checkAck())
//       {
//      	i2c_master_stop();
//   	 DEBUG_OLED("OLED Error from set command:0x%X \n\r",0x00);
//    	oled.i2c=0;
//      	return false;
//       }
//
//   i2c_master_writeByte(data);
//   if(!i2c_master_checkAck())
//     {
//      	i2c_master_stop();
//      	DEBUG_OLED("OLED Error from set command:0x%X \n\r",data);
//      	oled.i2c=0;
//      	return false;
//      }
//    i2c_master_stop();
//    oled.i2c=1;
//    return true;
//}

//static bool ICACHE_FLASH_ATTR sendData (u8 data)
//{
//
//
//   //count=0;
//	i2c_master_start();
//	i2c_master_writeByte(DISPLAY_ADDRESS);  // SA0 must by 0
//    if(!i2c_master_checkAck())
//      {
//    	i2c_master_stop();
//    	DEBUG_OLED("OLED Error send_data address:0x%X \n\r",DISPLAY_ADDRESS);
//    	oled.i2c=0;
//    	return false;
//      }
//
//
//    i2c_master_writeByte(0x40);  // set Data mode
//    if(!i2c_master_checkAck())
//       {
//       	i2c_master_stop();
//       	DEBUG_OLED("OLED Error send_data data mode:0x%X \n\r",0x40);
//       	oled.i2c=0;
//      	return false;
//       }
//	    DEBUG_OLED("OLED Send_data data mode:0x%X \n\r",0x40);
//
//
//    i2c_master_writeByte(data);
//    if(!i2c_master_checkAck())
//       {
//       	i2c_master_stop();
//       	DEBUG_OLED("OLED Error send_data data :0x%X \n\r",data);
//       	oled.i2c=0;
//       	return false;
//       }
// 	DEBUG_OLED("OLED send_data data :0x%X \n\r",data);
//
//      i2c_master_stop();
//
//     oled.i2c=1;
//     return true;
//}
//

static bool ICACHE_FLASH_ATTR send_buffer( u8 *buffer, u16 len)
{
//	if(oled.countErr<=0){
	//DEBUG_OLED("Send Buffer %d:%d\r\n",oled.i2c,oled.countErr);
//		return false;
//	}

    if(!oled.i2c) return false;
	  //On error try it now
	oled.countErr=COUNTERR;
	bool i2c_address;
	do{
	i2c_address=false;
	i2c_master_start();
	i2c_master_writeByte(DISPLAY_ADDRESS);  // SA0 must by 0
	 if(!i2c_master_checkAck())
	  {
		i2c_master_stop();
	  	//DEBUG_OLED("OLED Error write buffer  address:0x%X \n\r",DISPLAY_ADDRESS);
	  	oled.countErr--;
	  	i2c_address=true;
		oled.i2c=0;
	  	//Some delay
	  	os_delay_us(10);
	  	if(oled.countErr<=0){
	  	//DEBUG_OLED("OLED Error write buffer  address:0x%X try it:%d \n\r",DISPLAY_ADDRESS,COUNTERR);
	  	oled.i2c=0;
	   	return false;
	  	}
	  }
	}while(i2c_address);

	oled.countErr=COUNTERR;
        //On error in this moment this is critcal and don't try now return false
	 u16 i;
	 for (i=0;i<len;i++)
	 {
	   i2c_master_writeByte(buffer[i]);
		 if(!i2c_master_checkAck())
		    {
		     	i2c_master_stop();
		       	DEBUG_OLED("OLED Error write buffer :0x%X:0x%X \n\r",i,buffer[i]);
		       //	DEBUG_OLED("OLED ErrCount :%d \n\r",oled.countErr);
		    	oled.i2c=0;
		    	//oled.countErr--;
		       	return false;
		       }

         if(oled.debug){
        	 system_soft_wdt_feed();
        	 DEBUG_OLED("OLED write buffer data :0x%X:0x%X \n\r",i,buffer[i]);
         }
	 }

	  i2c_master_stop();
	  oled.i2c=1;
	  //oled.countErr=COUNTERR;
	  //DEBUG_OLED("OLED Restore ErrCount :%d \n\r",oled.countErr);
	  return true;
}

static void ICACHE_FLASH_ATTR setXY(u8 row, u8 col)
{
	 //DEBUG_OLED("\n\r OLED Set X:Y--> %d:%d",row,col);
	 u8 buffer[]={
	     0x00,  //command mode
		 PAGESTARTADDRESS+row,
		 0x0f&col,
		 0x10 | (0x0f&(col>>4))
	 };
	 //DEBUG_OLED("\n\r High Address: 0x%X",(0x10 | (0x0f&(col>>4))));
	 send_buffer(buffer,sizeof(buffer));
}

 bool ICACHE_FLASH_ATTR send_Char_XY(unsigned char data, u8 x ,u8 y)
{
	//Data falidate
	 if(data<0x20 || data>0x80)
	 {
	   return false;
	 }
	setXY(x,y);
	//DEBUG_OLED("\n\r OLED SendChar 0x%x", data);
	u8 i;
	u8 buffer[9];
	buffer[0]=0x40; //data mode
	for(i=0;i<5;i++)
	{
		buffer[i+1]=myFont_5x7[(data-0x20)*5+i];
		//DEBUG_OLED("\n\r OLED SendChar data: 0x%X:0x%X", ((data-0x20)*5+i),buffer[i+1]);

	}

	return send_buffer(buffer,sizeof(buffer));
}

bool ICACHE_FLASH_ATTR
InverseRow(u8 row)
{
	u8 i=0;  //index
	u8 data[129];
	data[0]=0x40;
	//read row  from oled_buffer
	u16 startAddress=row*128;
	for (i=1;i<129;i++){
	 data[i]=~(oled_buffer[startAddress]);
	 //DEBUG_OLED("Normal :0x%X Inverse :0x%X \n",oled_buffer[startAddress],data[i]);
	 startAddress++;
	}

	//inverse row
	 setXY(row,0);
	//write row

	return send_buffer(data,sizeof(data));
}

bool ICACHE_FLASH_ATTR
ClearDisplay()
 {

	 char clear[3];
	 //Fill empty space with zero;
	 ets_sprintf(clear," "); //-two space

	 displayString(clear,0,0);
	 displayString(clear,1,0);
	 displayString(clear,2,0);
	 displayString(clear,3,0);
	 displayString(clear,4,0);
	 displayString(clear,5,0);
	 displayString(clear,6,0);
	 displayString(clear,7,0);


//	 u8 clearbuffer[DISPLAYHIGH*DISPLAYWIDTH/8 +1];
//	 clearbuffer[0]=0x40;
//	 oled.DEBUG_OLED=0;
//	 //u8 i=0;
//	 u16 k=0;
//	 setXY(0,0);
//	 for (k=1;k<(DISPLAYHIGH*DISPLAYWIDTH/8+1);k++)
//	 {
//		 clearbuffer[k]=0;
//	 }
//
//	 send_buffer(clearbuffer,sizeof(clearbuffer));
//	 DEBUG_OLED("Clear Display bufferSize: 0x%X /n",sizeof(clearbuffer));
	 clearDisplayFlag=0;
//	 oled.DEBUG_OLED=0;
	 return true;
 }

 bool ICACHE_FLASH_ATTR displayString(char *string,u8 x ,u8 y)
 {
     //DEBUG("displayString :%d:%d \n\r",oled.i2c,oled.countErr);
	 if(!oled.i2c || oled.countErr==1){
	 		DEBUG("From displayString i2c oled error I am goining to Init it... \r\n");
	 		initOledDisplay();
	 	}
      //First char
	  oled.debug=0;
	  u8  i=0;
	  u8  j=0;
	  u16 k=0;
	  u8 m=0;
	  u8 len=strlen(string);
	  u8 buffer[129];
	   //DEBUG_OLED("\n\r String len:0x%X",len);
	  buffer[k++]=0x40; //write to GDRAM
	    //first set string possition
	  setXY(x,y);
	  u16 startAddress=x*128+y;
	  for(i=0;i<len;i++)
	   {
		  //send_Char_XY(string[i],x,y);
		  if(string[i]<0x20 || string[i]>0x80){m++;continue;}
		    //DEBUG_OLED("\n\r Char:%c",string[i]);
		  for(j=0;j<5;j++)
		  	{
		  		buffer[k]=myFont_5x7[(string[i]-0x20)*5+j];
		  		//save data to oled_buffer for inversing
		  		oled_buffer[startAddress++]=buffer[k++];
		    }
	   }
	   //Fill empty space with zero;
	  for(i=k;i<129;i++){
	      buffer[i]=0;
	      oled_buffer[startAddress++]=0;
	  }

	  //DEBUG_OLED("\n\r size of buffer:0x%X",sizeof(buffer));
	  //end_buffer(buffer,sizeof(buffer)-5*m);
	 oled.debug=0;
     return send_buffer(buffer,sizeof(buffer));
 }

//static ICACHE_FLASH_ATTR selfTest(void)
//{
//	  displayString("Hello I am here! ",2,0);
//	  //send_Char_XY('E',1,0);
//}

//bool ICACHE_FLASH_ATTR oled_update()
//{
//	 DEBUG_OLED("\n\r OLED Update...");
//	 u8 cmdbuff[]=
//	 {
//	   0x00, //command mode
//	   COLUMNADDR,
//	   0,   //start
//	   DISPLAYWIDTH-1, //end
//	   PAGEADDR,
//	   0,     //start
//	   7      //end
//	 };
//	 send_buffer(cmdbuff,sizeof(cmdbuff));
//	 send_buffer(oled_buffer,sizeof(oled_buffer));
//	 return true;
//}

 bool ICACHE_FLASH_ATTR initOledDisplay(void)
 {
	 //DEBUG("Init Oled Display... \r\n");
	 //Init I2C
	 if(!status.i2c)
	 {
	 i2c_master_init();
	 status.i2c=1;
	 }

	 u8 buffer[]=
	 {
		0x00, //command mode
		TURNOFF,
		//NORMAL,
		//TURNOFF,
		CLOCKDIV,
		0x80,//0x80
		MULTIPLEX,
		0x3F,
		OFFSET,
		0x00,
		STARTLINE,
		CHARGEPUMP,
		0x14,
		MEMORYMODE,
		0x00, //0x00
		SEGREMAP | 0x01,
		COMSCANDEC,
		SETCOMPINS,
		0x12,  //0x12
		SETCONTRAST,
		0xCF,
		PRECHARGE,
		0xF1,
		VCOMDETECT,
		0x40,
		ON_RESUME,
		NORMAL,
		TURNON
	   };
	  oled.i2c=1;
	  oled.on=1;
	  return(send_buffer(buffer,sizeof(buffer)));


	 // oled.countErr=COUNTERR;
	 // DEBUG_OLED("Init OLED....\n\r");
	 //Init Display

	 /*
      u16 i;
      oled_buffer[0]=0x40; //datat command
	  for(i=1;i<sizeof(oled_buffer)-1;i++)
	  {
		  oled_buffer[i]=0; //clear it
	  }
      oled_update();
      */

	  //selfTest();

 }

 bool ICACHE_FLASH_ATTR
 oled_off(){
	 u8 cmdbuff[]={
	 0x00,  //command mode
	 TURNOFF
	 };
	 oled.on=0;
	 return  send_buffer(cmdbuff,sizeof(cmdbuff));
 }

 bool ICACHE_FLASH_ATTR
 oled_on(){
	 u8 cmdbuff[]={
	 0x00,  //command mode
	 TURNON
	 };
	 //send_buffer(cmdbuff,sizeof(cmdbuff));
	 oled.on=1;
	 return send_buffer(cmdbuff,sizeof(cmdbuff));

 }

 bool ICACHE_FLASH_ATTR
 oled_contrast(u8 contrast){
	 u8 cmdbuff[]={
	 0x00,
	 SETCONTRAST,
	 contrast
	 };
	 return send_buffer(cmdbuff,sizeof(cmdbuff));
 	//return true;
 }

