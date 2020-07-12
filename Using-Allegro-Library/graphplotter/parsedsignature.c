/***************************************************************
    parsedsignature.c
***************************************************************/

#include <stdio.h>          // for sprintf()
#include <my_string.h>
#include "parsedsignature.h"

char ParsedSignature_errorMessage[1000];


    MainComponent *findMainComponent (ParsedSignature *pS, char* name)
    {
        char errorMessage[100];
        MainComponent *mainComp;
        for(mainComp = pS->headMainComp; mainComp != NULL; mainComp = mainComp->next)
            if(strcmp(mainComp->name, name)==0)
                return mainComp;
        sprintf (errorMessage, "Cannot find component with name '%s' .\n", name);
        strcpy (ParsedSignature_errorMessage, errorMessage);
        // change to strcat
        return NULL;
    }


    ParsedSignature *parseSignature (char* signature)
    {
        // Basically, here we separate components.
        // A valid component is of the form:
        // " componentName = stringExpression; "
        // Components are separated by the character ';'.
        // If componentName is invalid then the component is ignored.

        ParsedSignature *pS;
        MainComponent *temp;
        char name[1000], expr[1000];
        bool onGetName;
        bool firstTime;
        int startOfName, stopOfName;
        int startOfExpr, stopOfExpr;
        int startOfParm, stopOfParm;
        int i, len;
        char parameterName[4][10];
        int parameters;
        char c;

        pS = newParsedSignature();
        len = strlen(signature);
        onGetName = true;
        i = 0;

        while(true) // while getting components
        {
            if(i>=len) break; c=signature[i];
            parameters = 0;

            if(onGetName)
            {
                while(c==' ' || c=='\n' || c=='\t' || c=='\r')   // ignore leading spaces
                { i++; if(i>=len) break; c=signature[i]; }
                if(i>=len) break;

                if(c=='_' || (c>='a' && c<='z') || (c>='A' && c<='Z'))
                {
                    startOfName=i;
                    do
                    {   i++; if(i>=len) break; c=signature[i];
                    } while(c=='_' || (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'));
                    if(i>=len) break;
                    stopOfName=i;
                    strcpy2 (name, signature, startOfName, stopOfName);

                    while(c==' ' || c=='\n' || c=='\t' || c=='\r')   // ignore trailing spaces
                    { i++; if(i>=len) break; c=signature[i]; }
                    if(i>=len) break;

                    if(c=='(')
                    {
                        while(true)
                        {
                            firstTime=true;
                            while(true)
                            {
                                i++; if(i>=len) break; c=signature[i];
                                if(c==' ' || c=='\n' || c=='\t' || c=='\r')
                                    { if(!firstTime) break; }    // if error
                                else if(c==';') break;
                                else if(c==')') break;
                                else if(c==',') break;
                                else if(c=='_' || (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9'))
                                {
                                    if(firstTime) { startOfParm = stopOfParm = i; firstTime=false; }
                                    else stopOfParm=i;
                                }
                                else break;
                            }
                            if(i>=len) break;
                            if(c!=')' && c!=',') break;
                            else 
                            {   if(!firstTime) strcpy2 (parameterName[parameters++], signature, startOfParm, stopOfParm+1);
                                if(c==')') break;
                            }
                        }
                        if(i>=len) break;
                        if(c==')')
                        {
                            do { i++; if(i>=len) break; c=signature[i];
                            } while(c==' ' || c=='\n' || c=='\t' || c=='\r');   // ignore trailing spaces
                            if(i>=len) break;
                        }
                        else { while(c!=';') { i++; if(i>=len) break; c=signature[i]; } i++; continue; }
                    }
                    if(c=='=')
                    {   onGetName=false;
                        startOfExpr=i+1;
                    }
                    else { while(c!=';') { i++; if(i>=len) break; c=signature[i]; } i++; } // if it does not stop well
                }
                else { while(c!=';') { i++; if(i>=len) break; c=signature[i]; } i++; } // if it does not start well
            }

            if(!onGetName) // that is if 'onGetExpression'
            {
                while(c!=';') { i++; if(i>=len) break; c=signature[i]; }
                stopOfExpr = i++;
                strcpy2 (expr, signature, startOfExpr, stopOfExpr);
                temp = newMainComponent (name, expr, parameterName, parameters);

                if(pS->headMainComp==NULL)
                    pS->headMainComp = temp;
                else
                {   temp->next = pS->headMainComp;
                    pS->headMainComp->prev = temp;
                    pS->headMainComp = temp;
                }
                onGetName=true;
            }
        }

        pS->headTobeEvaluated=NULL;
        pS->tailTobeEvaluated=NULL;
        pS->availableID=1;
        return pS;
    }


    Component *parseComponent (ParsedSignature *pS, char* name, int* arrayIndex, int arraySize)
    {
        Component *comp;
        MainComponent *mainComp = findMainComponent (pS, name);
        if(mainComp==NULL) return NULL;

        if(mainComp->ID==0)  // if first time to meet this component
        {
            if(!parseForDimensions(mainComp)) { strcpy(ParsedSignature_errorMessage, MainComponent_errorMessage); return NULL; }
            mainComp->ID = pS->availableID;
            pS->availableID = setCompID (mainComp->root, pS->availableID);
        }
        comp = findComponent (mainComp, arrayIndex, arraySize);
        if(comp==NULL) { strcpy (ParsedSignature_errorMessage, MainComponent_errorMessage); return NULL; }

        //printf("%s = %s\n", mainComp->name, unparseCompExpr(mainComp->root));

        if(!parseCompExpr(comp, arrayIndex, arraySize, mainComp, pS)) { return NULL; }
        return comp;

        //sprintf(ParsedSignature_errorMessage,
        //"Component with name '%s' takes %d arguments but %d are provided."
        //, name, mainComp->parameters, arguments);
    }

    int getComponentParameters (ParsedSignature *pS, char* name)
    {
        MainComponent *mainComp = findMainComponent (pS, name);
        if(mainComp==NULL) return 0;
        return mainComp->parameters;
    }

    void commitAllTobeEvaluated (ParsedSignature *pS)
    {
        Component *comp;
        for(comp = pS->headTobeEvaluated; comp != NULL; comp = comp->nextTobeEvaluated)
            evaluateCompExpr(comp, NULL);
    }




ParsedSignature *removed_parsedsignature = NULL;

ParsedSignature *newParsedSignature ()
{
    ParsedSignature *pS;
    if(removed_parsedsignature==NULL)
    {
        pS = (ParsedSignature*)FreeMemory;
        FreeMemory += sizeof(ParsedSignature);
    }
    else
    {   pS = removed_parsedsignature;
        //removed_parsedsignature = removed_parsedsignature->next;
    }
    return pS;
}

void freeParsedSignature (ParsedSignature *pS)
{
    /*pS->prev = NULL;
    if(removed_parsedsignature!=NULL)
        removed_parsedsignature->prev = pS;
    pS->next = removed_parsedsignature;
    removed_parsedsignature = pS;*/
}

