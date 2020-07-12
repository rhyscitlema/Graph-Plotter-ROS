/*
    expression.c

    Math expression parser:
    Parse and evaluate a math expression given in a string.

    This program implements a NEW algorithm. Find the details at:
    http://yemelitc.org/algorithms/expression-parsing-algorithm.html

    Copyright (c) 2014, Yemeli Tasse Cyrille <yemelitc@yemelitc.org>.

    Licensed under the Apache license.
    A copy of the License is included with the source code, and
    can be obtained at: http://www.apache.org/licenses/LICENSE-2.0
*/

#include <stdio.h>
#include <my_string.h>
#include <my_math.h>
#include "expression.h"
#include "parsedsignature.h"


/*
    const int openBracket        = 1027;
    const int closeBracket       = 1026;

    const int OR                 = 1025;
    const int AND                = 1024;
    const int NOT                = 1023;
    const int equal              = 1022;
    const int lessThan           = 1021;
    const int greaterThan        = 1020;
    const int lessThanOrEqual    = 1019;
    const int greaterThanOrEqual = 1018;

    const int plus               = 1017;
    const int minus              = 1016;
    const int negative           = 1015;
    const int times              = 1014;
    const int divide             = 1013;
    const int exponent           = 1012;
    const int modulo             = 1011;
    const int factorial          = 1010;

    const int ABS                = 1009;
    const int EXP                = 1008;
    const int LOG                = 1007;
    const int LNN                = 1006;
    const int SIN                = 1005;
    const int COS                = 1004;
    const int TAN                = 1003;
    const int SQRT               = 1002;
    const int FLOOR              = 1001;

    const int afunction          = 1000;
    const int aconstant          = 999;
    const int aoperator          = 998;
    const int aparameter         = 900;
    const int avariable          = 899;

    const int A_FUNCTION     = 1;
    const int TIME_DEPENDENT = 2;
*/
    #define openBracket         1027
    #define closeBracket        1026

    #define OR                  1025
    #define AND                 1024
    #define NOT                 1023
    #define equal               1022
    #define lessThan            1021
    #define greaterThan         1020
    #define lessThanOrEqual     1019
    #define greaterThanOrEqual  1018

    #define plus                1017
    #define minus               1016
    #define negative            1015
    #define times               1014
    #define divide              1013
    #define exponent            1012
    #define modulo              1011
    #define factorial           1010

    #define ABS                 1009
    #define EXP                 1008
    #define LOG                 1007
    #define LNN                 1006
    #define SIN                 1005
    #define COS                 1004
    #define TAN                 1003
    #define SQRT                1002
    #define FLOOR               1001

    #define afunction           1000
    #define aconstant           999
    #define aoperator           998
    #define aparameter          900
    #define avariable           899


    int Expression_exprQualifier;
    char Expression_errorMessage[1000];
    const int VS = 900;
    double Expression_variable[900];
    char variableStack[10][50];
    int variableStackSize = 0;
    ParsedSignature *Expression_parsedSignature;




Expression *removed_expression = NULL;

Expression *newExpression (int id)
{
    Expression *expr;
    if(removed_expression==NULL)
    {
        expr = (Expression*)FreeMemory;
        FreeMemory += sizeof(Expression);
    }
    else
    {   expr = removed_expression;
        removed_expression = removed_expression->right;
    }
    expr->ID = id;
    expr->left = expr->right = NULL;
    return expr;
}

