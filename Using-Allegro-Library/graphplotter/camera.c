/*
    camera.c
*/

#include <stdio.h>      // for printf()
#include "camera.h"
#include "expression.h"
#include "parsedsignature.h"
#include "../hardware/mouse.h"
#include "../hardware/keyboard.h"
#include "../hardware/screen.h"



    Camera *headCamera = NULL;
    char Camera_errorMessage[1000];



    void paint_background(Camera *cmr)
    {
        int x, y, pixel, colour=0;
        double s, t;
        double CmrOrg[3];
        double CmrOr2[3];
        double CmrDir[3];
        double pixelDirectionX[1280*3];
        double pixelDirectionY[800*3];

        CmrDir[0] = cmr->Zaxis[0] * cmr->ZSize;
        CmrDir[1] = cmr->Zaxis[1] * cmr->ZSize;
        CmrDir[2] = cmr->Zaxis[2] * cmr->ZSize;

        CmrOrg[0] = CmrDir[0] - cmr->origin[0] + WORLD_LIMIT;
        CmrOr2[0] = CmrDir[0] - cmr->origin[0] - WORLD_LIMIT;
        CmrOrg[1] = CmrDir[1] - cmr->origin[1] + WORLD_LIMIT;
        CmrOr2[1] = CmrDir[1] - cmr->origin[1] - WORLD_LIMIT;
        CmrOrg[2] = CmrDir[2] - cmr->origin[2] + WORLD_LIMIT;
        CmrOr2[2] = CmrDir[2] - cmr->origin[2] - WORLD_LIMIT;

        for(y=0; y < cmr->YSize; y++)
        {
            t = ((cmr->YSize)>>1)-1-y;
            pixelDirectionY[y*3+0] = cmr->Yaxis[0]*t + CmrDir[0];
            pixelDirectionY[y*3+1] = cmr->Yaxis[1]*t + CmrDir[1];
            pixelDirectionY[y*3+2] = cmr->Yaxis[2]*t + CmrDir[2];
        }
        for(x=0; x < cmr->XSize; x++)
        {
            t =  ((cmr->XSize)>>1)-1-x;
            pixelDirectionX[x*3+0] = cmr->Xaxis[0]*t;
            pixelDirectionX[x*3+1] = cmr->Xaxis[1]*t;
            pixelDirectionX[x*3+2] = cmr->Xaxis[2]*t;
        }

        for(y=0; y < cmr->YSize; y++)
        for(x=0; x < cmr->XSize; x++)
        {
            // if an object has already paint itself to this pixel, then skip it
            pixel = y * cmr->XSize + x;
            if(cmr->closestDistance[pixel] < WORLD_LIMIT)
                continue;

            CmrDir[0] = pixelDirectionX[x*3+0] + pixelDirectionY[y*3+0];
            CmrDir[1] = pixelDirectionX[x*3+1] + pixelDirectionY[y*3+1];
            CmrDir[2] = pixelDirectionX[x*3+2] + pixelDirectionY[y*3+2];

            // now find the value of t for which the Cmr line intersects
            t = WORLD_LIMIT;

            s = CmrOrg[0]/CmrDir[0];        // for limit of +ve Xaxis
            if(s>=1)
            { if(s<t) { t=s; colour = 0x006F6F00; } }
            else
            {   s = CmrOr2[0]/CmrDir[0];    // for limit of -ve Xaxis
                if(s>=1 && s<t)
                { t=s; colour = 0x006F006F; }
            }

            s = CmrOrg[1]/CmrDir[1];        // for limit of +ve Yaxis
            if(s>=1)
            { if(s<t) { t=s; colour = 0x00006F00; } }
            else
            {   s = CmrOr2[1]/CmrDir[1];    // for limit of -ve Yaxis
                if(s>=1 && s<t)
                { t=s; colour = 0x0000006F; }
            }

            s = CmrOrg[2]/CmrDir[2];        // for limit of +ve Zaxis
            if(s>=1)
            { if(s<t) { t=s; colour = 0x00888888; } }
            else
            {   s = CmrOr2[2]/CmrDir[2];    // for limit of -ve Zaxis
                if(s>=1 && s<t)
                { t=s; colour = 0x00000000; }
            }

            cmr->closestColour[pixel] = colour;
        }
    }

    //static Camera checkRangeForAllCameras(int x, int y) {}




    /*double magnitude (double[] u)
    { return mysqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]); }

    void crossproduct (double[] out, double[] u, double[] v)
    {   out[0] = u[1]*v[2] - u[2]*v[1];
        out[1] = u[2]*v[0] - u[0]*v[2];
        out[2] = u[0]*v[1] - u[1]*v[0];
    }*/



    static void updateLocalAxes (Camera *cmr)
    {
        double Xaxis[3], Yaxis[3];
        double Xmag, Ymag;

        Xaxis[0] = -cmr->Zaxis[1];
        Xaxis[1] = cmr->Zaxis[0];           // XAxis = crossproduct((0,0,1), Zaxis)
        Xaxis[2] = 0; 
        crossproduct(Yaxis, cmr->Zaxis, Xaxis);
        Xmag = magnitude(Xaxis);
        Ymag = magnitude(Yaxis);

        // now set Xaxis to Rot vector in its world coordinates form
        Xaxis[0] = Xaxis[0]*cmr->rotation[0]/Xmag + Yaxis[0]*cmr->rotation[1]/Ymag;
        Xaxis[1] = Xaxis[1]*cmr->rotation[0]/Xmag + Yaxis[1]*cmr->rotation[1]/Ymag;
        Xaxis[2] = Xaxis[2]*cmr->rotation[0]/Xmag + Yaxis[2]*cmr->rotation[1]/Ymag;
        crossproduct(Yaxis, cmr->Zaxis, Xaxis);

        cmr->Xaxis[0] = Xaxis[0]; cmr->Xaxis[1] = Xaxis[1]; cmr->Xaxis[2] = Xaxis[2];
        cmr->Yaxis[0] = Yaxis[0]; cmr->Yaxis[1] = Yaxis[1]; cmr->Yaxis[2] = Yaxis[2];
    }


    // relative shift (delta x, delta y, delta z) to change object origin vector Org only
    static void shift (Camera *cmr, double dx, double dy, double dz)
    {
        // LOCAL_TO_WORLD coordinate transformation
        cmr->origin[0] += cmr->Xaxis[0]*dx + cmr->Yaxis[0]*dy + cmr->Zaxis[0]*dz;
        cmr->origin[1] += cmr->Xaxis[1]*dx + cmr->Yaxis[1]*dy + cmr->Zaxis[1]*dz;
        cmr->origin[2] += cmr->Xaxis[2]*dx + cmr->Yaxis[2]*dy + cmr->Zaxis[2]*dz;
    }


    // relative turn (delta x, delta y) to change object direction vector Zax only
    // dz = change in size of object
    static void turn (Camera *cmr, double dx, double dy, double dz)
    {
        double newZax[3];
        double magn;

        cmr->ZSize += dz; if(cmr->ZSize<1) cmr->ZSize=1;
        if(dx==0 && dy==0) return;

        // LOCAL_TO_WORLD coordinate transformation
        newZax[0] = cmr->Zaxis[0]*500 + cmr->Xaxis[0]*dx + cmr->Yaxis[0]*dy;
        newZax[1] = cmr->Zaxis[1]*500 + cmr->Xaxis[1]*dx + cmr->Yaxis[1]*dy;
        newZax[2] = cmr->Zaxis[2]*500 + cmr->Xaxis[2]*dx + cmr->Yaxis[2]*dy;

        if(newZax[0]==0 && newZax[1]==0)    // if newZax == world z-axis, then keep previous configurations
            return;

        magn = magnitude(newZax);
        newZax[0] = newZax[0]/magn;
        newZax[1] = newZax[1]/magn;
        newZax[2] = newZax[2]/magn;

        if((int)(5*newZax[0])==0 && (int)(5*newZax[1])==0)    // oh, we have to check again!
            return;
        cmr->Zaxis[0] = newZax[0];
        cmr->Zaxis[1] = newZax[1];
        cmr->Zaxis[2] = newZax[2];
        updateLocalAxes(cmr);
    }


    // relative rotation (change in angle) so to update object rotation vector Rot only
    static void rotate (Camera *cmr, double relative_angle)
    {
        cmr->rotation[2] += relative_angle;
        cmr->rotation[0] = cos(cmr->rotation[2]*PI/180.0); // angle given in degrees
        cmr->rotation[1] = sin(cmr->rotation[2]*PI/180.0); // then converted to radians
        updateLocalAxes(cmr);
    }
    static void setRotation (Camera *cmr, double absolute_angle)
    {
        cmr->rotation[2] = absolute_angle;
        cmr->rotation[0] = cos(cmr->rotation[2]*PI/180.0); // angle given in degrees
        cmr->rotation[1] = sin(cmr->rotation[2]*PI/180.0); // then converted to radians
        updateLocalAxes(cmr);
    }




