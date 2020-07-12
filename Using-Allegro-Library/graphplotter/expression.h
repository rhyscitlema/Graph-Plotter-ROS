#ifndef _EXPRESSION_H
#define _EXPRESSION_H
/*
    expression.h
*/

#include "structures.h"


extern char Expression_errorMessage[];

extern double Expression_variable[];

extern int Expression_exprQualifier;

extern ParsedSignature *Expression_parsedSignature;

extern double compute (Expression *expr, double* inputArguments);

extern Expression *parseExpression (char* strExpr, char parameterName[4][10], int parameters);

#define A_FUNCTION      1
#define TIME_DEPENDENT  2


#endif
