/***********************************************
    _stdio.c

    IMPORTANT: Usage of standard stdio.h
    library makes the OS to crash on a call
    of sprintf(). Use sprintf0() instead.
***********************************************/

#include <_stdarg.h>
#include <_stdio.h>
#include <boolean_null.h>
#include "terminal.h"

#ifndef USE_STANDARD_STDIO_H



typedef int (*PUTCHAR_FUNCTION)(void*, char);     // variable of type 'pointer to a function'

PUTCHAR_FUNCTION get_putchar_function(void *SO)   // note: 'casting' of the pointer to an object's
{
    return &debug_putchar;
}



void fputc0 (char chr, void *SO)                 // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function(SO);
    call_putchar(SO, chr);
}

void fputc1 (mchar chr, void *SO)                // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function(SO);
    call_putchar(SO, (char)chr);
}

void fputs0 (const char* str, void *SO)                // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function (SO);
    if(!str) return;
    while(*str) { call_putchar(SO, (char)*str); str++; } // no newline at the end
}

void fputs1 (const mchar* str, void *SO)                 // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function (SO);
    if(!str) return;
    while(*str) { call_putchar(SO, (char)*str); str++; } // no newline at the end
}

void fputs4 (const mchar* str, int length, void *SO)      // SO = some_object
{
    const mchar* str_end;
    PUTCHAR_FUNCTION call_putchar;
    if(length < 0) { fputs1 (str, SO); return; }
    call_putchar = get_putchar_function (SO);
    if(!str) return;
    str_end = str + length;
    while(str != str_end)
    { call_putchar(SO, (char)*str); str++; }
}



void putc0 (char chr)
{ fputc0(chr,0); }

void putc1 (mchar chr)
{ fputc1(chr,0); }

void puts0 (const char* str)
{ fputs0(str,0); }

void puts1 (const mchar* str)
{ fputs1(str,0); }

void puts4 (const mchar* str, int length)
{ fputs4(str,length,0); }



#endif
