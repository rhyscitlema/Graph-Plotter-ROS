#ifndef _GDT_H_INCLUDED
#define _GDT_H_INCLUDED
/****************************************************************************
    gdt.h
    global descriptor table (gdt) for i86 processors. This handles
    the basic memory map for the system and permission levels
****************************************************************************/

#include "_stdint.h"

// maximum amount of descriptors allowed
#define MAX_DESCRIPTORS 3


/***     gdt descriptor access bit flags.    ***/

// set access bit
#define I86_GDT_DESC_ACCESS         0x0001          //00000001

// descriptor is readable and writable. default: read only
#define I86_GDT_DESC_READWRITE      0x0002          //00000010

// set expansion direction bit
#define I86_GDT_DESC_EXPANSION      0x0004          //00000100

// executable code segment. Default: data segment
#define I86_GDT_DESC_EXEC_CODE      0x0008          //00001000

// set code or data descriptor. default: system defined descriptor
#define I86_GDT_DESC_CODEDATA       0x0010          //00010000

// set dpl bits
#define I86_GDT_DESC_DPL            0x0060          //01100000

// set "in memory" bit
#define I86_GDT_DESC_MEMORY         0x0080          //10000000


/**    gdt descriptor grandularity bit flags    ***/

// masks out limit-Hi (High 4 bits of limit)
#define I86_GDT_GRAND_LIMITHI_MASK  0x0F            //00001111

// set os defined bit
#define I86_GDT_GRAND_OS            0x10            //00010000

// set if 32bit. default: 16 bit
#define I86_GDT_GRAND_32BIT         0x40            //01000000

// 4k grandularity. default: none
#define I86_GDT_GRAND_4K            0x80            //10000000


/* gdt descriptor: defines the properties of a specific
  memory block and permissions.
*/
struct gdt_descriptor
{
    uint16b    limit;  // bits 0-15 of segment limit
    uint16b    baseLo; // bits 0-23 of base address
    uint8b     baseMid;
    uint8b     flags;  // descriptor access flags
    uint8b     grand;
    uint8b     baseHi; // bits 24-32 of base address
};
//typedef struct gdt_descriptor GDT_DESCRIPTOR
#define GDT_DESCRIPTOR struct gdt_descriptor

// Setup a descriptor in the Global Descriptor Table
extern void gdt_set_descriptor(uint32b i, uintptrb base, uintptrb limit, uint8b access, uint8b grand);

// returns descritor
extern GDT_DESCRIPTOR* i86_gdt_get_descriptor (int i);

extern void i86_gdt_initialise ();

#endif
