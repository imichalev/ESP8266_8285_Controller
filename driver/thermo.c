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
    static float thermo_err = 0.0;
    float currentThemp;
    currentThemp = atoi(temperature);

    thermo_err = themperatureSet - currentThemp;
    if (thermo_err > 0)
    {
        POWER_ON;
        nextion_power_on();
        power=1;

    }
    else
    {
        POWER_OFF;
        nextion_power_off();
        power=0;
    }
}