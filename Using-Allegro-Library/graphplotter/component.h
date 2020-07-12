#ifndef _COMPONENT_H
#define _COMPONENT_H
/**************************************************
    component.h
**************************************************/

#include "structures.h"
#include "expression.h"
#include <my_stdint.h>

extern bool parseCompExpr (Component *component, int* arrayIndex, int arraySize, MainComponent *mainComp, ParsedSignature *pS);

extern char* unparseCompExpr (Component *component);
extern void evaluateCompExpr (Component *component, double* inputArguments);
extern int setCompID (Component *component, int id);
extern char* unparseCompExpr (Component *component);

extern Component *newComponent (int id);
extern void freeComponent (Component *comp);


extern char* FreeMemory;

#endif
