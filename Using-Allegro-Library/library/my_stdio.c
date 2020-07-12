/***********************************
    my_stdio.c
***********************************/

#include <stdarg.h>
#include <my_stdio.h>
#include <my_string.h>
#include "../terminal/terminal.h"


/*
void putchar (const char chr)
{

}
void puts(const char* str)
{
}
void printf(const char* format, ...)
{
}
*/



typedef int (*PUTCHAR_FUNCTION)(void*, const char);     // variable of type 'pointer to a function'

PUTCHAR_FUNCTION get_putchar_function(void *SO)   // note: 'casting' of the pointer to an object's
{
    return &debug_putchar;
}



int my_fputc (const char chr, void *SO)                 // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function(SO);
    return call_putchar(SO, chr);
}

int my_fputs (const char* str, void *SO)                // SO = some_object
{
    PUTCHAR_FUNCTION call_putchar = get_putchar_function (SO);
    for(; *str != 0; str++)
        call_putchar(SO, *str);                         // no newline at the end
    return 0;
}

int my_fprintf (void *SO, const char* format, ...)      // SO = some_object
{
    char to_str[64];                    // integer to string, maximum of 64 characters
    char chr, *str;
    int n;
    PUTCHAR_FUNCTION call_putchar;

    va_list varg;                       // start variable argument list
    va_start(varg, format);             // ...not sure how these things works...!

    call_putchar = get_putchar_function(SO);

    for(; *format != 0; format++)
    {
        if(*format!='%')
            call_putchar(SO, *format);
        else
        {
            format++;
            switch(*format)
            {
            case 'c':
                chr = (char)(va_arg(varg, int));    // get next argument (was passed as an integer)
                call_putchar(SO, chr);
                break;
            case 's':
                str = va_arg(varg, char*);      // get next argument as a char* (string)
                for(; *str != 0; str++) call_putchar(SO, *str);
                break;
            case 'd':
                n = va_arg(varg, int);          // get next argument as an integer
                str = inttostr(n, to_str);
                for(; *str != 0; str++) call_putchar(SO, *str);
                break;
            case 'x':
                n = va_arg(varg, int);          // get next argument as an integer
                str = hextostr(n, to_str);
                for(; *str != 0; str++) call_putchar(SO, *str);
                break;
            case 'b':
                n = va_arg(varg, int);          // get next argument as an integer
                str = bintostr(n, to_str);
                for(; *str != 0; str++) call_putchar(SO, *str);
                break;
            default:
                call_putchar(SO, '%');
                call_putchar(SO, *format);
                break;
            }
        }
    }
    va_end(varg);   // end variable argument list
    return 0;
}

