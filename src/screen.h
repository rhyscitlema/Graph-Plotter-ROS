#ifndef _SCREEN_H
#define _SCREEN_H
/**************************************************
    screen.h
**************************************************/


extern void screen_install ();          // install or initialise screen hardware device

extern void screen_refresh ();          // update changes of screen buffer to real screen

extern void screen_putpixels (const int* pixels_array, int XStart, int YStart, int XSize, int YSize, int row_stride);

extern void screen_putpixel (int x, int y, int colour);


// reserved screen space for system usage
// (see debug_putchar in terminal.h)
#define FREE_YZONE (16*6)


/* Information about the computer SCReen (from screen.c)
   Used only by the Operating System form of Rhyscitlema.
*/
struct _screen_info
{
    int XRes; // in pixels
    int YRes; // in pixels
};
extern struct _screen_info SCR;


#endif