Camera *removed_camera = NULL;

Camera *newCamera ()
{
    Camera *cmr;
    if(removed_camera==NULL)
    {   cmr = (Camera*)FreeMemory;
        FreeMemory += sizeof(Camera);
    }
    else
    {   cmr = removed_camera;
        removed_camera = removed_camera->next;
    }
    return cmr;
}

void freeCamera (Camera *cmr)
{
    cmr->prev = NULL;
    if(removed_camera!=NULL)
        removed_camera->prev = cmr;
    cmr->next = removed_camera;
    removed_camera = cmr;
}





bool editCamera (Camera *cmr, char* _signature)
{
    int arrayIndex[4];
    Component *_origin_comp, *_Zaxis_comp, *_rotation_comp;
    Component *_ZSize_comp, *_XStart_YStart, *_XSize_YSize;
    ParsedSignature *_myPS;

    _myPS = parseSignature (_signature);
    //if(_myPS==NULL) { strcpy(Camera_errorMessage, ParsedSignature_errorMessage); return false; }

    Expression_parsedSignature = _myPS;
    _origin_comp   = parseComponent (_myPS, "position",       arrayIndex, 0);
    _Zaxis_comp    = parseComponent (_myPS, "direction",      arrayIndex, 0);
    _rotation_comp = parseComponent (_myPS, "rotation_angle", arrayIndex, 0);
    _ZSize_comp    = parseComponent (_myPS, "zoom_amount",    arrayIndex, 0);
    _XStart_YStart = parseComponent (_myPS, "top_left_point", arrayIndex, 0);
    _XSize_YSize   = parseComponent (_myPS, "width_height",   arrayIndex, 0);

    if(_origin_comp==NULL || _Zaxis_comp==NULL || _rotation_comp==NULL
    || _ZSize_comp==NULL || _XStart_YStart==NULL || _XSize_YSize==NULL)
    { strcpy (Camera_errorMessage, ParsedSignature_errorMessage); return false; }

    if(cmr->XSize==0 && cmr->YSize==0) // if first time
    {
        if((_XStart_YStart->qualifier & TIME_DEPENDENT)!=0)
        { strcpy (Camera_errorMessage, "top_left_point must evaluate to two 'finite' numbers."); return false; }

        if((_XSize_YSize->qualifier & TIME_DEPENDENT)!=0)
        { strcpy (Camera_errorMessage, "width_height must evaluate to two 'finite' numbers."); return false; }

        evaluateCompExpr(_XStart_YStart, NULL);
        cmr->XStart = (int) Expression_variable [_XStart_YStart->ID+0];
        cmr->YStart = (int) Expression_variable [_XStart_YStart->ID+1];

        evaluateCompExpr(_XSize_YSize, NULL);
        cmr->XSize = (int) Expression_variable [_XSize_YSize->ID+0];
        cmr->YSize = (int) Expression_variable [_XSize_YSize->ID+1];

        cmr->closestColour   = (int*)FreeMemory;       FreeMemory += (cmr->YSize * cmr->XSize)*sizeof(int);
        cmr->closestDistance = (double*)FreeMemory;    FreeMemory += (cmr->YSize * cmr->XSize)*sizeof(double);
        cmr->closestObject   = (Object3D**)FreeMemory; FreeMemory += (cmr->YSize * cmr->XSize)*sizeof(Object3D*);
    }

    cmr->myParsedSignature = _myPS;
    cmr->origin_comp       = _origin_comp;
    cmr->Zaxis_comp        = _Zaxis_comp;
    cmr->rotation_comp     = _rotation_comp;
    cmr->ZSize_comp        = _ZSize_comp;
    strcpy (cmr->signature , _signature);
    return true;
}



