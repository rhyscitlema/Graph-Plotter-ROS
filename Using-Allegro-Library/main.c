/*****************************************
   main.c
*****************************************/

#include "./hardware/hardware.h"
#include "./hardware/screen.h"
#include "./hardware/mouse.h"
#include "./hardware/keyboard.h"
extern void graphplotter();

char FREEMEMORY[5000000]; // XRes*YRes*25
char* FreeMemory;

/* rhyscitlema software entry point */
int main ()
{
    FreeMemory = FREEMEMORY;

    // initialise hardware environment
	hardware_initialise();

    // install screen driver
    screen_install();

    // install keyboard driver
    kybrd_install();

    // install mouse driver
    mouse_install();

    // start software environment
    graphplotter();

    // end hardware environment
    hardware_shutdown();
    return 0;
}
