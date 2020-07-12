/*******************************
    my_math.c
    Math functions
*******************************/

#include <my_math.h>


/* square root of a number using the Babylonian method */
FLOAT my_sqrt(FLOAT n)
{
    FLOAT x=n, y;
    //for(x=0, y=n; y!=0; y>>=1)
    //    x+=1;
    //x = 1<<((x+1)/2);           // get nearest power of 2

    if(x==0) return 0;
    while(1)
    {   y = (x + n/x)/2;
        if (y >= x) break;
        x = y;
    }

    return x;               // why are these two results different?
//  return sqrt(n);         // in fact, why is the above code giving a wrong result?
}
