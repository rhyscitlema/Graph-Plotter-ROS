/********************************************************
    my_string.c
********************************************************/

#include <my_string.h>
#include <my_stdint.h>

/*
** These functions are commented because the same functions
** provided by the compiler library will be used instead.

// compare two strings
int strcmp (const char* str1, const char* str2)
{
    while ( *str1==*str2 && *str1)
    { ++str1; ++str2; }

    if(*str1 > *str2) return 1;
    else if(*str1 < *str2) return -1;
    else return 0;
}

// copies string s2 to s1, thus s1=s2
char* strcpy(char* s1, const char* s2)
{
    char *s1_p = s1;
    do{ *s1_p++ = *s2;
    } while(*s2++);
    return s1_p;
}

// returns length of string
uint strlen ( const char* str )
{
    uint len=0;
    while (str[len++]);
    return len;
}

// copies 'count' bytes from src to dest
void* memcpy(void* dest, const void* src, uint count)
{
    const char* sp = (const char*)src;
    char* dp = (char*)dest;
    for(; count!=0; count--) *dp++ = *sp++;
    return dest;
}

// sets count bytes of dest to val
void* memset(void* dest, char val, uint count)
{
    unsigned char* temp = (unsigned char*)dest;
    for(; count!=0; count--) temp[count] = val;
    return dest;
}

// compare bytes in memory
int memcmp(const void* mem1, const void* mem2, uint count)
{
    const char* m1 = (char*)mem1;
    const char* m2 = (char*)mem2;
    for(; count!=0; count--)
        if(*m1++ != *m2++)
            return false;
    return true;
}

** The above functions are commented because the same functions
** provided by the compiler library will be used instead.
*/



/* trim (remove) leading and trailing spaces */
char* trim(char* str)
{
    int i=0, j;
    for(j=0; str[j]==' '; j++);     // trim leading spaces

    for(; str[j]!=0; j++)           // copy middle characters
        str[i++] = str[j];
    for(j=i-1; str[j]==' '; j--)    // trim trailing spaces
        i--;
    str[i]=0;                       // end string
    return str;
}

/* get integer from string (decimal digits) */
int strtoint (const char* str)
{
    int ans=0;
    bool neg=false;
    if(*str=='-')                   // if number is negative
    { neg=true; str++; }            // that is first character is '-'
    do
    {   ans *= 10;
        if(*str<'0' || *str>'9')    // if not a decimal digit, then
            return 0x7FFFFFFF;      // error, return maximum integer type.
        ans += (*str++ - '0');      // else add decimal digit.
        str++;
    } while(*str!=0);               // while not end of string
    if(neg) return -ans;
    else return ans;
}

/* get integer from string (hexadecimal digits) */
int strtohex (const char* str)
{
    int ans=0;
    return ans;
}

/* get integer from string (binary digits) */
int strtobin (const char* str)
{
    int ans=0;
    return ans;
}


/* get string from integer (decimal digits) */
char* inttostr (int n, char* str)
{
	int len=0, neg=0;

	if(n<0)                         // if number is negative
	{	n=-n;                       // then put first character '-'
		str[len++]='-';
		neg=1;                      // offset for strreverse()
	}
	do{
        str[len++] = n%10 + '0';    // get and put new decimal digit
	} while ((n /= 10) > 0);
	str[len] = '\0';		        // end string

	strreverse(str+neg, len-neg);   // reverse the digits
    return str;
}

/* get string from integer (hexadecimal digits) */
char* hextostr (int n, char* str)
{
    char digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int len=0, neg=0;
    
	if(n<0)                         // similar procedure as that
	{	n=-n;                       // used in inttostr()
		str[len++]='-';
		neg=1;
	}
    do{
        str[len++] = digits[n & 0xF]; // get and put new hexadecimal digit
    } while((n=n>>4)>0);
	str[len] = '\0';		        // end string

    strreverse(str+neg, len-neg);
    return str;
}

/* get string from integer (binary digits) */
char* bintostr (int n, char* str)
{
	int len=0, neg=0;

	if(n<0)                         // similar procedure as that
	{	n=-n;                       // used in inttostr()
		str[len++]='-';
		neg=1;
	}
    do{
        str[len++] = (n & 1) + '0'; // get and put new binary digit
    } while((n=n>>1)>0);
	str[len] = '\0';		        // end string

    strreverse(str+neg, len-neg);
    return str;
}

/* reverse string of size 'len' */
char* strreverse (char* str, int len)
{
    char temp;                      // used for swapping
    int i;
    for(i=0; i<len/2; i++)          // from start to middle of string
    {
        temp = str[i];              // swap values at position 'i' and 'len-1-i'
        str[i] = str[len-1-i];
        str[len-1-i] = temp;
    }
    return str;
}






// to be removed
char* strcpy2 (char *s1, const char* s2, const int start, const int stop)
{
    int i;
    char *s1_p = s1;
    for(s2 += start, i=stop-start; i!=0; i--)
        *s1_p++ = *s2++;
    *s1_p = '\0';
    return s1_p;
}

char* strcat2 (char *s1, const char* s2, const int start, const int stop)
{
    int i, j;
    j = strlen(s1);
    for(i = start; i < stop; i++, j++)
        s1[j] = s2[i];
    s1[j] = '\0';
    return s1;
}



