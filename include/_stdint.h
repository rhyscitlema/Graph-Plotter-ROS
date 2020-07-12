#ifndef _MY_STDINT_H
#define _MY_STDINT_H

/******************
    _stdint.h
*******************/



#ifndef NULL
#define NULL (void*)0
#endif

#ifndef null
#define null (void*)0
#endif

#ifndef bool
#define bool int
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef true
#define true TRUE
#endif

#ifndef false
#define false FALSE
#endif



/* function() type */
typedef void (*FUNCTION)();



/* Undefined width integer types */
typedef signed          sint;
typedef unsigned        uint;

/* Exact-width integer types */
typedef char            int8b;
typedef signed char     sint8b;
typedef unsigned char   uint8b;
typedef short           int16b;
typedef signed short    sint16b;
typedef unsigned short  uint16b;
typedef int             int32b;
typedef signed int      sint32b;
typedef unsigned int    uint32b;
typedef long long           int64b;
typedef signed long long    sint64b;
typedef unsigned long long  uint64b;

/* Minimum-width integer types */
typedef char            least_int8b;
typedef signed char     least_sint8b;
typedef unsigned char   least_uint8b;
typedef short           least_int16b;
typedef signed short    least_sint16b;
typedef unsigned short  least_uint16b;
typedef int             least_int32b;
typedef signed int      least_sint32b;
typedef unsigned int    least_uint32b;
typedef long long           least_int64b;
typedef signed long long    least_sint64b;
typedef unsigned long long  least_uint64b;

/* Fastest minimum-width integer types 
   Not actually guaranteed to be fastest for all purposes
   Here we use the exact-width types for 8 and 16-bit ints. 
*/
typedef char            fast_int8b;
typedef signed char     fast_sint8b;
typedef unsigned char   fast_uint8b;
typedef short           fast_int16b;
typedef signed short    fast_sint16b;
typedef unsigned short  fast_uint16b;
typedef int             fast_int32b;
typedef signed int      fast_sint32b;
typedef unsigned int    fast_uint32b;
typedef long long           fast_int64b;
typedef signed long long    fast_sint64b;
typedef unsigned long long  fast_uint64b;

/* nteger types capable of holding object pointers */
typedef int      intptrb;
typedef signed   sintptrb;
typedef unsigned uintptrb;

/* Greatest-width integer types */
typedef long long          intmaxb;
typedef signed long long   sintmaxb;
typedef unsigned long long uintmaxb;

/* Limits of exact-width integer types */
#define MIN_SINT8b  (-128) 
#define MIN_SINT16b (-32768)
#define MIN_SINT32b (-2147483647 - 1)
#define MIN_SINT64b (-9223372036854775807LL - 1)

#define MAX_SINT8b  127
#define MAX_SINT16b 32767
#define MAX_SINT32b 2147483647
#define MAX_SINT64b 9223372036854775807LL

#define MAX_UINT8b  0xff /* 255U */
#define MAX_UINT16b 0xffff /* 65535U */
#define MAX_UINT32b 0xffffffff  /* 4294967295U */
#define MAX_UINT64b 0xffffffffffffffffULL /* 18446744073709551615ULL */

/* Limits of minimum-width integer types */
#define MIN_LEAST_SINT8b  MIN_SINT8b
#define MIN_LEAST_SINT16b MIN_SINT16b
#define MIN_LEAST_SINT32b MIN_SINT32b
#define MIN_LEAST_SINT64b MIN_SINT64b

#define MAX_LEAST_SINT8b  MAX_SINT8b
#define MAX_LEAST_SINT16b MAX_SINT16b
#define MAX_LEAST_SINT32b MAX_SINT32b
#define MAX_LEAST_SINT64b MAX_SINT64b

#define MAX_LEAST_UINT8b  MAX_UINT8b
#define MAX_LEAST_UINT16b MAX_UINT16b
#define MAX_LEAST_UINT32b MAX_UINT32b
#define MAX_LEAST_UINT64b MAX_UINT64b

/* Limits of fastest minimum-width integer types */
#define MIN_FAST_SINT8b  MIN_SINT8b
#define MIN_FAST_SINT16b MIN_SINT16b
#define MIN_FAST_SINT32b MIN_SINT32b
#define MIN_FAST_SINT64b MIN_SINT64b

#define MAX_FAST_SINT8b  MAX_SINT8b
#define MAX_FAST_SINT16b MAX_SINT16b
#define MAX_FAST_SINT32b MAX_SINT32b
#define MAX_FAST_SINT64b MAX_SINT64b

#define MAX_FAST_UINT8b  MAX_UINT8b
#define MAX_FAST_UINT16b MAX_UINT16b
#define MAX_FAST_UINT32b MAX_UINT32b
#define MAX_FAST_UINT64b MAX_UINT64b


/* Limits of integer types capable of holding
    object pointers */ 
#define MIN_SINTPTR MIN_SINT32b
#define MAX_SINTPTR MAX_SINT32b
#define MAX_UINTPTR MAX_UINT32b

/* Limits of greatest-width integer types */
#define MIN_SINTMAXb MIN_SINT64b
#define MAX_SINTMAXb MAX_SINT64b
#define MAX_UINTMAXb MAX_UINT64b

#endif

