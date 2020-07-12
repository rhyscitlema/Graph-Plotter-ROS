/*
    tools.h

    The Graph Plotter 3D Interface.
*/
#ifndef _TOOLS_H
#define _TOOLS_H


#include "user_main_gui.h"

extern void tools_initialise ();


extern void tools_apply_change();

extern void tools_remove_object();


extern void tools_add_graph();

extern void tools_add_camera();


extern int remove_all_objects();


extern void tools_set_time();

//extern void tools_pause_resume();

extern void tools_forward_backward();


extern void tools_time_flow_faster();

extern void tools_time_flow_slower();

extern void tools_time_flow_normal();

extern void tools_time_flow_custom();


#endif
