/**************************************************
    hardware.c
**************************************************/

#include "../hal/hal.h"
#include "hardware.h"

/* initialise hardware environment */
void hardware_initialise()
{
    hal_initialise();
}

/* end hardware environment */
void hardware_shutdown()
{
    hal_shutdown();
}
