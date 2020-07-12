/**************************************************
    maincomponent.c
**************************************************/

#include <stdio.h>          // for sprintf()
#include <my_string.h>
#include "maincomponent.h"


char MainComponent_errorMessage[1000];


    Component *findComponent (MainComponent *mainComp, int* arrayIndex, int arraySize)
    {
        Component *comp;
        int i, j, d;
        int dimensions = mainComp->dimensions;

        if(arraySize > dimensions)
        {
            if(dimensions==0)
                sprintf(MainComponent_errorMessage,
                "Component '%s' is not an array.", mainComp->name);
            else
                sprintf(MainComponent_errorMessage,
                "Component '%s' is not a %d but a %d dimensional array.", mainComp->name, arraySize, dimensions);
            return NULL;
        }

        comp = mainComp->root;
        for(d=1; d<=arraySize; d++)
        {
            i = arrayIndex[d-1];
            j = mainComp->dimensionSize[dimensions-d];

            if(i<0 || i>=j)
            {
                sprintf(MainComponent_errorMessage,
                "Array index %d of '%s' is %d but must be < %d and >= 0.", d, mainComp->name, i, j);
                return NULL;
            }
            comp = comp->headChild;
            for(; i!=0; i--) comp = comp->nextChild;
        }
        return comp;
    }









    const int openBracket = 1;
    const int closeBracket = 2;
    const int a_comma = 3;
    const int a_leave = 4;


    Component *addNew (Component *current, int id)
    {
        Component *temp;

        if(current->ID==openBracket) // or previousID==closeBracket
        {
            current->headChild = newComponent (id);
            current->headChild->parent = current;
            current = current->headChild;
        }
        else if(id==a_comma)
        {
            while(id < current->parent->ID)
            { current = current->parent; }

            if(current->parent->ID != id)
            {
                temp = newComponent (id);
                if(current->prevChild!=NULL)
                    current->prevChild->nextChild = temp;
                else current->parent->headChild = temp;
                temp->parent = current->parent;
                current->parent = temp;
                temp->headChild = current;
                // important: current node is the same as before
            }
        }
        else if(id==closeBracket)
        {
            while(id<current->ID)                    // get to the openBracket node and delete it
            { current = current->parent; }           // this will essentially be the last child of parent

            if(current->prevChild!=NULL)
            {   current->prevChild->nextChild = current->headChild;
                current->headChild->prevChild = current->prevChild; // may not be necessary
            }
            else current->parent->headChild = current->headChild;
            current->headChild->parent = current->parent;
            current = current->headChild;
        }
        else
        {
            current->nextChild = newComponent (id);
            current->nextChild->prevChild = current;
            current->nextChild->parent = current->parent;
            current = current->nextChild;
        }
        return current;
    }


    void countDimensions (MainComponent *mainComp)
    {
        Component *temp1, *temp2;
        int i, j;
        for(i=0, temp1 = mainComp->root->headChild; temp1 != NULL; temp1 = temp1->headChild, i++)
        {
            for(j=0, temp2=temp1; temp2 != NULL; temp2 = temp2->nextChild, j++);
            mainComp->dimensionSize[i]=j;
        }
        mainComp->dimensions=i;
    }


    bool parseForDimensions (MainComponent *mainComp)
    {
        int i, start=0, stop=0, previousID;
        char c;
        Component *current = newComponent (openBracket);
        current->parent = NULL;
        previousID = openBracket;

        for(i=0; i < strlen(mainComp->strExpr); i++)
        {
            c = mainComp->strExpr[i];

            //if(c!='{' && c!=',')

            switch(c)
            {
                case '{':
                    if(previousID!=openBracket && previousID!=a_comma)
                    {
                        sprintf(MainComponent_errorMessage,
                        "ERROR on %s: At position %d, previous to '{' must be either '{' or ','", mainComp->name, i);
                    }
                    start = stop = i+1;
                    current = addNew(current, openBracket);
                    previousID = openBracket;
                    break;
                case '}':
                    if(previousID == a_leave || previousID == a_comma)
                    {
                        current = addNew(current, a_leave);
                        strcpy2 (current->strExpr, mainComp->strExpr, start, stop);
                    }
                    current = addNew(current, closeBracket);
                    previousID = closeBracket;
                    break;
                case ',':
                    if(previousID == a_leave)
                    {
                        current = addNew(current, a_leave);
                        strcpy2 (current->strExpr, mainComp->strExpr, start, stop);
                    }
                    start = stop = i+1;     // get ready to extract next leave
                    current = addNew(current, a_comma);
                    previousID = a_comma;
                    break;
                default:
                    stop++;
                    if(c!=' ' && c!='\n' && c!='\t' && c!='\r')
                        previousID = a_leave;
                    break;
            }
        }
        if(previousID==a_leave)
        {   current = addNew(current, a_leave);
            strcpy2 (current->strExpr, mainComp->strExpr, start, stop);
        }

        while(current->parent != NULL) current = current->parent;
        mainComp->root = current->headChild;
        countDimensions(mainComp);
        return true;
    }



MainComponent *removed_maincomponent = NULL;

MainComponent *newMainComponent (char* _name, char* _strExpr, char pN[4][10], int p)
{
    int i;
    MainComponent *mainComp;
    if(removed_maincomponent==NULL)
    {
        mainComp = (MainComponent*)FreeMemory;
        FreeMemory += sizeof(MainComponent);
    }
    else
    {   mainComp = removed_maincomponent;
        removed_maincomponent = removed_maincomponent->next;
    }
    for(i=0; i<p; i++)
        strcpy(mainComp->parameterName[i], pN[i]);
    mainComp->ID=0;
    mainComp->parameters=p;
    strcpy(mainComp->name,_name);
    strcpy(mainComp->strExpr,_strExpr);
    mainComp->next = NULL;
    mainComp->prev = NULL;
    return mainComp;
}

void freeMainComponent (MainComponent *mainComp)
{
    mainComp->prev = NULL;
    if(removed_maincomponent!=NULL)
        removed_maincomponent->prev = mainComp;
    mainComp->next = removed_maincomponent;
    removed_maincomponent = mainComp;
}

