/****************************************************************************
    Idt.c
    Interrupt Descriptor Table. The IDT is responsible for providing
    the interface for managing interrupts, installing, setting, requesting,
    generating, and interrupt callback managing.
****************************************************************************/

#include "idt.h"
#include "hal.h"


// describes the structure for the processors idtr register
struct _idtr
{
    uint16b limit;  // size of the interrupt descriptor table (idt)
    uint32b base;   // base address of idt
};

// interrupt descriptor table
static IDT_DESCRIPTOR idtarr [I86_MAX_INTERRUPTS];

// idtr structure used to help define the cpu's idtr register
static struct _idtr idtr;




static void i86_default_i_handler ()
{   hal_fault("i86_default_i_handler: Unhandled Exception");
    for(;;);
}
static void i86_i0_handler ()
{   hal_fault("Interrupt 0: Divide by 0");
    for(;;);
}
static void i86_i1_handler ()
{   hal_fault("Interrupt 1: Single step (Debugger)");
    for(;;);
}
static void i86_i2_handler ()
{   hal_fault("Interrupt 2: Non Maskable Interrupt (NMI) Pin");
    for(;;);
}
static void i86_i3_handler ()
{   hal_fault("Interrupt 3: Breakpoint (Debugger)");
    for(;;);
}
static void i86_i4_handler ()
{   hal_fault("Interrupt 4: Overflow");
    for(;;);
}
static void i86_i5_handler ()
{   hal_fault("Interrupt 5: Bounds check");
    for(;;);
}
static void i86_i6_handler ()
{   hal_fault("Interrupt 6: Undefined Operation Code (OPCode) instruction");
    for(;;);
}
static void i86_i7_handler ()
{   hal_fault("Interrupt 7: No coprocessor");
    for(;;);
}
static void i86_i8_handler ()
{   hal_fault("Interrupt 8: Double Fault");
    for(;;);
}
static void i86_i9_handler ()
{   hal_fault("Interrupt 9: Coprocessor Segment Overrun");
    for(;;);
}
static void i86_i10_handler ()
{   hal_fault("Interrupt 10: Invalid Task State Segment (TSS)");
    for(;;);
}
static void i86_i11_handler ()
{   hal_fault("Interrupt 11: Segment Not Present");
    for(;;);
}
static void i86_i12_handler ()
{   hal_fault("Interrupt 12: Stack Segment Overrun");
    for(;;);
}
static void i86_i13_handler ()
{   hal_fault("Interrupt 13: General Protection Fault (GPF)");
    for(;;);
}
static void i86_i14_handler ()
{   hal_fault("Interrupt 14: Page Fault");
    for(;;);
}
static void i86_i15_handler ()
{   hal_fault("Interrupt 15: Unassigned");
    for(;;);
}
static void i86_i16_handler ()
{   hal_fault("Interrupt 16: Coprocessor error");
    for(;;);
}
static void i86_i17_handler ()
{   hal_fault("Interrupt 17: Alignment Check (486+ Only)");
    for(;;);
}
static void i86_i18_handler ()
{   hal_fault("Interrupt 18: Machine Check (Pentium/586+ Only)");
    for(;;);
}



// installs idtr into processors idtr register
static void idt_install ()
{
    #ifdef _MSC_VER
    _asm lidt [idtr]
    #endif
}

// returns interrupt descriptor
IDT_DESCRIPTOR* i86_get_idt_descriptor (uint32b i)
{
    if (i>I86_MAX_INTERRUPTS)
        return 0;
    return &idtarr[i];
}


// installs a new interrupt handler
void i86_install_i_handler (uint32b i, uint16b flags, uint16b sel, FUNCTION handler)
{
    uint32b uiBase;
    if (i>I86_MAX_INTERRUPTS)
        return;
    if (!handler)
        return;
    // get base address of interrupt handler
    uiBase = (uint32b)handler;

    // store base address into idt
    idtarr[i].baseLo    =   (uint16b)(uiBase & 0xffff);
    idtarr[i].baseHi    =   (uint16b)((uiBase >> 16) & 0xffff);
    idtarr[i].reserved  =   0;
    idtarr[i].flags     =   (uint8b)flags;
    idtarr[i].sel       =   sel;
}


// initialise basic idt
void i86_idt_initialise (uint16b codeSel)
{
    int32b i;
    // set up idtr for processor
    idtr.limit = sizeof (IDT_DESCRIPTOR) * I86_MAX_INTERRUPTS -1;
    idtr.base = (uint32b)idtarr;

    // set default handlers
    for (i=0; i<I86_MAX_INTERRUPTS; i++)
        i86_install_i_handler (i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
            codeSel, i86_default_i_handler);

    i86_install_i_handler (0, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i0_handler);
    i86_install_i_handler (1, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i1_handler);
    i86_install_i_handler (2, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i2_handler);
    i86_install_i_handler (3, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i3_handler);
    i86_install_i_handler (4, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i4_handler);
    i86_install_i_handler (5, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i5_handler);
    i86_install_i_handler (6, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i6_handler);
    i86_install_i_handler (7, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i7_handler);
    i86_install_i_handler (8, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i8_handler);
    i86_install_i_handler (9, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i9_handler);
    i86_install_i_handler (10, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i10_handler);
    i86_install_i_handler (11, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i11_handler);
    i86_install_i_handler (12, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i12_handler);
    i86_install_i_handler (13, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i13_handler);
    i86_install_i_handler (14, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i14_handler);
    i86_install_i_handler (15, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i15_handler);
    i86_install_i_handler (16, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i16_handler);
    i86_install_i_handler (17, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i17_handler);
    i86_install_i_handler (18, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, i86_i18_handler);

    // finally, install idt
    idt_install ();
}
