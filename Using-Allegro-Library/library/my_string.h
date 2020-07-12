#ifndef _MY_STRING_H
#define _MY_STRING_H
/****************************************************************
    my_string.h
****************************************************************/

/*
** These functions are commented because the same functions
** provided by the compiler library will be used instead.

extern char* strcpy(char* str1, const char* str2);
extern uint strlen (const char* str );
extern int strcmp (const char* str1, const char* str2);

extern void* memcpy (void* dest, const void* src, uint count);
extern void* memset (void* dest, char val, uint count);
extern int memcmp (const void* mem1, const void* mem2, uint count);
*/
#include<string.h>

extern char* trim (char* str);                  // trim (remove) leading and trailing spaces
extern int strtoint (const char* str);          // get integer from string (decimal digits)
extern int strtohex (const char* str);          // get integer from string (hexadecimal digits)
extern int strtobin (const char* str);          // get integer from string (binary digits)
extern char* inttostr (int n, char* str);       // get string from integer (decimal digits)
extern char* hextostr (int n, char* str);       // get string from integer (hexadecimal digits)
extern char* bintostr (int n, char* str);       // get string from integer (binary digits)
extern char* strreverse (char* str, int len);   // reverse string of size 'len'

// to be removed
extern char* strcpy2 (char* str1, const char* str2, const int start, const int stop);
extern char* strcat2 (char* str1, const char* str2, const int start, const int stop);

#endif
