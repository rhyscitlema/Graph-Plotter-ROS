#ifndef _IDT_H
#define _IDT_H
/***************************************************************************
    idt.h
    Interrupt Descriptor Table. The IDT is responsible for providing
    the interface for managing interrupts, installing, setting, requesting,
    generating, and interrupt callback managing.
***************************************************************************/

// We can test new architecture here as needed

#include "_stdint.h"


// i86 defines 256 possible interrupt handlers (0-255)
#define I86_MAX_INTERRUPTS      256

// must be in the format 0D110, where D is descriptor type
#define I86_IDT_DESC_BIT16      0x06    //00000110
#define I86_IDT_DESC_BIT32      0x0E    //00001110
#define I86_IDT_DESC_RING1      0x40    //01000000
#define I86_IDT_DESC_RING2      0x20    //00100000
#define I86_IDT_DESC_RING3      0x60    //01100000
#define I86_IDT_DESC_PRESENT    0x80    //10000000


// interrupt descriptor
struct idt_descriptor
{
    uint16b baseLo;     // bits 0-16 of interrupt routine (ir) address
    uint16b sel;        // code selector in gdt
    uint8b  reserved;   // reserved, shold be 0
    uint8b  flags;      // bit flags. Set with flags above
    uint16b baseHi;     // bits 16-32 of ir address
};
//typedef
#define IDT_DESCRIPTOR  struct idt_descriptor

// return interrupt descriptor
extern IDT_DESCRIPTOR* i86_get_idt_descriptor (uint32b i);

// install interrupt handler. When INT is fired, it will call this callback
extern void i86_install_i_handler (uint32b i, uint16b flags, uint16b sel, FUNCTION handler);

// initialise basic idt
extern void i86_idt_initialise (uint16b codeSel);


#endif
