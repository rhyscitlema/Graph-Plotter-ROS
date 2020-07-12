#ifndef _GRAPH_H
#define _GRAPH_H
/***************************************************************
    graph.h
***************************************************************/

#include "object3D.h"


typedef struct _Graph
{
    Object3D obj;
    Expression *equation;
    Expression *extraConditions;
    Expression *colouringFunction;
    struct _Graph *prev, *next;
} Graph;

extern void graph_processAll ();
extern void graph_paintAll (Camera *cmr);
extern bool addGraph (char* _signature);

#endif

