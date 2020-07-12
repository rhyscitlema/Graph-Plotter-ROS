#ifndef _PARSEDSIGNATURE_H
#define _PARSEDSIGNATURE_H
/***************************************************************
    parsedsignature.h
***************************************************************/

#include "structures.h"
#include "maincomponent.h"


extern char ParsedSignature_errorMessage[];

extern ParsedSignature *parseSignature (char* signature);

extern Component *parseComponent (ParsedSignature *pS, char* name, int* arrayIndex, int arraySize);

extern int getComponentParameters (ParsedSignature *pS, char* name);

extern void commitAllTobeEvaluated (ParsedSignature *pS);

extern ParsedSignature *newParsedSignature ();

extern MainComponent *findMainComponent (ParsedSignature *pS, char* name);


#endif
