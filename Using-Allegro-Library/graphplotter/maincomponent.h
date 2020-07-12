#ifndef _MAINCOMPONENT_H
#define _MAINCOMPONENT_H
/**************************************************
    maincomponent.h
**************************************************/

#include "structures.h"
#include "component.h"

extern char MainComponent_errorMessage[];

extern Component *findComponent (MainComponent *mainComp, int* arrayIndex, int arraySize);

extern bool parseForDimensions (MainComponent *mainComp);

extern MainComponent *newMainComponent (char* name, char* strExpr, char parameterName[4][10], int parameters);

extern void freeMainComponent (MainComponent *mainComp);


#endif
