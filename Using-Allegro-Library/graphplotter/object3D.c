/*********************************************************************************************
    object3D.c

    An object in Rhyscitlema 3D virtual environment.

    IMPORTANT:
    - Make sure to differentiate well between 'local' and 'world' coordinates.
    - The local coordinates are relative to the particular object (local 3D coordinates).
    - The local axes: Xaxis, Yaxis and Zaxis are vectors expressed in 'world' coordinates.
*********************************************************************************************/

#include "object3D.h"
#include "../hardware/screen.h"



    char errorMessage[100];

    double cameraAxes[3][3];
    double cameraOrigin[3];
    double pixel_direction[3];

    Camera *objCmr;





/*
Object3D *removed_object3D = NULL;

Object3D *newObject3D ()
{
    Object3D *obj;
    if(removed_object3D==NULL)
    {
        obj = (Object3D*)FreeMemory;
        FreeMemory += sizeof(Object3D);
    }
    else
    {   obj = removed_object3D;
        removed_object3D = removed_object3D->next;
    }
    return obj;
}

void freeObject3D (Object3D *obj)
{
    obj->prev = NULL;
    if(removed_object3D!=NULL)
        removed_object3D->prev = obj;
    obj->next = removed_object3D;
    removed_object3D = obj;
}
*/



    bool object3D_edit (char* _signature)
    { return false; }



    void putClosestPixel (Object3D *obj, int x, int y, int colour, double t)
    {
        int i = y * objCmr->XSize + x;
        if(t < objCmr->closestDistance[i])
        {
            objCmr->closestDistance[i] = t;
            objCmr->closestObject[i] = obj;
            objCmr->closestColour[i] = colour;
        }
    }



    /*
    //static void setvector (double[] out, double x, double y, double z)
    //{ out[0]=x; out[1]=y; out[2]=z; }

    //static void setvector (int[] out, int x, int y, int z)
    //{ out[0]=x; out[1]=y; out[2]=z; }

    //static void copyvectors (double[] out, double[] in)
    //{ out[0]=in[0]; out[1]=in[1]; out[2]=in[2]; }

    //static void addvectors (double[] out, double[] u, double[] v)
    //{ out[0] = u[0]+v[0]; out[1] = u[1]+v[1]; out[2] = u[2]+v[2]; }

    static void substractvectors (double[] out, double[] u, double[] v)
    { out[0] = u[0]-v[0]; out[1] = u[1]-v[1]; out[2] = u[2]-v[2]; }

    static double dotproduct (double[] u, double[] v)
    { return u[0]*v[0] + u[1]*v[1] + u[2]*v[2]; }

    static double magnitude (double[] u)
    { return Math.sqrt(dotproduct(u,u)); }

    static void crossproduct (double[] out, double[] u, double[] v)
    {   out[0] = u[1]*v[2] - u[2]*v[1];
        out[1] = u[2]*v[0] - u[0]*v[2];
        out[2] = u[0]*v[1] - u[1]*v[0];
    }

    static void pointOnLine (double[] out, double[] o, double t, double[] d)
    {   out[0] = o[0] + t*d[0];
        out[1] = o[1] + t*d[1];
        out[2] = o[2] + t*d[2];
    }
    */




    void updateLocalAxes (Object3D *obj)
    {
        double Xaxis[3], Yaxis[3];
        double Xmag, Ymag;

        Xaxis[0] = -obj->Zaxis[1];
        Xaxis[1] = obj->Zaxis[0];           // XAxis = crossproduct((0,0,1), Zaxis)
        Xaxis[2] = 0; 
        crossproduct(Yaxis, obj->Zaxis, Xaxis);
        Xmag = magnitude(Xaxis);
        Ymag = magnitude(Yaxis);

        // now set Xaxis to Rot vector in its world coordinates form
        Xaxis[0] = Xaxis[0]*obj->rotation[0]/Xmag + Yaxis[0]*obj->rotation[1]/Ymag;
        Xaxis[1] = Xaxis[1]*obj->rotation[0]/Xmag + Yaxis[1]*obj->rotation[1]/Ymag;
        Xaxis[2] = Xaxis[2]*obj->rotation[0]/Xmag + Yaxis[2]*obj->rotation[1]/Ymag;
        crossproduct(Yaxis, obj->Zaxis, Xaxis);

        obj->Xaxis[0] = Xaxis[0]; obj->Xaxis[1] = Xaxis[1]; obj->Xaxis[2] = Xaxis[2];
        obj->Yaxis[0] = Yaxis[0]; obj->Yaxis[1] = Yaxis[1]; obj->Yaxis[2] = Yaxis[2];
    }


    // relative shift (delta x, delta y, delta z) to change object origin vector Org only
    void shift (Object3D *obj, double dx, double dy, double dz)
    {
        Camera *cmr = objCmr; //should be cmr = mouse->leftClickedCamera;
        // LOCAL_TO_WORLD coordinate transformation
        obj->origin[0] += cmr->Xaxis[0]*dx + cmr->Yaxis[0]*dy + cmr->Zaxis[0]*dz;
        obj->origin[1] += cmr->Xaxis[1]*dx + cmr->Yaxis[1]*dy + cmr->Zaxis[1]*dz;
        obj->origin[2] += cmr->Xaxis[2]*dx + cmr->Yaxis[2]*dy + cmr->Zaxis[2]*dz;
    }


    // relative rotation (change in angle) so to update object rotation vector Rot only
    void rotate (Object3D *obj, double relative_angle)
    {
        obj->rotation[2] += relative_angle;
        obj->rotation[0] = cos(obj->rotation[2]*PI/180.0); // angle given in degrees
        obj->rotation[1] = sin(obj->rotation[2]*PI/180.0); // then converted to radians
        updateLocalAxes(obj);
    }


    // relative turn (delta x, delta y) to change object direction vector Zax only
    // dz = change in size of object
    void turn (Object3D *obj, double dx, double dy, int dz)
    {
        double newZax[3];
        double magn;

        rotate(obj, dz);
        if(dx==0 && dy==0) return;

        // LOCAL_TO_WORLD coordinate transformation
        newZax[0] = obj->Zaxis[0]*500 + obj->Xaxis[0]*dx + obj->Yaxis[0]*dy;
        newZax[1] = obj->Zaxis[1]*500 + obj->Xaxis[1]*dx + obj->Yaxis[1]*dy;
        newZax[2] = obj->Zaxis[2]*500 + obj->Xaxis[2]*dx + obj->Yaxis[2]*dy;

        if(newZax[0]==0 && newZax[1]==0)    // if newZax == world z-axis, then keep previous configurations
            return;

        magn = magnitude(newZax);
        newZax[0] = newZax[0]/magn;
        newZax[1] = newZax[1]/magn;
        newZax[2] = newZax[2]/magn;

        if((int)(5*newZax[0])==0 && (int)(5*newZax[1])==0)    // we have to check again!
            return;
        obj->Zaxis[0] = newZax[0];
        obj->Zaxis[1] = newZax[1];
        obj->Zaxis[2] = newZax[2];
        updateLocalAxes(obj);
    }


