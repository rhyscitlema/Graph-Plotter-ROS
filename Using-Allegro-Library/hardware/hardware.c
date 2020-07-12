/**************************************************
    hardware.c
**************************************************/

#include <allegro.h>    // use Allegro library for hardware access
#include "hardware.h"


/* initialise hardware environment */
void hardware_initialise()
{
    // initialise Allegro equipments
    allegro_init();
}

/* end hardware environment */
void hardware_shutdown()
{
    // Shut down Allegro
    allegro_exit();
    exit(0);
}

