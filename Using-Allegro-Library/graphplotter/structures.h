#ifndef _STRUCTURES_H
#define _STRUCTURES_H


typedef struct _ParsedSignature
{
    int availableID;
    struct _MainComponent *headMainComp;
    struct _Component *headTobeEvaluated;
    struct _Component *tailTobeEvaluated;
} ParsedSignature;


typedef struct _Expression
{
    int ID;
    double constant;
    struct _Component *exprComp;
    struct _Expression *left, *right, *parent;
} Expression;


typedef struct _Component
{
    int ID;                         // 1=>open bracket, 2=>closed bracket, 3=>a comma, 4=>a leave, >4 => processed.
    int qualifier;                  // if = 0 then not qualified because expression is not parsed.
    char strExpr[300];              // if null then node is a comma, else it is a leave.
    int strExprLength;
    struct _Expression *expression;  // if null then node strExpr is not parsed.
    struct _Component *parent;
    struct _Component *headChild;    // if headChild=null then it is a leave node.
    struct _Component *prevChild;
    struct _Component *nextChild;    // if nextChild=null then it is the last neighbour.
    struct _Component *nextTobeEvaluated;
} Component;


typedef struct _MainComponent
{
    int ID;                 // 0 => is not parsed, >0 => is parsed
    char name[50];
    char strExpr[500];
    int parameters;         // number of parameters
    char parameterName[4][10];
    int dimensions;         // number of dimensions
    int dimensionSize[4];
    struct _Component *root;         // Note: the root is the only one to never have a nextChild.
    struct _MainComponent *prev, *next;
} MainComponent;


#endif