bool addCamera (char* _signature)
{
    Camera *cmr = newCamera();

    if(!editCamera (cmr, _signature))
    { freeCamera(cmr); printf("%s\n", Camera_errorMessage); return false; }

    // add at head of camera linked-list
    cmr->prev = NULL;
    if(headCamera!=NULL)
        headCamera->prev = cmr;
    cmr->next = headCamera;
    headCamera = cmr;
    return true;
}




// do something based on user inputs
void camera_process (Camera *cmr)
{
    //int entry, *move;
    double magn;

    commitAllTobeEvaluated (cmr->myParsedSignature);

    cmr->origin[0]   = Expression_variable [cmr->origin_comp->headChild->ID];
    cmr->origin[1]   = Expression_variable [cmr->origin_comp->headChild->nextChild->ID];
    cmr->origin[2]   = Expression_variable [cmr->origin_comp->headChild->nextChild->nextChild->ID];
    cmr->Zaxis[0]    = Expression_variable [cmr->Zaxis_comp->headChild->ID];
    cmr->Zaxis[1]    = Expression_variable [cmr->Zaxis_comp->headChild->nextChild->ID];
    cmr->Zaxis[2]    = Expression_variable [cmr->Zaxis_comp->headChild->nextChild->nextChild->ID];
    cmr->rotation[2] = Expression_variable [cmr->rotation_comp->ID];
    cmr->ZSize       = Expression_variable [cmr->ZSize_comp->ID];

    magn = magnitude(cmr->Zaxis);
    cmr->Zaxis[0]/=magn; cmr->Zaxis[1]/=magn; cmr->Zaxis[2]/=magn; // make it a unit vector
    setRotation(cmr, cmr->rotation[2]); // will call updateLocalAxes(cmr)

    //{
    //    if(!(/*mouse_clickedCamera==cmr &&*/ mouse_get_right()))  // condition to move camera
    //    { /*to_next_mouse*/ return; }
    //    move = mouse_get_motion();                          // get mouse relative motion
    //    if(move[0]==0 && move[0]==0 && move[0]==0)
    //    { /*to_next_mouse*/ return; }

    /*    if(cmr->turnOnRightClick)
        {   if(kybrd_get_ctrl())
                shift(cmr, -move[0], -move[1], move[2]*4);  // change camera origin only
            else turn(cmr, -move[0], -move[1], move[2]*2);  // change camera direction vector only
        }
        else
        {   if(kybrd_get_ctrl())
                  turn(cmr, -move[0], -move[1], move[2]*2); // change camera direction vector only
            else shift(cmr, -move[0], -move[1], move[2]*4); // change camera origin only
        }
        mouse_clear_motion();
        clearkey();
        // go_to_next_mouse
    }*/
}

