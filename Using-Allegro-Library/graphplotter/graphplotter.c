/*
    graphplotter.c
    Graph Plotter 3D
    Plot 3D graphs of equations in x, y and z
*/

#include <stdio.h>
#include <my_stdio.h>
#include <my_string.h>
#include "../hardware/keyboard.h"
#include "../hardware/screen.h"
#include "../hardware/mouse.h"
#include "../mcursor/mcursor.h"
#include "../terminal/terminal.h"
#include "camera.h"
#include "graph.h"



void graphplotter ()
{
    Camera *cmr;
    int count=0;
    char str[1000];
    //bool update_done;



    /* initialise system utilities */
    mcursor_initialise();
    //terminal_initialise();

    my_fputs("  Rhyscitlema 3D Graph Plotter\n", NULL);
    my_fputs("  Plot 3D graphs of equations in x, y and z\n", NULL);
    //my_fputs("    Left-click to select and move an Object (use scrolling too).\n", NULL);
    //my_fputs("    Press and maintain Ctrl to turn Camera or Object. Rotate with Ctrl+Right.\n", NULL);
    //my_fputs("    The program IS working... I trust you will realize the rest...!\n", NULL);
    my_fputs(" Press Escape to Quit.\n", NULL);



    sprintf(str, " Name = First Camera;\n t1 = t*PI/360;   't' is the reserved parameter for 'time';\n\n pst0 = 15000cos(t1);\n pst1 = 15000sin(t1);\n drt0 = -sin(t1);\n drt1 = cos(t1);\n\n position = {pst0, pst1, 0};       try: position = {0, 0, 0} ;\n direction = {-drt1, drt0, 0};    with: direction = {1, 0, 0} ;\n\n rotation_angle = 0;\n zoom_amount = 500;\n top_left_point = {0, 0};    not editable;\n width_height = {%d, %d};    not editable;\n\n Note: A statement must be terminated by a ';'\n A statement is said to be a 'component' if it is of the form:\n    component_name = component_expression ;\n A statement that is not a valid component is IGNORED.\n Above there are valid components used to identify the camera.\n Modify their component_expressions so to modify the camera. ;", SCR.XRes, SCR.YRes);
    addCamera(str);

    addGraph ("\n position = {3000, 0, 0};\n direction = {-0.7, 0, 0.7};\n\n rotation_angle = 6t;    't' is the reserved parameter for 'time';\n\n boundary = {-bdr, bdr, -bdr, bdr, -1, 1};\n accuracy = 1;\n transparency = 0;    not editable;\n\n bdr = 800;\n k = bdr-100;\n\n graph_equation =   z=0 ,\n                             x <= k+50*sin(y/50) and x >= -k-50*sin(y/50)\n                      and y <= k+50*sin(x/50) and y >= -k-50*sin(x/50) ;\n\n graph_colouring (x,y,z) =\n                           (abs(x)%256)\n                       + (abs(y)%256)*256\n                       + (abs(z)%256)*256*256 ;\n");

    addGraph (" position = {0, -3000, 0};\n direction = {d0, d1, -0.7};\n rotation_angle = 0;\n accuracy = 10;\n transparency = 0;    not editable;\n boundary = {-bdr, bdr, -bdr, bdr, -c, c};    bdr = 800;\n\n t1 = t*PI/60;   't' is the reserved parameter for 'time';\n d0 = -sin(t1);\n d1 = cos(t1);\n a = 250;    b = 250;    c = 100(1.01 + sin(t1));\n\n graph_equation =  z/c = sin((x/a)^2 + (y/b)^2) , x*x+y*y<=bdr*bdr;\n\n graph_colouring (x,y,z) = (abs(x)%256) + (abs(y)%256)*256\n                                     + ((127.5(1-z/c))%256)*256*256;\n\n  Note: A statement must be terminated by a ';'\n A statement is said to be a 'component' if it is of the form:\n    component_name = component_expression ;\n A statement that is not a valid component is IGNORED.\nModify the component_expressions above and click on 'Edit'. ;");

    addGraph ("\n position = {0, 3000,  0};\n direction = {-0.347, -0.526, 0.777};\n rotation_angle = t * f(2);\n\n f(u) = 3u;    this is a user-defined function;\n\n boundary = {-bdx, bdx, -bdy, bdy, -bdz, bdz};\n accuracy = 10;\n transparency = 0;    not editable;\n\n bdx = 400;   bdy = 400;    bdz = 500;\n\n a = b;    b = 150;    c=120;\n\n graph_equation =   z/c = (x/a)^3 - 3(x/a) + (y/b)^3 - 3(y/b);\n\n graph_colouring (x,y,z) = \n                           ( (127.5*(1 + x / bdx)) % 256)\n                       + ( (127.5*(1 + y / bdy)) % 256)*256 \n                       + ( (127.5*(1 + z / bdz)) % 256)*256*256 ;\n");

    addGraph ("\n position = {-3000, 0, 0};\n direction = {0.05, 0.948, 0.315};\n rotation_angle = 10t;\n\n boundary = {-bdr, bdr, -bdr, bdr, -1000, 1000};\n transparency = 0;   not editable;\n\n accuracy = 50;\n Note: greater accuracy and greater boundary reduce the\n speed of execution. Remove this graph and notice the change. ;\n\n a = 250;  b = a;     c = 150;    bdr = 550;\n\n graph_equation =    y/b = - tan(z/c) * x/a ,\n                               x*x + y*y <= bdr*bdr ;\n\n graph_colouring (x,y,z) =\n                           (abs(x)%256)\n                       + (abs(y)%256)*256\n                       + (abs(z)%256)*256*256 ;\n");

    addGraph ("\n u = t*PI/180;  a = 2000;\n position = { ( 4a + a * sin(20u)) cos(u)  , \n                     ( 4a + a * sin(20u)) sin(u)  ,\n                         a * cos(20u)    };\n this is the vector equation of the curve called  'toroidal spiral' ;\n\n direction = { 1, 0, 0 };\n rotation_angle = 0;\n transparency = 0;    not editable;\n accuracy = 2;\n\n boundary = {-bdr, bdr, -bdr, bdr, -bdr, bdr};\n bdr = 500;\n graph_equation =   (x*x + y*y +z*z) / (bdr*bdr) = 1 ;\n                              this is the equation of a 'sphere' ;\n\n graph_colouring (x,y,z) =\n                           ( (127.5*(1 + x / bdr)) % 256)\n                       + ( (127.5*(1 + y / bdr)) % 256)*256 \n                       + ( (127.5*(1 + z / bdr)) % 256)*256*256 ;\n");



    /* initialise user inputs */
    clearkey();
    mouse_clear_motion();
    mouse_clear_buttons();
    Expression_variable[0] = 0;

    while(1)
    {
        //update_done = false;
        Expression_variable[0] += 1;

        /* update user inputs */
        kybrd_update();
        mouse_update();

        /* System response to user input */
        if(getkey()>>8 == KEY_ESC)
            break;


        /* update system logic */
        mcursor_process();
        camera_processAll();
        graph_processAll();
        mouse_clear_motion();


        /* update screen */

        camera_paintInitialiseAll();

        for(cmr = headCamera; cmr!=NULL; cmr = cmr->next)
            graph_paintAll(cmr);
 
        camera_paintFinaliseAll();

        set_debug_Pos(8*20, 16*5); my_fprintf(NULL, " (%d) ",count); count++;
        mcursor_paint();
        screen_refresh();
        //update_done = true;
    }
}

