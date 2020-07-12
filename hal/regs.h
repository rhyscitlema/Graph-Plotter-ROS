#ifndef _REGS_H_INCLUDED
# define _REGS_H_INCLUDED
/****************************************************************************
    regs.h
    processor register structures and declarations. This interface abstracts
    register names behind a common, portable interface
****************************************************************************/

#ifndef ARCH_X86
#error "[regs.h] platform not implimented. Define ARCH_X86 for HAL"
#endif


#include <_stdint.h>


// 32 bit registers
struct _R32BIT
{
    uint32b eax, ebx, ecx, edx, esi, edi, ebp, esp, eflags;
    uint8b cflag;
};

// 16 bit registers
struct _R16BIT
{
    uint16b ax, bx, cx, dx, si, di, bp, sp, es, cs, ss, ds, flags;
    uint8b cflag;
};

// 16 bit registers expressed in 32 bit registers
struct _R16BIT32
{
    uint16b ax, axh, bx, bxh, cx, cxh, dx, dxh;
	uint16b si, di, bp, sp, es, cs, ss, ds, flags;
	uint8b cflags;
};

// 8 bit registers
struct _R8BIT
{
    uint8b al, ah, bl, bh, cl, ch, dl, dh;
};

// 8 bit registers expressed in 32 bit registers
struct _R8BIT32
{
    uint8b al, ah; uint16b axh; 
	uint8b bl, bh; uint16b bxh; 
	uint8b cl, ch; uint16b cxh; 
	uint8b dl, dh; uint16b dxh; 
};

// 8 and 16 bit registers union
union _INTR16
{
    struct _R16BIT x;
    struct _R8BIT h;
};

// 32 bit, 16 bit and 8 bit registers union
union _INTR32
{
	struct _R32BIT x;
	struct _R16BIT32 l;
	struct _R8BIT32 h;
};

#endif
