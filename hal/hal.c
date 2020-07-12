/****************************************************************************
    hal.c
    Hardware Abstraction Layer Interface for i86 architecture

    The Hardware Abstraction Layer (HAL) provides an abstract interface
    to control the basic motherboard hardware devices. This is accomplished
    by abstracting hardware dependencies behind this interface.
****************************************************************************/

#include "hal.h"
#include "cpu.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"


void hal_initialise ()      // initialise hardware devices
{
    disable_ints ();

    // initialise motherboard controllers and system timer
    i86_cpu_initialise ();
    i86_pic_initialise (0x20,0x28);
    i86_disable_irqs();

    i86_pit_start_counter (100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
    i86_pit_initialise ();

    enable_ints ();
}


void hal_shutdown ()        // shutdown hardware devices
{ i86_cpu_shutdown(); }

void hal_restart ()         // restart hardware devices
{ i86_cpu_restart(); }

void  enable_ints ()        // enable all hardware interrupts
{ _asm sti }

void  disable_ints ()       // disable all hardware interrupts
{ _asm cli }


uint8b inportb (uint16b portid)  // read byte from device using port mapped io
{
    _asm {
    mov dx, word ptr [portid]
    in  al, dx
    mov byte ptr [portid], al
    }
    return (uint8b)portid;
}

void  outportb (uint16b portid, uint8b value)    // write byte to device through port mapped io
{
    _asm {
    mov al, byte ptr [value]
    mov dx, word ptr [portid]
    out dx, al
    }
}

// delay a bit
void delayABit()
{
    _asm {
    mov ecx, 0xFF
    delay:
    loop delay
    }
}

// enable interrupt request
void enable_irq(uint8b irq)
{ i86_enable_irq(irq); }


void interruptdone (uint32b intno)    // notify hal that interrupt is done
{
    // insure its a valid hardware irq
    if (intno > 16)
        return;

    // test if we need to send end-of-interrupt to second pic
    if (intno >= 8)
        i86_pic_send_command (I86_PIC_OCW2_MASK_EOI, 2);

    // always send end-of-interrupt to primary pic
    i86_pic_send_command (I86_PIC_OCW2_MASK_EOI, 1);
}


// sets new interrupt vector
void  install_i_handler (int32b intno, FUNCTION handler )
{
    // install interrupt handler! This overwrites prev interrupt descriptor
    i86_install_i_handler (intno, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, 0x8, handler);
}

// returns current interrupt vector
FUNCTION get_i_handler (int32b intno)
{
    FUNCTION handler;
    uint32b addr;

    // get the descriptor from the idt
    IDT_DESCRIPTOR* desc = i86_get_idt_descriptor (intno);
    if (!desc)
        return 0;

    // get address of interrupt handler
    addr = desc->baseLo | (desc->baseHi << 16);

    // return interrupt handler
    handler = (FUNCTION)addr;
    return handler;
}



void sound (uint frequency)      // output sound to speaker
{
    // sets frequency for speaker. frequency of 0 disables speaker
    outportb (0x61, 3 | (uint8b)(frequency<<2) );
}



extern int puts0 (const char* string);
extern void screen_refresh ();

void hal_fault (const char* fault)
{
    // clear interrupts to prevent double fault
    disable_ints();

    // print fault message and halt
    puts0 ("\n ***  Hardware failure *** : \n");
    puts0 (fault);
    puts0 ("\n");
    screen_refresh();
}
