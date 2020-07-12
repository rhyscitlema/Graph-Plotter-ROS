/*
    component.c
*/

#include <stdio.h>          // for sprintf()
#include <my_string.h>
#include "parsedsignature.h"



    Component *removed_component = NULL;

    Component *newComponent (int id)
    {
        Component *comp;
        if(removed_component==NULL)
        {
            comp = (Component*)FreeMemory;
            FreeMemory += sizeof(Component);
        }
        else
        {   comp = removed_component;
            removed_component = removed_component->nextChild;
        }
        comp->ID = id;
        comp->qualifier = 0;
        comp->parent = NULL;
        comp->headChild = NULL;
        comp->nextChild = NULL;
        comp->prevChild = NULL;
        return comp;
    }

    void freeComponent (Component *comp)
    {
        comp->prevChild = NULL;
        if(removed_component!=NULL)
            removed_component->prevChild = comp;
        comp->nextChild = removed_component;
        removed_component = comp;
    }



    extern Expression *parseExpression (char* strExpr, char parameterName[4][10], int parameters);

    // arrayIndex[] is needed only for the construction of compFullName in case of an error
    bool parseCompExpr (Component *component, int* arrayIndex, int arraySize, MainComponent *mainComp, ParsedSignature *pS)
    {
        int i;
        char compFullName[50], str[10];
        Component *comp = component;

        while(true)
        {
            if(comp->headChild != NULL)
            {
                comp = comp->headChild;
                arrayIndex[arraySize++] = 0;
            }
            else
            {   // work done at leave node
                if(comp->expression==NULL)
                {
                    comp->expression = parseExpression (comp->strExpr, mainComp->parameterName, mainComp->parameters);
                    comp->qualifier = Expression_exprQualifier;

                    if(comp->expression==NULL)
                    {
                        strcpy(compFullName, mainComp->name);
                        for(i=0; i<arraySize; i++)
                        {
                            sprintf(str, "[%d]", arrayIndex[i]);
                            strcat(compFullName, str);
                        }

                        sprintf (ParsedSignature_errorMessage,
                        "ERROR at %s : %s", compFullName, Expression_errorMessage);
                        // change to strcat
                        return false;
                    }

                    if(mainComp->parameters==0)                  // if not a function
                    {
                        //if((comp->qualifier & Expression_TIME_DEPENDENT)!=0)  // if time dependent variable
                        {
                            if(pS->tailTobeEvaluated == NULL)
                                pS->headTobeEvaluated = comp;
                            else pS->tailTobeEvaluated->nextTobeEvaluated = comp;
                            pS->tailTobeEvaluated = comp;
                        }
                    }
                    else comp->qualifier |= A_FUNCTION;
                }

                // move to next nodes
                for( ; comp != component; comp = comp->parent, arraySize--)
                    if(comp->nextChild != NULL)
                        { comp = comp->nextChild; arrayIndex[arraySize-1]++; break; }
                if(comp==component) break;
            }
        }
        return true;
    }



    void evaluateCompExpr(Component *component, double* inputArguments)
    {
        Component *comp;

        /* Start of iterative algorithm */
        comp = component;
        while(true)
        {
            if(comp->headChild!=NULL)
                comp = comp->headChild;
            else
            {   // work done at leave node
                Expression_variable[comp->ID] = compute (comp->expression, inputArguments);

                // move to next nodes
                for( ; comp != component; comp = comp->parent)
                    if(comp->nextChild != NULL)
                        { comp = comp->nextChild; break; }
                if(comp==component) break;
            }
        }

        /* End of iterative algorithm */

        /* Start of recursive algorithm
        if(headChild==null)  // if a leave node
            Expression.variable[ID] = compute (inputArguments);
        else
            for(comp=headChild; comp!=null; comp = comp.nextChild)
                comp.evaluateCompExpr(arguments, size);
         End of recursive algorithm */
    }



    // returns the next ID that should follow the given component ID
    int setCompID (Component *component, int id)
    {
        Component *comp;

        /* Start of iterative algorithm */
        comp = component;
        while(true)
        {
            comp->ID = id;
            if(comp->headChild != NULL)
                comp = comp->headChild;
            else
            {   id+=1;  // work done at leave node

                // move to next nodes
                for(; comp != component; comp = comp->parent)
                    if(comp->nextChild != NULL)
                        { comp = comp->nextChild; break; }
                if(comp==component) break;
            }
        }
        return id;
        /* End of iterative algorithm */

        /* Start of recursive algorithm 
        ID = id;
        if(headChild==null) // if a leave node
            return id+1;
        else
            for(comp=headChild; comp!=null; comp = comp.nextChild)
                id = comp.setCompID(id);
        return id;
         End of recursive algorithm */
    }



    char* unparseCompExpr (Component *component)
    {
        Component *comp;
        static char originalForm[100];
        originalForm[0]=0;

        /* start of iterative algorithm */
        comp = component;
        while(true)
        {
            if(comp->headChild != NULL)
            {
                comp = comp->headChild;
                strcat(originalForm,"(");
            }
            else
            {   // work done at leave node
                strcat(originalForm, comp->strExpr);

                // move to next nodes
                for( ; comp != component; comp = comp->parent, strcat(originalForm,")"))
                    if(comp->nextChild != NULL)
                        { comp = comp->nextChild; strcat(originalForm,","); break; }
                if(comp==component) break;
            }
        }
        return originalForm;
        /* End of iterative algorithm */
    }

