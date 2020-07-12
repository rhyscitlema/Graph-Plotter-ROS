#ifndef _Object3D_H
#define _Object3D_H
/*********************************************************************************************
    object.h

    IMPORTANT:
    - Make sure to differentiate well between 'local' and 'world' coordinates.
    - The local coordinates are relative to the particular object (local 3D coordinates).
    - The local axes: Xaxis, Yaxis and Zaxis are vectors expressed in 'world' coordinates.
*********************************************************************************************/

#include <my_math.h>                    // only for mysqrt(), sin() and cos()
#include <my_stdint.h>
#include "parsedsignature.h"
#include "camera.h"

#define WORLD_LIMIT 0x1000000000000000  // maximum distance between any two points




typedef struct _Object3D
{
    ParsedSignature *myParsedSignature;
    char signature[2000];               // Uniquelly identifies a particular Object
    double origin[3];                   // Object Origin position vector (in world coordinates)
    double Xaxis[3];                    // Object X axis unit vector (in world coordinates)
    double Yaxis[3];                    // Object Y axis unit vector (in world coordinates)
    double Zaxis[3];                    // Object Z axis unit vector == Object direction vector
    double rotation[3];                 // Object Rotation unit vector (in 'local' coordinates)
    double boundary[6];                 // cube-like container enclosing the Object
    double accuracy;
    double transparency;
    Component *origin_comp, *Zaxis_comp, *rotation_comp;
    Component *boundary_comp, *accuracy_comp, *transparency_comp;
    /* The above are the main pre-defined variables */

    double CmrOrigin[3];
    double CmrAxes[3][3];
    double valueOfT;
    char resize_axis;

    bool turnOnLeftClick;
} Object3D;

extern struct _Camera *objCmr;

extern char* Object3D_errorMessage;

extern void putClosestPixel (Object3D *obj, int x, int y, int colour, double t);

extern void moveProcess(Object3D *obj);

extern void paint (Object3D *obj);

extern double cameraAxes[3][3];
extern double cameraOrigin[3];
extern double pixel_direction[3];




#define setvector(out,x,y,z) {out[0]=x; out[1]=y; out[2]=z;}

#define copyvector(out,in) {out[0]=in[0]; out[1]=in[1]; out[2]=in[2];}

#define addvectors(out,u,v) {out[0]=u[0]+v[0]; out[1]=u[1]+v[1]; out[2]=u[2]+v[2]; }

#define substractvectors(out,u,v) {out[0]=u[0]-v[0]; out[1]=u[1]-v[1]; out[2]=u[2]-v[2]; }

#define magnitude(u) mysqrt(dotproduct(u,u))

#define dotproduct(u,v) (u[0]*v[0] + u[1]*v[1] + u[2]*v[2])

#define crossproduct(out, u, v)         \
{   out[0] = u[1]*v[2] - u[2]*v[1];     \
    out[1] = u[2]*v[0] - u[0]*v[2];     \
    out[2] = u[0]*v[1] - u[1]*v[0];     \
}

#define pointOnLine(out, o, t, d)       \
{   out[0] = o[0] + t*d[0];             \
    out[1] = o[1] + t*d[1];             \
    out[2] = o[2] + t*d[2];             \
}



#endif
