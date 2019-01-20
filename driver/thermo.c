/*

 * thermo.c
 *
 *  Created on: 28.12.2018 �.
 *      Author: admin
 */

#include "user_interface.h"
#include "user_config.h"
#include "driver/nextion.h"
#include "driver/thermo.h"

void ICACHE_FLASH_ATTR
controllerOn()
{
    //static float thermo_err = 0.0;

    //float currentThemp;
    float offSet=0.5;
    //currentThemp = atoi(temperature);
    // if(currentTemperature<(themperatureSet*1.0+offSet) || currentTemperature<(themperatureSet*1.0-offSet)){
    //    power=1;
    // }
    
    if(power==1){
        if(currentTemperature>(themperatureSet*1.0+offSet)){         
            power=0;
        }
    }else{
        if(currentTemperature<(themperatureSet*1.0-offSet)){      
           power=1;
        }
    }

    if(power==1){
       POWER_ON;
       nextion_power_on();
    }else{
       POWER_OFF;
       nextion_power_off();
    }

    // thermo_err = themperatureSet*1.0 - currentTemperature;  //20 -19.9 =0.1
    // if(thermo_err<0)thermo_err=thermo_err*(-1.0);
    // if (thermo_err < 0.5)
    // {
    //     POWER_ON;
    //     nextion_power_on();
    //     power=1;

    // }
    // else
    // {
    //     POWER_OFF;
    //     nextion_power_off();
    //     power=0;
    // }
}