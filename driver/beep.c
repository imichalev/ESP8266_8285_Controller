/*

 * beep.c
 *
 *  Created on: 8.05.2017 ã.
 *      Author: admin
 */

#include "driver/beep.h"
#include "user_interface.h"
#include "driver/pwm.h"
#include "osapi.h"
//#include <Timer.h>


#define PWM_0_OUT_IO_MUX  PERIPHS_IO_MUX_MTMS_U
#define PWM_0_OUT_IO_NUM 14
#define PWM_0_OUT_IO_FUNC FUNC_GPIO14
#define PWM_CHANNELS 1


uint32 io_info[PWM_CHANNELS][3] = {   {PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM}};
u32 duty[PWM_CHANNELS] = {2000};
static os_timer_t beepTimer;

 void ICACHE_FLASH_ATTR
  beepIsrTimer(void)
 {
	 os_timer_disarm(&beepTimer);
	 //RTC_CLR_REG_MASK(FRC1_CTRL_ADDRESS, FRC1_ENABLE_TIMER);
	 //pwm_set_duty(0,1);
	 //pwm_start();
	 pwm_stop();
	 return;
 }


  void Beep(u16 delay)
{
	  return;
	  //gpio16_input_conf();
      pwm_init(3000,duty,PWM_CHANNELS,io_info);
      pwm_set_duty(2000, 1);
      pwm_start();

      os_timer_disarm(&beepTimer);
      os_timer_setfn(&beepTimer,beepIsrTimer,NULL);
   	  os_timer_arm(&beepTimer,delay,0);

   	  //pwm_set_duty(0,1);
   	  //pwm_start();

      //os_delay_us(delay*1000);
      //pwm_stop();
      return;
}


