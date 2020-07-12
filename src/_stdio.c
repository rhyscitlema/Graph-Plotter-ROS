/*
    _stdio.c
*/

#include <_stdarg.h>
#include <_stdio.h>
#include "terminal.h"


typedef int (*PUTCHAR_FUNCTION)(void*, mchar);     // variable of type 'pointer to a function'

PUTCHAR_FUNCTION get_putchar_function(void *SO)   // note: 'casting' of the pointer to an object's
{
    return &debug_putchar;
}


void fputc2 (char chr, void *SO)                 // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function(SO);
    call_putchar(SO, chr);
}

void putc2 (mchar chr)
{ fputc2(chr,0); }


bool file_is_modified (const lchar* filename) { return 0; }

bool file_to_string (const  char* filename,  char** filecontent_ptr, long *contentSize_ptr) { return 0; }

bool string_to_file (const char* filename, const char* filecontent, long contentSize) { return 0; }

