/**************************************************
    mouse.c
**************************************************/

#include <allegro/mouse.h>      // use Allegro library for mouse hardware access
#include "mouse.h"
#include "screen.h"             // used by mouse_update() only

static int mouse_motion[3];     // (dx,dy,dz) relative motion

static bool mouse_left, mouse_middle, mouse_right;  // first three buttons
static bool mouse_4th, mouse_5th;                   // extra buttons
static bool mouse_disabled = true;      // assumed disabled until when installed
static bool Zmotion_enabled = false;
static bool extra_buttons_enabled = false;


int* mouse_get_motion()
{ return mouse_motion; }

bool mouse_get_left()           // true if 'left' button currently clicked
{ return mouse_left; }

bool mouse_get_middle()         // true if 'middle' button currently clicked
{ return mouse_middle; }

bool mouse_get_right()          // true if 'right' button currently clicked
{ return mouse_right; }

bool mouse_get_4th()            // true if '4th' button currently clicked
{ return mouse_4th; }

bool mouse_get_5th()            // true if '5th' button currently clicked
{ return mouse_5th; }


void mouse_clear_motion()       // clear mouse dx, dy and dz values to zeros
{ mouse_motion[0] = mouse_motion[1] = mouse_motion[2] = 0; }

void mouse_clear_buttons()      // clear all mouse buttons to false
{ mouse_left = mouse_middle = mouse_right = mouse_4th = mouse_5th = false; }


void mouse_update()             // update mouse input
{
    if(mouse_disabled) return;
    poll_mouse();                               // call Allegro function
    mouse_motion[0] = mouse_x - (SCR.XRes>>1);  // get relative from absolute motion
    mouse_motion[1] = ((SCR.YRes+FREE_YZONE)>>1) - mouse_y;
    mouse_motion[2] = mouse_z;
    position_mouse(SCR.XRes>>1, (SCR.YRes+FREE_YZONE)>>1);
    position_mouse_z(0);
    mouse_left   = (mouse_b & 1) ? true:false;
    mouse_right  = (mouse_b & 2) ? true:false;
    mouse_middle = (mouse_b & 4) ? true:false;
}

void mouse_install()            // install mouse hardware driver
{
    install_mouse();            // install Allegro mouse
    poll_mouse();               // call Allegro function (not really needed though!)
    position_mouse(SCR.XRes>>1, (SCR.YRes+FREE_YZONE)>>1);
    position_mouse_z(0);
    mouse_disable(false);
    Zmotion_enabled = true;
    extra_buttons_enabled = false;
}

void mouse_disable(bool bl)     // enable/disable mouse hardware
{
    mouse_disabled = bl;
    mouse_clear_motion();       // initialise values
    mouse_clear_buttons();
}

