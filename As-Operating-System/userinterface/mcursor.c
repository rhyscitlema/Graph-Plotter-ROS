/**************************************************
    mcursor.c
    mouse cursor as a 2D object on screen
    mouse cursor is NOT a rhyscitlema 3D object
**************************************************/

#include "mcursor.h"
#include "mcursor_image.h"
#include <mouse.h>
#include "../hardware/screen.h"


// current and previous mouse cursor postion
// note: relavite to screen/camera center/origin and local axes
int mcursorPos[2];
int mcursorPrevPos[2];

bool mcursor_disabled=true;     // assumed disabled until when initialised



// initialise mouse cursor
void mcursor_initialise()
{
    mcursor_disable(false);
    mcursor_reset();
}

// enable/disable mouse cursor
void mcursor_disable(int bl)
{   mcursor_disabled = bl;
}

// get mouse cursor position
int* mcursor_get_position()
{ return mcursorPos; }

// reset mouse cursor position
void mcursor_reset()
{ mcursorPos[0] = mcursorPos[1] = 0;
  mcursorPrevPos[0] = mcursorPrevPos[1] = 0;
}



// update mouse cursor position
void mcursor_process()
{
    int* move = headMouse->motion;

    mcursorPrevPos[0] = mcursorPos[0];
    mcursorPrevPos[1] = mcursorPos[1];
    mcursorPos[0] += move[0];               // update x position
    mcursorPos[1] += move[1];               // update y position

    if(mcursorPos[0] > (SCR.XRes>>1)-2)     // check if out of bounds (hard coded!)
        mcursorPos[0] = (SCR.XRes>>1)-2;
    else if(mcursorPos[0] < -(SCR.XRes>>1))
        mcursorPos[0] = -(SCR.XRes>>1);

    if(mcursorPos[1] > (SCR.YRes>>1)-1)
        mcursorPos[1] = (SCR.YRes>>1)-1;
    else if(mcursorPos[1] < 2-(SCR.YRes>>1))
        mcursorPos[1] = 2-(SCR.YRes>>1);

    mouse_clear_motion(headMouse);
    headMouse->previousPosition[0] = mcursorPrevPos[0];
    headMouse->previousPosition[1] = mcursorPrevPos[1];
    headMouse->currentPosition[0]  = mcursorPos[0];
    headMouse->currentPosition[1]  = mcursorPos[1];
}


// paint mouse cursor image
void mcursor_paint()
{
    int i, j, x, y;
    if(mcursor_disabled) return;
    x = (SCR.XRes>>1) + mcursorPos[0];
    y = (SCR.YRes>>1)-1-mcursorPos[1];
    for(j=0; j<MCI_YSize; j++)
     for(i=0; i<MCI_XSize; i++)
      if(i+x<SCR.XRes && j+y<SCR.YRes)
       if(mcursor_image[j][i]!=0xFF000000)
        screen_putpixel (i+x, j+y, mcursor_image[j][i]);
}