/*
    // new object origin vector Org
    void setOrigin (double x, double y, double z)
    { setvector(origin, x,y,z); }

    // new object direction vector Zax
    void setDirection (double x, double y, double z)
    {
        if(x==0 && y==0)  return;               // if parallel to world z-axis, quit
        setvector(Zaxis, x,y,z);
        updateLocalAxes();
    }
*/
    // new object rotation vector Rot
    void setRotation (Object3D *obj, double absolute_angle)
    {
        obj->rotation[2] = absolute_angle;
        obj->rotation[0] = cos(obj->rotation[2]*PI/180.0); // angle given in degrees
        obj->rotation[1] = sin(obj->rotation[2]*PI/180.0); // then converted to radians
        updateLocalAxes(obj);
    }





    // putpixel given a point (x, y, z) of an object in its local coordinates
    bool putLocalPixel (Object3D *obj, double x, double y, double z, int* pixel)
    {
        double t;
        double LiD[3];               // line direction vector, of the line joining CaO and (x,y,z).
        double Fnl[3];               // final direction vector lying on camera plane.
        double Tem[3];

        Tem[0] = objCmr->origin[0] - objCmr->Zaxis[0]*objCmr->ZSize;
        Tem[1] = objCmr->origin[1] - objCmr->Zaxis[1]*objCmr->ZSize;
        Tem[2] = objCmr->origin[2] - objCmr->Zaxis[2]*objCmr->ZSize;
        LiD[0] = obj->origin[0]  +  obj->Xaxis[0]*x + obj->Yaxis[0]*y + obj->Zaxis[0]*z  -  Tem[0];
        LiD[1] = obj->origin[1]  +  obj->Xaxis[1]*x + obj->Yaxis[1]*y + obj->Zaxis[1]*z  -  Tem[1];
        LiD[2] = obj->origin[2]  +  obj->Xaxis[2]*x + obj->Yaxis[2]*y + obj->Zaxis[2]*z  -  Tem[2];

        t = dotproduct (objCmr->Zaxis, LiD) / objCmr->ZSize;
        if(t<1)                                     // if (x,y,z) is NOT in front of plane of camera,
        { pixel[0]=pixel[1]=0; return false; }   // then it cannot be put to screen.

        Fnl[0] = LiD[0]/t - objCmr->Zaxis[0]*objCmr->ZSize;
        Fnl[1] = LiD[1]/t - objCmr->Zaxis[1]*objCmr->ZSize;
        Fnl[2] = LiD[2]/t - objCmr->Zaxis[2]*objCmr->ZSize;

        pixel[0] = (int)dotproduct (Fnl, objCmr->Xaxis);
        pixel[1] = (int)dotproduct (Fnl, objCmr->Yaxis);

        //X = (objCmr->XSize>>1)-1-pixel[0];
        //Y = (objCmr->YSize>>1)-1-pixel[1];
        //if(X<0 || X>=objCmr->XSize || Y<0 || Y>=objCmr->YSize)
        //    return false;   // if out of bounds
        //else return true;
        //putClosestPixel (X, Y, 0xFFFFFFFF, t);
        return true;
    }




    void cameraLocal_to_World_to_ObjectLocal (Object3D *obj)
    {
        double temp[3];
        cameraAxes[0][0] = dotproduct (obj->Xaxis, objCmr->Xaxis);
        cameraAxes[0][1] = dotproduct (obj->Xaxis, objCmr->Yaxis);
        cameraAxes[0][2] = dotproduct (obj->Xaxis, objCmr->Zaxis);
        cameraAxes[1][0] = dotproduct (obj->Yaxis, objCmr->Xaxis);
        cameraAxes[1][1] = dotproduct (obj->Yaxis, objCmr->Yaxis);
        cameraAxes[1][2] = dotproduct (obj->Yaxis, objCmr->Zaxis);
        cameraAxes[2][0] = dotproduct (obj->Zaxis, objCmr->Xaxis);
        cameraAxes[2][1] = dotproduct (obj->Zaxis, objCmr->Yaxis);
        cameraAxes[2][2] = dotproduct (obj->Zaxis, objCmr->Zaxis);

        substractvectors(temp, objCmr->origin, obj->origin);
        cameraOrigin[0] = dotproduct (obj->Xaxis, temp) - cameraAxes[0][2]*objCmr->ZSize;
        cameraOrigin[1] = dotproduct (obj->Yaxis, temp) - cameraAxes[1][2]*objCmr->ZSize;
        cameraOrigin[2] = dotproduct (obj->Zaxis, temp) - cameraAxes[2][2]*objCmr->ZSize;
    }

    static void computePixelLine (double* pixel_line, int x, int y)
    {
        double temp[3];
        temp[0] = (objCmr->XSize>>1)-1-x;
        temp[1] = (objCmr->YSize>>1)-1-y;
        temp[2] = objCmr->ZSize;
        pixel_line[0] = dotproduct (cameraAxes[0], temp);
        pixel_line[1] = dotproduct (cameraAxes[1], temp);
        pixel_line[2] = dotproduct (cameraAxes[2], temp);
    }

    /*
    void computePixelDirectionXY ()
    {
        int y, x;
        double t;
        for(y=0; y<objCmr->YSize; y++)
        {
            t = (objCmr->YSize>>1)-1-y;
            objCmr->pixelDirectionY[y*3+0] = cameraAxes[0][1]*t + cameraAxes[0][2]*objCmr->ZSize;
            objCmr->pixelDirectionY[y*3+1] = cameraAxes[1][1]*t + cameraAxes[1][2]*objCmr->ZSize;
            objCmr->pixelDirectionY[y*3+2] = cameraAxes[2][1]*t + cameraAxes[2][2]*objCmr->ZSize;
        }
        for(x=0; x<objCmr->XSize; x++)
        {
            t = (objCmr->XSize>>1)-1-x;
            objCmr->pixelDirectionX[x*3+0] = cameraAxes[0][0]*t;
            objCmr->pixelDirectionX[x*3+1] = cameraAxes[1][0]*t;
            objCmr->pixelDirectionX[x*3+2] = cameraAxes[2][0]*t;
        }
    }

    void computePixelLineMethod2 (double[] pixel_line, int x, int y)
    {   pixel_line[0] = objCmr->pixelDirectionX[x*3+0] + objCmr->pixelDirectionY[y*3+0];
        pixel_line[1] = objCmr->pixelDirectionX[x*3+1] + objCmr->pixelDirectionY[y*3+1];
        pixel_line[2] = objCmr->pixelDirectionX[x*3+2] + objCmr->pixelDirectionY[y*3+2];
    }
    */



    extern void pixelPropagate(int x, int y);

    // putpixel given a point (x, y, z) of an object in its local coordinates
    bool putLocalPixel2 (Object3D *obj, double x, double y, double z, int colour)
    {
        double t;
        double LiD[3];               // line direction vector, of the line joining CaO and (x,y,z).
        double Fnl[3];               // final direction vector lying on camera plane.
        double Tem[3];
        int X, Y;

        Tem[0] = objCmr->origin[0] - objCmr->Zaxis[0]*objCmr->ZSize;
        Tem[1] = objCmr->origin[1] - objCmr->Zaxis[1]*objCmr->ZSize;
        Tem[2] = objCmr->origin[2] - objCmr->Zaxis[2]*objCmr->ZSize;
        LiD[0] = obj->origin[0]  +  obj->Xaxis[0]*x + obj->Yaxis[0]*y + obj->Zaxis[0]*z  -  Tem[0];
        LiD[1] = obj->origin[1]  +  obj->Xaxis[1]*x + obj->Yaxis[1]*y + obj->Zaxis[1]*z  -  Tem[1];
        LiD[2] = obj->origin[2]  +  obj->Xaxis[2]*x + obj->Yaxis[2]*y + obj->Zaxis[2]*z  -  Tem[2];

        t = dotproduct (objCmr->Zaxis, LiD) / objCmr->ZSize;
        if(t<1)                                     // if (x,y,z) is NOT in front of plane of camera,
            return false;                           // then it cannot be put to screen.

        Fnl[0] = LiD[0]/t - objCmr->Zaxis[0]*objCmr->ZSize;
        Fnl[1] = LiD[1]/t - objCmr->Zaxis[1]*objCmr->ZSize;
        Fnl[2] = LiD[2]/t - objCmr->Zaxis[2]*objCmr->ZSize;
        X = (int)dotproduct (Fnl, objCmr->Xaxis);
        Y = (int)dotproduct (Fnl, objCmr->Yaxis);

        X = (objCmr->XSize>>1)-1-X;
        Y = (objCmr->YSize>>1)-1-Y;
        if(X<0 || X>=objCmr->XSize || Y<0 || Y>=objCmr->YSize)
            return false;   // if out of bounds

        putClosestPixel (obj, X, Y, colour, t);
        //pixelUsed[y*XS+x] = pixelCheck;
        pixelPropagate (X, Y);
        return true;
    }

    void showAxes (Object3D *obj, int XS, int YS, int ZS)
    {
        const int WI = 15;
        int i, j, P, N;

        //if(mouse_clickedObject!=obj)// && mouse.pointedObject!=this)  // if any mouse
        //    return;

        N = (int)obj->boundary[0];
        for(j=-WI; j<WI; j++)
            for(i=N ; i > N-XS ; i--)
                putLocalPixel2 (obj, i, j, 0, 0x00000080);

        P = (int)obj->boundary[1];
        for(j=-WI; j<WI; j++)
            for(i=P ; i < P+XS ; i++)
                putLocalPixel2 (obj, i, j, 0, 0x000000F0);

        N = (int)obj->boundary[2];
        for(j=-WI; j<WI; j++)
            for(i=N ; i > N-YS ; i--)
                putLocalPixel2 (obj, j, i, 0, 0x00008000);

        P = (int)obj->boundary[3];
        for(j=-WI; j<WI; j++)
            for(i=P ; i < P+YS ; i++)
                putLocalPixel2 (obj, j, i, 0, 0x0000F000);

        N = (int)obj->boundary[4];
        for(j=-WI; j<WI; j++)
            for(i=N ; i > N-ZS ; i--)
                putLocalPixel2 (obj, 0, j, i, 0x00800000);

        P = (int)obj->boundary[5];
        for(j=-WI; j<WI; j++)
            for(i=P ; i < P+ZS ; i++)
                putLocalPixel2 (obj, 0, j, i, 0x00F00000);
    }




    void moveProcess (Object3D *obj)
    {
        //double pixel_direction[3]; // ??? may be a duplicate of the static version
        //double temp[3];
        double magn;
        //int* move;
        int i;//, pixel;
        Component *comp;
        //Camera *cmr;


        //if(!(mouse_clickedObject==obj && mouse_leftPressed))
        //{ obj->resize_axis=20; /*return;*/ }

        commitAllTobeEvaluated (obj->myParsedSignature);

        for(i=0, comp = obj->boundary_comp->headChild; comp != 0; comp = comp->nextChild, i++)
            obj->boundary[i] = Expression_variable[comp->ID];
        obj->origin[0]   = Expression_variable [obj->origin_comp->headChild->ID];
        obj->origin[1]   = Expression_variable [obj->origin_comp->headChild->nextChild->ID];
        obj->origin[2]   = Expression_variable [obj->origin_comp->headChild->nextChild->nextChild->ID];
        obj->Zaxis[0]    = Expression_variable [obj->Zaxis_comp->headChild->ID];
        obj->Zaxis[1]    = Expression_variable [obj->Zaxis_comp->headChild->nextChild->ID];
        obj->Zaxis[2]    = Expression_variable [obj->Zaxis_comp->headChild->nextChild->nextChild->ID];
        obj->rotation[2] = Expression_variable [obj->rotation_comp->ID];
        obj->accuracy    = Expression_variable [obj->accuracy_comp->ID];
        //obj->transparency= Expression_variable [transparency_comp->ID];

        magn = magnitude(obj->Zaxis);
        obj->Zaxis[0]/=magn; obj->Zaxis[1]/=magn; obj->Zaxis[2]/=magn; // make it a unit vector
        setRotation(obj, obj->rotation[2]); // will call updateLocalAxes()


        /*move = mouse_get_motion();

        if(resize_axis==20)
        {
            cmr = mouse_leftClickedCamera;
            if(cmr==NULL) return;
            pixel = mouse.previousPosition[1]*cmr->XSize
                  + mouse.previousPosition[0];
            if(pixel<0 || pixel >= cmr->XSize * cmr->YSize) return; // if index out of range
            obj->valueOfT = cmr->closestDistance [pixel];
            obj->CmrAxes[0][0] = dotproduct (obj->Xaxis, cmr->Xaxis);
            obj->CmrAxes[0][1] = dotproduct (obj->Xaxis, cmr->Yaxis);
            obj->CmrAxes[0][2] = dotproduct (obj->Xaxis, cmr->Zaxis);
            obj->CmrAxes[1][0] = dotproduct (obj->Yaxis, cmr->Xaxis);
            obj->CmrAxes[1][1] = dotproduct (obj->Yaxis, cmr->Yaxis);
            obj->CmrAxes[1][2] = dotproduct (obj->Yaxis, cmr->Zaxis);
            obj->CmrAxes[2][0] = dotproduct (obj->Zaxis, cmr->Xaxis);
            obj->CmrAxes[2][1] = dotproduct (obj->Zaxis, cmr->Yaxis);
            obj->CmrAxes[2][2] = dotproduct (obj->Zaxis, cmr->Zaxis);
            substractvectors(temp, cmr->origin, obj->origin);
            obj->CmrOrigin[0] = dotproduct (obj->Xaxis, temp) - obj->CmrAxes[0][2] * cmr->ZSize;
            obj->CmrOrigin[1] = dotproduct (obj->Yaxis, temp) - obj->CmrAxes[1][2] * cmr->ZSize;
            obj->CmrOrigin[2] = dotproduct (obj->Zaxis, temp) - obj->CmrAxes[2][2] * cmr->ZSize;


            temp[0] = (cmr->XSize>>1)-1-mouse_previousPosition[0];
            temp[1] = (cmr->YSize>>1)-1-mouse_previousPosition[1];
            temp[2] = cmr->ZSize;
            pixel_direction[0] = dotproduct (obj->CmrAxes[0], temp);
            pixel_direction[1] = dotproduct (obj->CmrAxes[1], temp);
            pixel_direction[2] = dotproduct (obj->CmrAxes[2], temp);

            pointOnLine(temp, obj->CmrOrigin, obj->valueOfT, pixel_direction);
                 if(temp[0] < obj->boundary[0] && (int)(temp[1]/30)==0 && (int)(temp[2]/30)==0) obj->resize_axis=0;
            else if(temp[0] > obj->boundary[1] && (int)(temp[1]/30)==0 && (int)(temp[2]/30)==0) obj->resize_axis=1;
            else if(temp[1] < obj->boundary[2] && (int)(temp[0]/30)==0 && (int)(temp[2]/30)==0) obj->resize_axis=2;
            else if(temp[1] > obj->boundary[3] && (int)(temp[0]/30)==0 && (int)(temp[2]/30)==0) obj->resize_axis=3;
            else if(temp[2] < obj->boundary[4] && (int)(temp[0]/30)==0 && (int)(temp[1]/30)==0) obj->resize_axis=4;
            else if(temp[2] > obj->boundary[5] && (int)(temp[0]/30)==0 && (int)(temp[1]/30)==0) obj->resize_axis=5;
        }

        if(move[0]==0 && move[0]==0 && move[0]==0) return;

        if(obj->resize_axis >= 20)
        {
            magn = obj->valueOfT;
            if(obj->turnOnLeftClick)
            {   if(kybrd_get_ctrl())//(mouse.ctrlPressed)
                    shift(obj, -move[0]*magn, -move[1]*magn, move[2]*4); // change object origin only
                else turn (obj, move[0], -move[1], move[2]);     // change object direction vector only
            }
            else
            {   if(kybrd_get_ctrl())//(mouse.ctrlPressed)
                    turn (obj, move[0], -move[1], move[2]);     // change object direction vector only
                else shift(obj, -move[0]*magn, -move[1]*magn, move[2]*4); // change object origin only
            }
            obj->resize_axis = 30;
        }

        else
        {
            temp[0] = -move[0]*obj->valueOfT;
            temp[1] = -move[1]*obj->valueOfT;
            temp[2] = move[2];
            obj->boundary [obj->resize_axis] += dotproduct (obj->CmrAxes[(obj->resize_axis)/2], temp);
        }
        mouse_clear_motion();*/
    }





    // pixel propagation
    char pixelCheck;
    char pixelUsed[1280*800];
    int pixelPropagated[1280*800*2];
    int pixelProp;


    void pixelPropagateInitialise ()
    {
        int i;
        if(pixelCheck<=0)
        {
            for(i=0; i < (objCmr->YSize * objCmr->XSize); i++)
                pixelUsed[i] = 0x7F;
            pixelCheck = 0x7F;
        }
        pixelCheck--;
        pixelProp=0;
    }

    void PROPAGATE (int x, int y)
    {
        int XS, YS;
        XS = objCmr->XSize;
        YS = objCmr->YSize;
        if(x<0 || x>=XS || y<0 || y>=YS) // if out of range
            return;
        if(pixelUsed[y*XS+x] == pixelCheck)   // if already propagated
            return;
        pixelUsed[y*XS+x] = pixelCheck;
        pixelPropagated[pixelProp*2+0] = x;
        pixelPropagated[pixelProp*2+1] = y;
        pixelProp++;
    }

    void pixelPropagate (int x, int y)
    {
        PROPAGATE (x-2, y-2);
        PROPAGATE (x-2, y-1);
        PROPAGATE (x-2, y  );
        PROPAGATE (x-2, y+1);
        PROPAGATE (x-2, y+2);

        PROPAGATE (x-1, y-2);
        PROPAGATE (x-1, y-1);
        PROPAGATE (x-1, y  );
        PROPAGATE (x-1, y+1);
        PROPAGATE (x-1, y+2);

        PROPAGATE (x  , y-2);
        PROPAGATE (x  , y-1);
        PROPAGATE (x  , y+1);
        PROPAGATE (x  , y+2);

        PROPAGATE (x+1, y-2);
        PROPAGATE (x+1, y-1);
        PROPAGATE (x+1, y  );
        PROPAGATE (x+1, y+1);
        PROPAGATE (x+1, y+2);

        PROPAGATE (x+2, y-2);
        PROPAGATE (x+2, y-1);
        PROPAGATE (x+2, y  );
        PROPAGATE (x+2, y+1);
        PROPAGATE (x+2, y+2);
    }





    extern bool shootPixelGetT (Object3D *obj, double t1, double t2, int xp, int yp);
    //bool shootPixelGetT (Object3D *obj, double t1, double t2, int xp, int yp)
    //{ putClosestPixel(obj, xp, yp, 0xFF, t1); return false; }



    bool shootPixel(Object3D *obj, int xp, int yp)
    {
        double t, t1, t2, x, y, z;
        double* boundary = obj->boundary;

        computePixelLine (pixel_direction, xp, yp);


        /* find the two values t1 and t2 */
        t1 = t2 = WORLD_LIMIT;

        t = (boundary[0] - cameraOrigin[0]) / pixel_direction[0];
        y = cameraOrigin[1] + t*pixel_direction[1];
        z = cameraOrigin[2] + t*pixel_direction[2];
        if( y>=boundary[2] && y<=boundary[3] && z>=boundary[4] && z<=boundary[5] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        t = (boundary[1] - cameraOrigin[0]) / pixel_direction[0];
        y = cameraOrigin[1] + t*pixel_direction[1];
        z = cameraOrigin[2] + t*pixel_direction[2];
        if( y>=boundary[2] && y<=boundary[3] && z>=boundary[4] && z<=boundary[5] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        t = (boundary[2] - cameraOrigin[1]) / pixel_direction[1];
        x = cameraOrigin[0] + t*pixel_direction[0];
        z = cameraOrigin[2] + t*pixel_direction[2];
        if( x>=boundary[0] && x<=boundary[1] && z>=boundary[4] && z<=boundary[5] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        t = (boundary[3] - cameraOrigin[1]) / pixel_direction[1];
        x = cameraOrigin[0] + t*pixel_direction[0];
        z = cameraOrigin[2] + t*pixel_direction[2];
        if( x>=boundary[0] && x<=boundary[1] && z>=boundary[4] && z<=boundary[5] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        t = (boundary[4] - cameraOrigin[2]) / pixel_direction[2];
        x = cameraOrigin[0] + t*pixel_direction[0];
        y = cameraOrigin[1] + t*pixel_direction[1];
        if( x>=boundary[0] && x<=boundary[1] && y>=boundary[2] && y<=boundary[3] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        t = (boundary[5] - cameraOrigin[2]) / pixel_direction[2];
        x = cameraOrigin[0] + t*pixel_direction[0];
        y = cameraOrigin[1] + t*pixel_direction[1];
        if( x>=boundary[0] && x<=boundary[1] && y>=boundary[2] && y<=boundary[3] )
        {   if(t<1) t=1; if(t<t1) { t2=t1; t1=t; } else t2=t; }

        if(t1==WORLD_LIMIT || t2==WORLD_LIMIT || t1+0.001 >= t2) return false;


        shootPixelGetT(obj, t1, t2, xp, yp);
        return true;
    }





    void paint (Object3D *obj)
    {
        int x, y;
        int a, b, c, d, nX, nY, pX, pY;
        int array[8][2];
        double* boundary = obj->boundary;

        commitAllTobeEvaluated (obj->myParsedSignature);

        cameraLocal_to_World_to_ObjectLocal (obj);

        //...
        putLocalPixel (obj, boundary[0], boundary[2], boundary[4], array[0]);
        putLocalPixel (obj, boundary[0], boundary[3], boundary[5], array[1]);
        putLocalPixel (obj, boundary[1], boundary[3], boundary[4], array[2]);
        putLocalPixel (obj, boundary[1], boundary[2], boundary[5], array[3]);
        putLocalPixel (obj, boundary[0], boundary[3], boundary[4], array[4]);
        putLocalPixel (obj, boundary[0], boundary[2], boundary[5], array[5]);
        putLocalPixel (obj, boundary[1], boundary[2], boundary[4], array[6]);
        putLocalPixel (obj, boundary[1], boundary[3], boundary[5], array[7]);

        x = (objCmr->XSize) >> 1;
        y = (objCmr->YSize) >> 1;
        nX = -x;  pX = x-1;
        nY = -y;  pY = y-1;

        while(true)
        {
            a = array[0][0];
            b = array[0][1];
            if(a!=0 || b!=0)
            {
                c = array[4][0];
                d = array[4][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[5][0];
                d = array[5][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[6][0];
                d = array[6][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
            }
            a = array[1][0];
            b = array[1][1];
            if(a!=0 || b!=0)
            {
                c = array[4][0];
                d = array[4][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[5][0];
                d = array[5][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[7][0];
                d = array[7][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
            }
            a = array[2][0];
            b = array[2][1];
            if(a!=0 || b!=0)
            {
                c = array[4][0];
                d = array[4][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[6][0];
                d = array[6][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[7][0];
                d = array[7][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
            }
            a = array[3][0];
            b = array[3][1];
            if(a!=0 || b!=0)
            {
                c = array[5][0];
                d = array[5][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[6][0];
                d = array[6][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
                c = array[7][0];
                d = array[7][1];
                if(c!=0 || d!=0)
                {   if(a<nX) { if(!(c<nX)) { y = b + (nX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=nX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else if(a>pX) { if(!(c>pX)) { y = b + (pX-a)*(d-b)/(c-a); if(y>=nY && y<=pY) x=pX; else if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else {} } } else { if(b<nY) { if(!(d<nY)) { x = a + (nY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=nY; } } else if(b>pY) { if(!(d>pY)) { x = a + (pY-b)*(c-a)/(d-b); if(x>=nX && x<=pX) y=pY; } } else { x=a; y=b; } }
                    if(x>pX) y=pY+1; else if(y>pY) x=pX+1; else break;
                }
            }
            break;
        }


        x = ((objCmr->XSize) >> 1)-1-x;  // change to true pixel
        y = ((objCmr->YSize) >> 1)-1-y;  // (x,y) value

        if(x<0 || y<0)              // if not valid
        {
            // last step...!
            if( shootPixel (obj, 0, 0) )
            { x=0; y=0; }
            else if( shootPixel (obj, 0, objCmr->YSize-1) )
            { x=0; y = objCmr->YSize-1; }
            else if( shootPixel (obj, objCmr->XSize-1, 0) )
            { x = objCmr->XSize-1; y=0; }
            else if( shootPixel (obj, objCmr->XSize-1, objCmr->YSize-1) )
            { x = objCmr->XSize-1; y = objCmr->YSize-1; }
        }


        if(!(x<0 || y<0))           // if IS valid
        {
            pixelPropagateInitialise();
            pixelPropagate(x, y);

            while(pixelProp>0)
            {   pixelProp--;
                x = pixelPropagated[pixelProp*2+0];
                y = pixelPropagated[pixelProp*2+1];
                if(shootPixel(obj, x, y))
                    pixelPropagate (x, y);
            }
        }


        showAxes(obj, 200,200,200);
    }






/*
        if(putLocalPixel(boundary[0], boundary[2], boundary[4], X, Y))
        {   array[0*4+0] = X[0]; array[0*4+1] = Y[0];
            array[1*4+0] = X[0]; array[1*4+1] = Y[0];
            array[2*4+0] = X[0]; array[2*4+1] = Y[0];
        }
        else { array[0*4+0] = array[0*4+1] = array[1*4+0] = array[1*4+1] = array[2*4+0] = array[2*4+1] = 0; }

        x = objCmr->XSize>>1;
        y = objCmr->YSize>>1;
        nX = -x; pX = x-1;
        nY = -y; pY = y-1;

        for(i=0; i<12; i++)
        {
            a = array[i*4+0];
            b = array[i*4+1];
            c = array[i*4+2];
            d = array[i*4+3];
            if(a==0 && b==0) continue;
            if(c==0 && d==0) continue;

        if(a<nX)
        {
          if(!(c<nX))       // a<nX, c>=nX
          {
            y = b + (nX-a)*(d-b)/(c-a);
            if(y>=nY && y<=pY) x=nX;

            else if(b<nY)
            {
              if(!(d<nY))   // b<nY, d>=nY
              {
                x = a + (nY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=nY;
              }
            }
            else if(b>pY)
            {
              if(!(d>pY))   // b>pY, d<=pY
              {
                x = a + (pY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=pY;
              }
            }
            else            // nY<=b<=pY
            {}
          }
        }
        else if(a>pX)
        {
          if(!(c>pX))       // a>pX, c<=pX
          {
            y = b + (pX-a)*(d-b)/(c-a);
            if(y>=nY && y<=pY) x=pX;

            else if(b<nY)
            {
              if(!(d<nY))   // b<nY, d>=nY
              {
                x = a + (nY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=nY;
              }
            }
            else if(b>pY)
            {
              if(!(d>pY))   // b>pY, d<=pY
              {
                x = a + (pY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=pY;
              }
            }
            else            // nY<=b<=pY
            {}
          }
        }
        else                // nX<=a<=pX
        {
            if(b<nY)
            {
              if(!(d<nY))   // b<nY, d>=nY
              {
                x = a + (nY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=nY;
              }
            }
            else if(b>pY)
            {
              if(!(d>pY))   // b>pY, d<=pY
              {
                x = a + (pY-b)*(c-a)/(d-b);
                if(x>=nX && x<=pX) y=pY;
              }
            }
            else            // nY<=b<=pY
            { x=a; y=b; }
        }

            if(x>pX) y=pY+1;        // if not valid
            else if(y>pY) x=pX+1;
            else break;
        }


0 - 4 - (0,1,2)
0 - 5 - (0,1,3)
0 - 6 - (0,2,3)

1 - 4 - (0,1,2)
1 - 5 - (0,1,3)
1 - 7 - (1,2,3)

2 - 4 - (0,1,2)
2 - 6 - (0,2,3)
2 - 7 - (1,2,3)

3 - 5 - (0,1,3)
3 - 6 - (0,2,3)
3 - 7 - (1,2,3)
*/


/*
    s = (boundary[0] - Pt[0]) / Dr[0];
    y = (int)( Pt[1] + s*Dr[1] );
    z = (int)( Pt[2] + s*Dr[2] );
    if( y>=boundary[2] && y<=boundary[3] && z>=boundary[4] && z<=boundary[5] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }

    s = (boundary[1] - Pt[0]) / Dr[0];
    y = (int)( Pt[1] + s*Dr[1] );
    z = (int)( Pt[2] + s*Dr[2] );
    if( y>=boundary[2] && y<=boundary[3] && z>=boundary[4] && z<=boundary[5] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }


    s = (boundary[2] - Pt[1]) / Dr[1];
    x = (int)( Pt[0] + s*Dr[0] );
    z = (int)( Pt[2] + s*Dr[2] );
    if( x>=boundary[0] && x<=boundary[1] && z>=boundary[4] && z<=boundary[5] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }

    s = (boundary[3] - Pt[1]) / Dr[1];
    x = (int)( Pt[0] + s*Dr[0] );
    z = (int)( Pt[2] + s*Dr[2] );
    if( x>=boundary[0] && x<=boundary[1] && z>=boundary[4] && z<=boundary[5] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }


    s = (boundary[4] - Pt[2]) / Dr[2];
    x = (int)( Pt[0] + s*Dr[0] );
    y = (int)( Pt[1] + s*Dr[1] );
    if( x>=boundary[0] && x<=boundary[1] && y>=boundary[2] && y<=boundary[3] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }

    s = (boundary[5] - Pt[2]) / Dr[2];
    x = (int)( Pt[0] + s*Dr[0] );
    y = (int)( Pt[1] + s*Dr[1] );
    if( x>=boundary[0] && x<=boundary[1] && y>=boundary[2] && y<=boundary[3] )
    {   if(s<1) s=1; if(s<t1) { t2=t1; t1=s; } else t2=s; }
*/


/* update world coordinate values of local X and Y axes,
   by using the local Zaxis and the world z-axis (0,0,1).
   They must NOT be parallel to each other.

   Then rotate the local dimension by 'assigning'
   the Xaxis to become the rotation unit vector Rot.
   Therefore z-component Rot[2] is NOT used at all.
*/


/*
    Origin = O = (Ox, Oy, Oz)
    X-axis = U = (Ux, Uy, Uz)
    Y-axis = V = (Vx, Vy, Vz)
    Z-axis = W = (Wx, Wy, Wz)

    LOCAL_TO_WORLD coordinate transformation
    Xw     Ox     Ux Vx Wx   Xl
    Yw  =  Oy  +  Uy Vy Wy x Yl
    Zw     Oz     Uz Vz Wz   Zl

    WORLD_TO_LOCAL coordinate transformation
    Xl     Ux Uy Uz     Xw - Ox
    Yl  =  Vx Vy Vz  x  Yw - Oy
    Zl     Wx Wy Wz     Zw - Oz


    LOCAL_TO_WORLD_TO_LOCAL coordinate transformation

    X2l     U2x U2y U2z       O1x - O2x     U1x V1x W1x   X1l
    Y2l  =  V2x V2y V2z  x  ( O1y - O2y  +  U1y V1y W1y x Y1l  )
    Z2l     W2x W2y W2z       O1z - O2z     U1z V1z W1z   Z1l
*/

