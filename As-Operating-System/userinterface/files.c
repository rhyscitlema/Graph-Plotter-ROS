/*
    files.c

    Handle file related issues.
    visit the files.h file first.
*/

#include "files.h"
#include <graph.h>
#include <camera.h>
#include <graphplotter.h>
#include <userinterface.h>
#include "../hardware/screen.h" // for SCR only
#include <_stdio.h> // for sprintf0() only



int Savefile (const wchar* file_name, const wchar* file_content)
{
    return 0;
}



int Openfile (const wchar* file_name, wchar* file_content)
{
    return 0;
}



// load all graphs and cameras from a file
int load_all_objects (const wchar* fileName)
{
    /*wchar fileContent [BUFFER_SIZE];

    if(fileName != NULL)
    {
        if(!Openfile (fileName, fileContent))
            return FALSE;
    }
    else
    {
        return FALSE;
    }

    //noObjects = FALSE;
    userinterface_display_other (NULL);
    graphplotter_load_all_objects (fileContent);
    return TRUE;*/


    wchar str[10000];

    //strcpy0 (str, Initial_graphs_and_cameras);
    //graphplotter_load_all_objects(str);

    sprintf0((char*)(str+5000), " type     =  Camera ;  name     =  Camera 1 ;   position_vector =  { pos0, pos1, 0 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   zoom_amount     =  0.5 + zoomOffset;  center_point    =  {0, 0} ;  width_height    =  {%d, %d} ;   #Below are user-defined variables ;        pos0 = 100cos(u) ;        pos1 = 100sin(u) ;        dir0 =   -sin(u) ;        dir1 =    cos(u) ;        u    = t*pi/12 ;   X_axis =       ( -dir0, -dir1, 0 ) ;  Y_axis = turnX((   0  ,   0  , 1 ) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   zoomOffset  = NoRightClick(NoCtrlPress(MouseMotionZ(0))) / 20;   moveOffset = OnCtrlPress (-MouseMotionX(0) / 10,                            -MouseMotionY(0) / 10,                             MouseMotionZ(0) * 10 );   turnAngle  = OnRightClick( MouseMotionX(0) / 1000,                            -MouseMotionY(0) / 1000,                             MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix((0,0,1), turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(Z_axis , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(X_axis , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ", SCR.XRes, SCR.YRes);
    strcpy0(str, (char*)(str+5000));
    camera_add(str);


    strcpy0(str, " type     =  Graph ;  name     =  Graph 1 ;   position_vector =  { 30 , 0 , 0 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   bdr = 8 ;   k = bdr-1 ;   a = 2.5 ;   graph_equation (x,y,z)  = z=0  , x <= k+cos(a*y) and x >= -k-cos(a*y)                               and y <= k+cos(a*x) and y >= -k-cos(a*x) ;   graph_colouring (x,y,z) =  (x*50) mod 256                          + ((y*50) mod 256)*256 ;   graph_boundary  =  {-bdr, bdr, -bdr, bdr, -1, 1} ;   plot_accuracy   =  1 ;   X_axis =       ( 0 ,-1,  0 ) ;  Y_axis = turnX((0.7, 0, 0.7) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(0) / 100,                        4t + MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Graph 2 ;   position_vector =  { 0 , -30 , 0 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   c = 1.01 + sin(u) ;   u = t*pi/2 ;   bdr = 8 ;   graph_equation (x,y,z)  =   z/c = sin(x/c)  ,  x*x+y*y<=bdr*bdr ;   graph_colouring (x,y,z) =         (x*100) mod 256                          +        ((y*100) mod 256)*256                          + ((127.5(1-z/c)) mod 256)*256*256 ;   graph_boundary  =  {-bdr, bdr, -bdr, bdr, -c, c} ;   plot_accuracy   =   8 ;   X_axis =       (-1,  0,  0 ) ;  Y_axis = turnX(( 0, -1, 0.1) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(0) / 100,                             MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Graph 3 ;   position_vector =  { 0 , 30 , 0 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   graph_equation (x,y,z)  =  z = 0.5 x^3 - 3x + 0.5 y^3 - 3y ;   graph_colouring (x,y,z) = ( (127.5*(1 + x / bdx)) mod 256)                          + ( (127.5*(1 + y / bdy)) mod 256)*256                           + ( (127.5*(1 + z / bdz)) mod 256)*256*256 ;   bdx = 4 ;    bdy = bdx;    bdz = 6 ;   graph_boundary  =  {-bdx, bdx, -bdy, bdy, -bdz, bdz} ;   plot_accuracy   =   10 ;   X_axis =       (0.8, -0.53,  0 ) ;  Y_axis = turnX((0.4,  0.6 , 0.6) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(0) / 100,                        4t + MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Graph 4 ;   position_vector =  { -30, 0, 0 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   graph_equation (x,y,z)  =   y = - tan(z) * x ,                             x*x + y*y <= bdr*bdr ;   graph_colouring (x,y,z) =  (x*50) mod 256                          + ((y*50) mod 256)*256                          + ((z*50) mod 2566)*256*256 ;   graph_boundary  =  {-bdr, bdr, -bdr, bdr, -10, 10} ;   plot_accuracy   =  50 ;    bdr = 5 ;   X_axis =       (-1, 0, 0) ;  Y_axis = turnX(( 0, 0, 1) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(0) / 100,                        4t + MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );   #Note: greater accuracy and greater boundary reduce the  #speed of execution. Remove this graph and notice the change! ;  ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Graph 5 ;   u = t*pi/6 ;  a = 12 ;   position_vector =  { (4a + a * sin(20u)) cos(u) ,                       (4a + a * sin(20u)) sin(u) ,                             a * cos(20u)         } + displacement ;   #above in {} is the vector equation of the 'toroidal spiral' ;   X_axis_vector  =  turnY(turnZ(X_axis));  Y_axis_vector  =  turnY(turnZ(Y_axis));   graph_equation (x,y,z)  =   x*x + y*y + z*z = bdr*bdr ;                            #this is the equation of a 'sphere' ;   graph_colouring (x,y,z) = ((127.5*(1 + x / bdr)) mod 256)                          + ((127.5*(1 + y / bdr)) mod 256)*256                           + ((127.5*(1 + z / bdr)) mod 256)*256*256 ;   graph_boundary =  {-bdr, bdr, -bdr, bdr, -bdr, bdr} ;   plot_accuracy  =  4 ;   bdr = 4 ;   X_axis =       (1, 0, 0) ;  Y_axis = turnX((0, 1, 0) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(0) / 100,                             MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Graph 6 ;   position_vector =  { 0 , -150 , -35 } + displacement ;  X_axis_vector   =  turnY(turnZ(X_axis));  Y_axis_vector   =  turnY(turnZ(Y_axis));   bdr = 8;  d0  = -sin(u) ;  d1  =  cos(u) ;  c   = 1.01 + sin(u) ;  u   = t*pi/2 ;   graph_equation (x,y,z)  =  z/c  =  sin( 0.16 x*x  +  0.16 y*y )  ,                             x*x + y*y <= bdr*bdr ;   graph_colouring (x,y,z) =          (x*30) mod 256                          +         ((y*30) mod 256)*256                          + ((127.5(1-z/c)) mod 256)*256*256 ;   graph_boundary  =  {-bdr, bdr, -bdr, bdr, -c, c} ;   plot_accuracy   =   8 ;   X_axis =       (-d1,  d0 , 0) ;  Y_axis = turnX(( d0,  d1 , 0) ,0) ;  Z_axis = crossproduct(X_axis, Y_axis);   #Note: Below is how user input is obtained and used ;   displacement = moveOffset * cameraMatrix;   cameraMatrix = CameraAxisX(0, 0, 0),                 CameraAxisY(0, 0, 0),                 CameraAxisZ(0, 0, 0);   dfc = DistanceFromCamera(0);   moveOffset = OnCtrlPress (-MouseMotionX(0) * dfc,                            -MouseMotionY(0) * dfc,                             MouseMotionZ(0) * 10 );   turnAngle  = OnLeftClick (-MouseMotionX(0) / 100,                            -MouseMotionY(60)/ 100,                             MouseMotionZ(0) / 100 );   turnY(v) = v * rotation_matrix(cameraMatrix[1], turnAngle[0]) ;  turnZ(v) = v * rotation_matrix(    Z_axis     , turnAngle[2]) ;  turnX(v) = v * rotation_matrix(    X_axis     , turnAngle[1]) ;   rotation_matrix (u, angle) =         ( u[0]*u[0]*(1-cos(angle)) +      cos(angle) ,           u[0]*u[1]*(1-cos(angle)) - u[2]*sin(angle) ,           u[0]*u[2]*(1-cos(angle)) + u[1]*sin(angle) ),          ( u[1]*u[0]*(1-cos(angle)) + u[2]*sin(angle) ,           u[1]*u[1]*(1-cos(angle)) +      cos(angle) ,           u[1]*u[2]*(1-cos(angle)) - u[0]*sin(angle) ),          ( u[2]*u[0]*(1-cos(angle)) - u[1]*sin(angle) ,           u[2]*u[1]*(1-cos(angle)) + u[0]*sin(angle) ,           u[2]*u[2]*(1-cos(angle)) +      cos(angle) ) ;   crossproduct(u,v) = ( u[1]*v[2] - u[2]*v[1] ,                        u[2]*v[0] - u[0]*v[2] ,                        u[0]*v[1] - u[1]*v[0] );  ");
    graph_add(str);


    strcpy0(str, " type     =  Graph ;  name     =  Background Negative X ;  position_vector =  (-B, 0, 0);  X_axis_vector   =  ( 0, 1, 0);  Y_axis_vector   =  ( 0, 0, 1);  plot_accuracy   =  1 ;  graph_boundary  =  {-B,B,-B,B,0,1} ;  graph_equation  (x,y,z) = 0=0 ;  graph_colouring (x,y,z) = 160 + 160*256*256 ;  B = 100000000 ; ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Background Positive X ;  position_vector =  ( B, 0, 0);  X_axis_vector   =  ( 0, 1, 0);  Y_axis_vector   =  ( 0, 0, 1);  plot_accuracy   =  1 ;  graph_boundary  =  {-B,B,-B,B,0,1} ;  graph_equation  (x,y,z) = 0=0 ;  graph_colouring (x,y,z) = 160*256 + 160*256*256 ;  B = 100000000 ; ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Background Negative Y ;  position_vector =  ( 0,-B, 0);  X_axis_vector   =  ( 0, 0, 1);  Y_axis_vector   =  ( 1, 0, 0);  plot_accuracy   =  1 ;  graph_boundary  =  {-B,B,-B,B,0,1} ;  graph_equation  (x,y,z) = 0=0 ;  graph_colouring (x,y,z) = 160 ;  B = 100000000 ; ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Background Positive Y ;  position_vector =  ( 0, B, 0);  X_axis_vector   =  ( 0, 0, 1);  Y_axis_vector   =  ( 1, 0, 0);  plot_accuracy   =  1 ;  graph_boundary  =  {-B,B,-B,B,0,1} ;  graph_equation  (x,y,z) = 0=0 ;  graph_colouring (x,y,z) = 160*256 ;  B = 100000000 ; ");
    graph_add(str);

    strcpy0(str, " type     =  Graph ;  name     =  Background Positive Z ;  position_vector =  ( 0, 0, B);  X_axis_vector   =  ( 1, 0, 0);  Y_axis_vector   =  ( 0, 1, 0);  plot_accuracy   =  1 ;  graph_boundary  =  {-B,B,-B,B,0,1} ;  graph_equation  (x,y,z) = 0=0 ;  graph_colouring (x,y,z) = 160 + 160*256 + 160*256*256 ;  B = 100000000 ; ");
    graph_add(str);


    return TRUE;
}
