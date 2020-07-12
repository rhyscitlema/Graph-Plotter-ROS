#ifndef _CAMERA_H
#define _CAMERA_H
/***************************************************************
    camera.h
***************************************************************/

#include "component.h"
#include "object3D.h"


typedef struct _Camera
{
    int XStart, YStart, XSize, YSize;   // Camera as a rectangle on the main screen

    ParsedSignature *myParsedSignature;  // Parsed version of the camera signature
    char signature[2000];
    double origin[3];                   // Camera Origin position vector (in world coordinates)
    double Xaxis[3];                    // Camera X axis unit vector (in world coordinates)
    double Yaxis[3];                    // Camera Y axis unit vector (in world coordinates)
    double Zaxis[3];                    // Camera Z axis unit vector == Camera direction vector
    double rotation[3];                 // Camera Rotation unit vector (in 'local' coordinates)
    double ZSize;
    Component *origin_comp, *Zaxis_comp, *rotation_comp, *ZSize_comp;

    int *closestColour;                 // Array of colours for camera pixels
    double *closestDistance;            // Array of values for distance 't'
    struct _Object3D** closestObject;           // Array of object pointers

    bool turnOnRightClick;

    struct _Camera *prev, *next;                // pointers for a double-linked-list of Cameras
} Camera;


extern Camera *headCamera;

extern char Camera_errorMessage[];

extern bool addCamera (char* _signature);
extern bool editCamera (Camera *cmr, char* _signature);
extern void camera_processAll ();

extern void camera_paintInitialiseAll ();
extern void camera_paintFinaliseAll ();

/*
//extern FLOAT (*Xaxis)[3];
//extern FLOAT (*Yaxis)[3];

extern char *PixelUsed;
extern char PixelCheck;

extern int (*Propagate)[2];
extern int (*PropPtr)[2];
*/

#endif
