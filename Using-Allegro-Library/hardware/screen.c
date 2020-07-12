/*******************************************
    screen.c
*******************************************/

#include <allegro/graphics.h>   // use Allegro library for graphics hardware access
#include <allegro.h>            // only to use the variable allegro_error
#include "screen.h"
#include <stdio.h>              // only to use printf()

extern void hardware_shutdown();


#define BPP 32
#define ScreenXRes 800//1280//800
#define ScreenYRes 600//800//600
struct _screen_info SCR;        // SCReen information, also used outside this file
BITMAP* ScreenBuffer;

/* install screen hardware driver */
void screen_install ()
{
    set_color_depth(BPP);

    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, ScreenXRes, ScreenYRes, 0, 0) != 0)
    {
        printf("Error setting graphics mode %dx%dx%d\n", ScreenXRes, ScreenYRes, BPP);
        printf("%s\n\n", allegro_error);
        hardware_shutdown();               // Print error message and exit program
    }

    ScreenBuffer = create_bitmap(ScreenXRes, ScreenYRes);  // Create the screen buffer

    SCR.XRes   = ScreenXRes;
    SCR.YRes   = ScreenYRes - FREE_YZONE;   // reserve screen space for system usage
}


void screen_refresh()                       // update changes to 'real' screen
{ blit(ScreenBuffer, screen, 0, 0, 0, 0, ScreenXRes, ScreenYRes); } // call Allegro function


void my_putpixel(int x, int y, int colour)  // put one pixel on 'buffer' screen
{ putpixel(ScreenBuffer, x, y, colour); }   // call Allegro function
