
/****************************************************************************
    gdt.c
    global descriptor table (gdt) for i86 processors. This handles
    the basic memory map for the system and permission levels
****************************************************************************/

#include "gdt.h"

extern void* memset(void* dest, char val, uint count);



// processor gdtr register points to base of gdt,
// this helps us set up the pointer.
struct _gdtr
{
    uint16b m_limit;    // size of gdt
    uint32b m_base;     // base address of gdt
};


// global descriptor table is an array of descriptors
static GDT_DESCRIPTOR gdtarr [MAX_DESCRIPTORS];

static struct _gdtr gdtr;   // gdtr data

static void gdt_install ()
{
    ASM_BEG
    ASM(lgdt [gdtr])
    ASM_END
}

// Setup a descriptor in the Global Descriptor Table
void gdt_set_descriptor(uint32b i, uintptrb base, uintptrb limit, uint8b access, uint8b grand)
{
    if (i > MAX_DESCRIPTORS)
        return;

    // null out the descriptor
    memset ((void*)&gdtarr[i], 0, sizeof (GDT_DESCRIPTOR));

    // set limit and base addresses
    gdtarr[i].baseLo    = (uint16b)(base & 0xffff);
    gdtarr[i].baseMid   = (uint8b)((base >> 16) & 0xff);
    gdtarr[i].baseHi    = (uint8b)((base >> 24) & 0xff);
    gdtarr[i].limit     = (uint16b)(limit & 0xffff);

    // set flags and grandularity bytes
    gdtarr[i].flags     = access;
    gdtarr[i].grand     = (uint8b)((limit >> 16) & 0x0f);
    gdtarr[i].grand    |= grand & 0xf0;
}

GDT_DESCRIPTOR* i86_gdt_get_descriptor (int i)  // returns descriptor in gdt
{
    if (i > MAX_DESCRIPTORS)
        return 0;
    return &gdtarr[i];
}

void i86_gdt_initialise ()
{
    // set up gdtr
    gdtr.m_limit = (sizeof (GDT_DESCRIPTOR) * MAX_DESCRIPTORS)-1;
    gdtr.m_base = (uint32b)gdtarr;

    // set null descriptor
    gdt_set_descriptor(0, 0, 0, 0, 0);

    // set default code descriptor
    gdt_set_descriptor (1,0,0xffffffff,
        I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_EXEC_CODE,
        I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

    // set default data descriptor
    gdt_set_descriptor (2,0,0xffffffff,
        I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,
        I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

    // install gdtr
    gdt_install ();
}
