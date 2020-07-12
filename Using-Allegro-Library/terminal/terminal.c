/******************************************************************************
    terminal.c
    Command line / Shell
    
    This file needs some re-organisation!
******************************************************************************/

#include <my_string.h>                  // used for trim() only
#include <my_stdio.h>
#include "terminal.h"
#include "../hardware/mouse.h"
#include "../hardware/screen.h"
#include "../hardware/keyboard.h"

#include "charfont.h"





/*
    debug_putchar( ,chr) function.
    Writes directly to main screen.
    Whatever calls this function should be
    willing to make a pause, like wait_key_stroke(),
    else the result will be wiped out.
*/

int debug_XPos=0;     // X pixel position
int debug_YPos=0;     // Y pixel position

void set_debug_Pos(int x, int y)
{ debug_XPos=x; debug_YPos=y; }

int debug_putchar(void* debugger, const char chr)
{
    int i, j;
    unsigned char x, c=chr;

    if(debug_XPos<0 || debug_XPos > SCR.XRes)           // if out of bounds
    { debug_XPos = 0;  debug_YPos += YCharSize; }
    if(debug_YPos<0 || debug_YPos > FREE_YZONE-YCharSize)
        debug_YPos = 0;

    if(c=='\n')
    {   debug_XPos = 0;                                 // go to newline
        debug_YPos += YCharSize;
        return 0;
    }

    for(j=0; j<YCharSize; j++)
     for(i=0, x=1<<(XCharSize-1) ; x!=0 ; x>>=1, i++)   // x denotes a bit position
     {
        if(charfont[c][j] & x)                          // check bit with x
            my_putpixel(debug_XPos+i, debug_YPos+j+SCR.YRes, 0x00FFFFFF);
        else                                            // y-pixel is after a length of 'SCR.YRes'
            my_putpixel(debug_XPos+i, debug_YPos+j+SCR.YRes, 0x00000000);
    }
    debug_XPos += XCharSize;   // go right = get ready for next character
    return 0;
}

