#ifndef _MY_MATH_H
#define _MY_MATH_H
/***********************************
    my_math.h
    Math library
***********************************/

#include <my_stdint.h>
#include <math.h>
#define mysqrt(x) sqrt(x)
//#define mysqrt(x) my_sqrt(x)


#define PI 3.14159265358979323846264


// FLOAT is the general data structure for a 'number'
// This general structure might change
typedef float FLOAT;

//#define abs(x) ((x)<0 ? -(x) : (x))

extern FLOAT my_sqrt(FLOAT n);

#endif

