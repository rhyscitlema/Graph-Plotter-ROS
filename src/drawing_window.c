/*
    drawing_window.c
*/

#include <drawing_window.h>
#include <userinterface.h>
#include <graphplotter.h>
#include <timer.h>
#include <mouse.h>
#include <_malloc.h>

#include "screen.h"


/*static*/ void drawing_window_close (DrawingWindow *dw)
{
    Object3D *camera = (Object3D*) findCameraFromDW(dw);
    userinterface_display_object (camera, false);
    object_remove (camera);
}


/*static*/ void drawing_window_resize (DrawingWindow *dw)
{ 
    if(graphplotter_remaining_updates<=0
    && timer_paused()) graphplotter_update();
}


DrawingWindow *drawing_window_new ()
{
    return (DrawingWindow*)1;
}


bool drawing_window_edit (DrawingWindow *dw)
{
    Camera *cmr;

    cmr = findCameraFromDW(dw);
    if(!cmr) return false;

    return true;
}


void drawing_window_draw (DrawingWindow *dw)
{
    Camera* cmr = findCameraFromDW (dw);
    if(!cmr) return ;
    screen_putpixels (cmr->pixelColour, cmr->XPost, cmr->YPost, cmr->XSize, cmr->YSize, cmr->XSize);
    graphplotter_remaining_updates--;
}


void drawing_window_remove (DrawingWindow *dw)
{
    if(dw == NULL) return;
    dw = NULL;
}