void camera_processAll()
{
    Camera *cmr;
    for(cmr = headCamera; cmr != NULL; cmr = cmr->next)
        camera_process(cmr);
}

    


    void camera_paintInitialise (Camera *cmr)
    {
        int i;
        for(i=0; i < (cmr->YSize * cmr->XSize); i++)
        {
            cmr->closestDistance[i] = WORLD_LIMIT;
            cmr->closestObject[i] = NULL;
            //closestColour[i]=0; // else do paint_background() later
        }
    }

    void camera_paintFinalise (Camera *cmr)
    {
        int a, x, y; // int b;
        paint_background(cmr);

        for(a=0, y=0; y < cmr->YSize; y++, a += cmr->XSize)
        {
            //b = (cmr->YStart + y)*SCR.XRes;
            for(x=0; x < cmr->XSize; x++)
                my_putpixel (cmr->XStart + x, cmr->YStart + y, cmr->closestColour[a+x]);
                //screenImage [b + cmr->XStart + x] = cmr->closestColour[a+x];
        }
    }

    void camera_paintInitialiseAll()
    {
        Camera *cmr;
        for(cmr = headCamera; cmr != NULL; cmr = cmr->next)
            camera_paintInitialise(cmr);
    }

    void camera_paintFinaliseAll()
    {
        Camera *cmr;
        for(cmr = headCamera; cmr != NULL; cmr = cmr->next)
            camera_paintFinalise(cmr);
    }


