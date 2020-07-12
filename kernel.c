/*
   kernel.c
*/

#include <_malloc.h>
#include <timer.h>
#include <mouse.h>
#include "src/screen.h"
#include "hal/hal.h"

static void userinterface_start();


#define KERNEL_LOCATION  0x00100000 /* 0x00100000 <= kernel space  <= kernel file size */
#define STACK_LOCATION   0x00800000 /* 0x00100000 <= stack space   <= 0x00800000 (stack goes downward) */
#define SCREEN_BUFFER    0x00800000 /* 0x00800000 <= screen buffer <= 0x01000000 */
#define FREE_MEMORY      0x01000000 /* 0x01000000 <= FreeMemory    <= RAM total size */
#define FREE_MEMORY_SIZE 0x08000000


/* kernel entry point is called by the kernel loader
 * important: both must agree on the KERNEL_LOCATION.
 * kernel.c and screen.c must agree on value of SCREEN_BUFFER
 */


void main()
{
    ASM_BEG
    ASM(cli)            // make sure interrupts are cleared
    ASM(mov  ax, 0x10)  // refresh all segment registers
    ASM(mov  ds, ax)    // to the data descriptor.
    ASM(mov  es, ax)
    ASM(mov  fs, ax)
    ASM(mov  gs, ax)
    ASM(mov  ss, ax)
    ASM(mov  esp, 0x00800000) //STACK_LOCATION
    ASM(mov  ebp, esp)  // For fail-safeness, store the current stack frame pointer,
    ASM(push ebp)       // this will insure that any routines we call has a place to return to.
    ASM_END

    /* install or initialise screen device */
    screen_install();

    useFreeMemory ((void*)FREE_MEMORY, FREE_MEMORY_SIZE);

    /* initialise hardware environment */
    hal_initialise();

    /* install kybrd device */
    kybrd_install();

    /* install mouse device */
    mouse_install();

    /* install timer device */
    timer_install(0);

    /* start software environment */
    userinterface_start();

    /* terminate hardware environment */
    hal_shutdown();
}


#include <_stdio.h>
#include <mouse.h>
#include <timer.h>
#include <graphplotter.h>
#include <userinterface.h>
#include <tools.h>
#include <files.h>

#include "src/terminal.h"
#include "src/mcursor.h"

extern int load_all_objects (const mchar* fileName); // in files.c
extern int getkey();        // in keyboard.c
extern void clearkey();     // in keyboard.c
extern void kybrd_update(); // in keyboard.c


static void userinterface_start()
{
    int count=0;
    mchar str[100];


    /* initialise system utilities */
    tools_initialise(NULL);
    mcursor_initialise();
    //terminal_initialise();

    puts1("  Rhyscitlema Graph Plotter 3D    ");
    puts1("  Plot 3D graphs of equations in x, y and z.");
    sprintf1((char*)str, "  Resolution = (%d, %d)", SCR.XRes, SCR.YRes);
    puts1((char*)str);
    puts1("  Press Escape to Quit.");

    strcpy21(str,"Initial_graphs_and_cameras.txt");
    load_all_objects(str);


    /* initialise user inputs */
  //clearkey();
    mouse_clear_motion(headMouse);
    mouse_clear_buttons(headMouse);

    while(1)
    {
        /* update user inputs */
      //kybrd_update();
        mcursor_process();      // TODO: the order in the calls of these
      //mouse_process_all();    // two functions cannot be arbitrary

        /* System response to user input */
        //if(getkey()>>8 == Key_Escape) break;

        /* update system logic */
        timer_set_time (timer_get_time() + 0.05);
        graphplotter_update();

        set_debug_Pos(8*25, 16*5);
        sprintf1((char*)str, " Frame = %d  ",count);
        putstr1((char*)str);
        count++;

        mcursor_paint();
        screen_refresh();
    }
}

