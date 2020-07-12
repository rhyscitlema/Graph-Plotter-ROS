/*
    userinterface.c

    Rhyscitlema Graph Plotter 3D
    Plot 3D graphs of equations in x, y and z,
    and make everything vary with time.
*/

#include <userinterface.h>
#include <_stdio.h>



void userinterface_display_camera (Camera *cmr, bool exists)
{
}

void userinterface_display_object (Object3D *obj, bool exists)
{
}

void userinterface_display_other  (const mchar* text)
{
}

void display_error_message (const mchar* error_message)
{ puts1(error_message); }



void calculator_evaluate_display (bool parse)
{
}

void userinterface_process ()
{
}



mchar* userinterface_get_text (UI_ITEM ui_item, mchar* buffer, int length)
{
    return NULL;
}

void   userinterface_set_text (UI_ITEM ui_item, const mchar* buffer, int length)
{
}

void   userinterface_release_text (mchar* buffer)
{
}

