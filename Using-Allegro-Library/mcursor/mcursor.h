#ifndef _MCURSOR_H
#define _MCURSOR_H
/**************************************************
    mcursor.h
    mouse cursor as a 2D object on screen
    Note: mouse cursor is NOT a world object
**************************************************/

#include <my_stdint.h>

// current and previous mouse cursor postion
// note: relavite to screen/camera origin and local axes
extern int mcursorPos[2];
extern int mcursorPrevPos[2];

// get mouse cursor position
extern int* mcursor_get_position();

// initialise mouse cursor
extern void mcursor_initialise();

// enable/disable mouse cursor
extern void mcursor_disable(bool bl);

// reset mouse cursor position
extern void mcursor_reset();

// update mouse cursor position
extern void mcursor_process();

// paint mouse cursor image
extern void mcursor_paint();

#endif
