/*
    drawing_window.c
*/

#include <drawing_window.h>
#include <userinterface.h>
#include <graphplotter.h>
#include <timer.h>
#include <mouse.h>
#include <_malloc.h>
#include "user_main_gui.h"

#include "../hardware/screen.h"



static void drawing_window_close (DrawingWindow *dw)
{
    Camera *cmr = findCameraFromDW(dw);
    userinterface_display_camera (cmr, FALSE);
    camera_remove(cmr);
}



static void drawing_window_resize (DrawingWindow *dw)
{ 
    //Camera *cmr = findCameraFromDW(dw);
    //gtk_window_get_size (GTK_WINDOW(dw->window), &cmr->newXSize, &cmr->newYSize);
    if(!timer_isRunning()) graphplotter_update();
}



DrawingWindow *drawing_window_new ()
{
    return (DrawingWindow*)1;
}



bool drawing_window_edit (DrawingWindow *dw)
{
    Camera *cmr;

    cmr = findCameraFromDW(dw);
    if(!cmr) return FALSE;

    return TRUE;
}



void drawing_window_draw (DrawingWindow *dw)
{
    Camera* cmr = findCameraFromDW (dw);
    if(!cmr) return ;
    screen_putpixels (cmr->closestColour, cmr->XStart, cmr->YStart, cmr->XSize, cmr->YSize, cmr->XSize);
    graphplotter_remaining_updates--;
}



void drawing_window_remove (DrawingWindow *dw)
{
    if(dw == NULL) return;
    dw = NULL;
}

