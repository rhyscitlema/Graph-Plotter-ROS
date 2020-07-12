#ifndef _SCREEN_H
#define _SCREEN_H
/**************************************************
    screen.h
**************************************************/

// Pointer to free memory. Declared in the program entry file.
// Mainly used to allocate memory for screen operations.
extern char* FreeMemory;

// reserved screen space for system usage
// (see debug_putchar in terminal.h)
#define FREE_YZONE 96

struct _screen_info
{   int XRes;       // horizontal resolution in pixels
    int YRes;       // vertical resolution in pixels - FREE_YZONE
};
extern struct _screen_info SCR;         // SCReen information, found in screen.c

extern void screen_install ();          // install screen hardware driver

extern void screen_refresh ();          // update changes of screen buffer to real screen

extern void my_putpixel(int x, int y, int colour); // put one pixel to screen buffer

#endif
