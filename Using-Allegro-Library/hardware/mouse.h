#ifndef _MOUSE_H
#define _MOUSE_H
/**************************************************
    mouse.h
**************************************************/

#include <my_stdint.h>

extern int* mouse_get_motion();

// true if left button currently clicked
extern bool mouse_get_left();

// true if middle button currently clicked
extern bool mouse_get_middle();

// true if right button currently clicked
extern bool mouse_get_right();

// true if 4th button currently clicked
extern bool mouse_get_4th();

// true if 5th button currently clicked
extern bool mouse_get_5th();

// clear mouse dx, dy and dz values to zeros
extern void mouse_clear_motion();

// clear all mouse buttons to false
extern void mouse_clear_buttons();

// enable/disable mouse hardware
extern void mouse_disable(bool bl);

// update mouse input
extern void mouse_update();

// install mouse hardware driver
extern void mouse_install();


#endif