void freeExpression (Expression *expr)
{
    expr->left = NULL;
    if(removed_expression!=NULL)
        removed_expression->left = expr;
    expr->right = removed_expression;
    removed_expression = expr;
}





    static Expression *addNew (Expression *node, int id)
    {
        if(id == exponent)                  // if is right-associative
            while(id > node->ID)
                node = node->parent;
        else                                // else is left-associative
            while(id >= node->ID)
                node = node->parent;

        if(id == closeBracket)
        {
            node = node->parent;             // this is the '(' node
            node->right = node->right->right;  // it is being removed
        }
        else
        {
            Expression *temp = node->right;
            node->right = newExpression(id);
            node->right->parent = node;
            node = node->right;
            node->left = temp;
        }
        return node;
    }

    Expression *addNew2 (Expression *node, int id)
    {
        node->right = newExpression(id);
        node->right->parent = node;
        node = node->right;
        return node;
    }



    Expression *parseExpression (char* strExpr, char parameterName[4][10], int parameters)
    {
        int bracketOpened = 0;
        int qualifier = 0;
        int arrayIndex[4];
        int arrayIndexSize = 0;
        int callingParameters;
        int i, j, k;
        char c;
        int position;
        int previousID;
        int bracket_count_type2;
        char str[100], str2[100], str3[10];
        Component *component;
        Expression *tempExpr;
        Expression *node;

        ParsedSignature *pS = Expression_parsedSignature;

        node = newExpression(openBracket);
        node->parent = NULL;
        previousID = openBracket;


        strcat(strExpr, " ");
        for(i = 0; i < strlen(strExpr); i++)
        {
            c = strExpr[i];


            if(c >= '0' && c <= '9')
            {
                if(previousID == aconstant || previousID == avariable || previousID == factorial)
                {
                    sprintf(Expression_errorMessage,
                    "Expected '*' before position %d of expression.", i);
                    return NULL;
                }

                if(previousID == closeBracket)
                    node = addNew (node, times);

                k=0;
                do
                {   str[k++] = c;
                    c = strExpr[++i];
                } while(c == '.' || (c >= '0' && c <= '9'));
                str[k] = 0;

                node = addNew2 (node, aconstant);
                sscanf(str, "%lf", &node->constant);
                previousID = aconstant;
            }


            if(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                position = i;
                k=0;
                do
                {   str[k++] = c;
                    c = strExpr[++i];
                } while(c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
                str[k] = 0;

                     if(0==strcmp(str, "or"))  c = '|';
                else if(0==strcmp(str, "and")) c = '&';
                else if(0==strcmp(str, "not")) c = '!';
                else
                {
                    if(previousID == avariable || previousID == factorial)
                    {
                        sprintf(Expression_errorMessage,
                        "Expected '*' before position %d of expression.", i);
                        return NULL;
                    }

                    if(previousID == closeBracket || previousID == aconstant)
                        node = addNew (node, times);

                         if(0==strcmp(str, "exp")) { node = addNew2 (node, EXP); previousID = afunction; }
                    else if(0==strcmp(str, "log")) { node = addNew2 (node, LOG); previousID = afunction; }
                    else if(0==strcmp(str, "ln" )) { node = addNew2 (node, LNN); previousID = afunction; }
                    else if(0==strcmp(str, "abs")) { node = addNew2 (node, ABS); previousID = afunction; }
                    else if(0==strcmp(str, "sin")) { node = addNew2 (node, SIN); previousID = afunction; }
                    else if(0==strcmp(str, "cos")) { node = addNew2 (node, COS); previousID = afunction; }
                    else if(0==strcmp(str, "tan")) { node = addNew2 (node, TAN); previousID = afunction; }
                    else if(0==strcmp(str, "sqrt")) { node = addNew2 (node, SQRT); previousID = afunction; }
                    else if(0==strcmp(str, "floor")) { node = addNew2 (node, FLOOR); previousID = afunction; }

                    else if(0==strcmp(str, "e"))
                    { node = addNew2 (node, aconstant); node->constant = 2.71828182845904523536028; previousID = aconstant; }
                    else if(0==strcmp(str, "PI"))
                    { node = addNew2 (node, aconstant); node->constant = PI; previousID = aconstant; }

                    else if(pS == NULL)
                    {
                        sprintf(Expression_errorMessage,
                        "Unexpected variable '%s' at position %d of expression.", str, i);
                        return NULL;
                    }

                    else 
                    {
                        for(j=0; j<parameters; j++)
                            if(0==strcmp(str, parameterName[j]))
                            { node = addNew2 (node, aparameter + j); break; }

                        if(j < parameters); // if a parameter was found just now
                        else if(0==strcmp(str, "t")) { node = addNew (node, 0); qualifier |= TIME_DEPENDENT; }
                        else if(0==strcmp(str, "x")) { node = addNew (node, aparameter+0); }
                        else if(0==strcmp(str, "y")) { node = addNew (node, aparameter+1); }
                        else if(0==strcmp(str, "z")) { node = addNew (node, aparameter+2); }
                        else
                        {
                            arrayIndexSize = 0;
                            for( ; c != ' ' && c == '['; c = strExpr[++i])
                            {
                                c = strExpr[++i];
                                bracket_count_type2 = 0;
                                k=0;
   
                                for( ; c != ']' || bracket_count_type2 != 0; c = strExpr[++i])
                                {
                                    str2[k++] = c;
                                    if(c == '[') bracket_count_type2++;
                                    else if(c == ']') bracket_count_type2--;

                                    if(i+1 == strlen(strExpr))
                                    {
                                        sprintf (Expression_errorMessage,
                                        "Expected ']' before end of expression.");
                                        return NULL;
                                    }
                                }
                                str2[k]=0;

                                tempExpr = parseExpression(str2, NULL, 0);
                                if(tempExpr != NULL)
                                    arrayIndex[arrayIndexSize++] = (int)compute(tempExpr, NULL);
                                else // also check if it is a finite number
                                {
                                    strcpy(str, Expression_errorMessage);
                                    sprintf(Expression_errorMessage,
                                    "Error at '%s' index %d: %s", str, arrayIndexSize+1, str);
                                    return NULL;
                                }
                            }

                            strcpy(str2, str);
                            for(j = 0; j < arrayIndexSize; j++)
                            {
                                sprintf(str3, "[%d]", arrayIndex[j]);
                                strcat(str2, str3);
                            }

                            for(j = 0; j < variableStackSize; j++)
                                if(0==strcmp(variableStack[j], str2))
                                {
                                    sprintf(Expression_errorMessage,
                                    "Deadlock error on variable '%s' at position %d of expression.", str2, position+1);
                                    return NULL;
                                }

                            strcpy(variableStack[variableStackSize], str2);
                            variableStackSize++;
                            component = parseComponent(pS, str, arrayIndex, arrayIndexSize);
                            variableStackSize--;

                            if(component == NULL) { strcpy (Expression_errorMessage, ParsedSignature_errorMessage); return NULL; }
 
                            if((component->qualifier & TIME_DEPENDENT) != 0) qualifier |= TIME_DEPENDENT;

                            node = addNew2 (node, component->ID);

                            callingParameters = getComponentParameters(pS, str);
                            if(callingParameters != 0)
                            {
                                node->exprComp = component;
                                previousID = afunction;
                            }
                        }
                        if(previousID != afunction) previousID = avariable;
                    }
                }
            }


            if(previousID == closeBracket || previousID == aconstant
            || previousID == avariable || previousID == factorial)
            {
                if(c == ' ' || c == '\n' || c == '\t' || c == '\r'); // do nothing

                else if(c == '|') { node = addNew (node, OR); previousID = aoperator; }

                else if(c == '&') { node = addNew (node, AND); previousID = aoperator; }

                else if(c == '!')
                {
                    sprintf(Expression_errorMessage,
                    "Exprected 'or', 'and' or '(' before position %d of expression.", i);
                    return NULL;
                }
 
                else if(c == '=') { node = addNew (node, equal); previousID = aoperator; }

                else if(c == '<')
                {
                    c = strExpr[i+1];
                    if(c == '=') { node = addNew (node, lessThanOrEqual); i++; }
                    else { node = addNew (node, lessThan); }
                    previousID = aoperator;
                }

                else if(c == '>')
                {
                    c = strExpr[i+1];
                    if(c == '=') { node = addNew (node, greaterThanOrEqual); i++; }
                    else { node = addNew (node, greaterThan); }
                    previousID = aoperator;
                }

                else if(c == '+') { node = addNew (node, plus); previousID = aoperator; }

                else if(c == '-') { node = addNew (node, minus); previousID = aoperator; }

                else if(c == '*') { node = addNew (node, times); previousID = aoperator; }

                else if(c == '/') { node = addNew (node, divide); previousID = aoperator; }

                else if(c == '^') { node = addNew (node, exponent); previousID = aoperator; }

                else if(c == '%') { node = addNew (node, modulo); previousID = aoperator; }

                else if(c == '!') { node = addNew (node, factorial); previousID = factorial; }

                else if(c == ')')
                {
                    if(bracketOpened == 0)
                    {
                        sprintf(Expression_errorMessage,
                        "A close without open bracket at position %d of expression.", i);
                        return NULL;
                    }
                    bracketOpened--;
                    node = addNew (node, closeBracket);
                    previousID = closeBracket;
                }
                else if(c == '(')
                {
                    node = addNew (node, times);
                    node = addNew2 (node, openBracket);
                    bracketOpened++;
                    previousID = openBracket;
                }
                else
                {   sprintf(Expression_errorMessage,
                    "Unrecognised '%c' at position %d of expression.", c, i);
                    return NULL;
                }
            }

            else
            {
                if(c == ' ' || c == '\n' || c == '\t' || c == '\r'); // do nothing

                else if(c == '-')
                {
                    if(previousID == afunction)
                    {
                        sprintf (Expression_errorMessage,
                        "Unexpected function before position %d of expression.", i);
                        return NULL;
                    }
                    node = addNew2 (node, negative);
                    previousID = aoperator;
                }

                else if(c == '+')
                {
                    if(previousID >= FLOOR && previousID <= ABS)
                    {
                        sprintf (Expression_errorMessage,
                        "Unexpected function before position %d of expression.", i);
                        return NULL;
                    }
                    //node = addNew2 (node, positive);
                    previousID = aoperator;
                }

                else if(c == '(')
                {
                    node = addNew2 (node, openBracket);
                    bracketOpened++;
                    previousID = openBracket;
                }

                else if(c == '!')
                {
                    if(previousID != openBracket && previousID != AND && previousID != OR)
                    {
                        sprintf (Expression_errorMessage,
                        "Exprected 'or', 'and' or '(' before position %d of expression.", i);
                        return NULL;
                    }
                    node = addNew (node, NOT);
                    previousID = aoperator;
                }

                else
                {   sprintf (Expression_errorMessage,
                    "Error at or before position %d of expression.", i);
                    return NULL;
                }
            }
        }


        if(bracketOpened == 1)
        {
            sprintf (Expression_errorMessage, "Expect 1 closed bracket at the end of expression.");
            return NULL;
        }
        if(bracketOpened > 1)
        {
            sprintf (Expression_errorMessage,
            "Expect %d closed brackets at the end of expression.", bracketOpened);
            return NULL;
        }
        if(!(previousID == closeBracket || previousID == aconstant
          || previousID == avariable || previousID == factorial))
        {
            sprintf (Expression_errorMessage, "Invalid ending of expression.");
            return NULL;
        }

        Expression_exprQualifier = qualifier;
        while(node->parent != NULL) node = node->parent;
        return node->right;
    }



    double compute (Expression *expr, double* inputArguments)
    {
        double callingArguments[1]; // used to for calling a function
        double l, r;
        int k;
        long ans;
        int ID;
        double* variable;

        ID = expr->ID;
        variable = Expression_variable;

        if(expr->left != NULL) l = compute(expr->left, inputArguments); else l = 0;
        if(expr->right != NULL) r = compute(expr->right, inputArguments); else r = 0;

        if(expr->exprComp != NULL)
        {
            callingArguments[0] = r;
            evaluateCompExpr(expr->exprComp, callingArguments);
            return variable[expr->exprComp->ID];
        }
        if(ID <= avariable) return variable[ID];
        if(ID == aconstant) return expr->constant;
        if(ID >= aparameter && ID <= aparameter+50)
            return inputArguments[ID - aparameter];

        switch(ID)
        {
            case OR:
                if(l!=0 || r!=0) return 1;
                else return 0;
            case AND:
                if(l!=0 && r!=0) return 1;
                else return 0;
            case NOT:
                if(r==0) return 1;
                else return 0;
            case equal:
                if(l==r) return 1;
                else return 0;
            case lessThan:
                if(l<r) return 1;
                else return 0;
            case greaterThan:
                if(l>r) return 1;
                else return 0;
            case lessThanOrEqual:
                if(l<=r) return 1;
                else return 0;
            case greaterThanOrEqual:
                if(l>=r) return 1;
                else return 0;

            case plus:
                return l + r;
            case minus:
                return l - r;
            case negative:
                return -r;
            case times:
                return l * r;
            case divide:
                if(r==0) return 2000000000;
                return l / r;
            case exponent:
                return (double)pow(l, r);
            case modulo:
                return ((long)l % (long)r);

            case factorial:
                k = (int)l;
                if(k<0) return 0;
                ans = 1;
                for(; k!=0; k--)
                    ans *= k;
                return ans;

            case ABS:
                if(r<0) return -r;
                return r;
            case EXP:
                return (double)exp(r);
            case LOG:
                if(r<=0) return 2000000000;
                return (double)log10(r);
            case LNN:
                if(r<=0) return 2000000000;
                return (double)log(r);
            case SIN:
                return (double)sin(r);
            case COS:
                return (double)cos(r);
            case TAN:
                return (double)tan(r);
            case SQRT:
                return (double)sqrt(r);
            case FLOOR:
                return ((long)r);

            default:
                return 0;
        }
    }

