/*
    user_main_gui.c
*/

#include "../hardware/screen.h"
#include "user_main_gui.h"
#include "terminal.h"
#include "mcursor.h"
#include "tools.h"
#include "files.h"

#include <_stdio.h>
#include <mouse.h>
#include <keyboard.h>
#include <graphplotter.h>
#include <userinterface.h>



void userinterface_start ()
{
    int count=0;
    mchar str[100];


    /* initialise system utilities */
    tools_initialise();
    mcursor_initialise();
    //terminal_initialise();

    puts0("  Rhyscitlema Graph Plotter 3D    \n");
    puts0("  Plot 3D graphs of equations in x, y and z.\n");
    sprintf0((char*)str, "  Resolution = (%d, %d)\n", SCR.XRes, SCR.YRes);
    puts0((char*)str);
    puts0("  Press Escape to Quit.\n");

    strcpy0(str,"Initial_graphs_and_cameras.txt");
    load_all_objects(str);


    /* initialise user inputs */
    clearkey();
    mouse_clear_motion(headMouse);
    mouse_clear_buttons(headMouse);

    while(1)
    {
        /* update user inputs */
        kybrd_update();
        mcursor_process();      // TODO: the order in the calls of these
        mouse_process_all();    // two functions cannot be arbitrary

        /* System response to user input */
        if(getkey()>>8 == KEY_ESC)
            break;

        /* update system logic */
        time_set_time (timer_get_time() + (number)0.05);
        graphplotter_update();

        set_debug_Pos(8*25, 16*5);
        sprintf0((char*)str, " Frame = %d  ",count);
        puts0((char*)str);
        count++;

        mcursor_paint();
        screen_refresh();
    }
}
