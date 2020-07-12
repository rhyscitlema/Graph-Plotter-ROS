#ifndef _MY_STDIO_H
#define _MY_STDIO_H
/***********************************
    my_stdio.h
***********************************/


extern int my_fputc (const char chr, void* some_object);
extern int my_fputs (const char* str, void* some_object);           // note: no newline at the end
extern int my_fprintf (void* some_object, const char* format, ...);

extern void set_debug_Pos(int x, int y);

#endif
