#ifndef _PIC_H_INCLUDED
#define _PIC_H_INCLUDED
/****************************************************************************
    pic.h
    8259 Programmable Interrupt Controller
****************************************************************************/

#include "_stdint.h"


//-----------------------------------------------
//    Devices connected to the PICs. May be useful
//    when enabling and disabling irq's
//-----------------------------------------------

// The following devices use PIC 1 to generate interrupts
#define I86_PIC_IRQ_TIMER           0
#define I86_PIC_IRQ_KEYBOARD        1
#define I86_PIC_IRQ_SERIAL2         3
#define I86_PIC_IRQ_SERIAL1         4
#define I86_PIC_IRQ_PARALLEL2       5
#define I86_PIC_IRQ_DISKETTE        6
#define I86_PIC_IRQ_PARALLEL1       7

// The following devices use PIC 2 to generate interrupts
#define I86_PIC_IRQ_CMOSTIMER       0
#define I86_PIC_IRQ_CGARETRACE      1
#define I86_PIC_IRQ_AUXILIARY       4
#define I86_PIC_IRQ_FPU             5
#define I86_PIC_IRQ_HDC             6

//-----------------------------------------------
//    Command words are used to control the devices
//-----------------------------------------------

// Command Word 2 bit masks. Use when sending commands
#define I86_PIC_OCW2_MASK_L1        1       //00000001
#define I86_PIC_OCW2_MASK_L2        2       //00000010
#define I86_PIC_OCW2_MASK_L3        4       //00000100
#define I86_PIC_OCW2_MASK_EOI       0x20    //00100000
#define I86_PIC_OCW2_MASK_SL        0x40    //01000000
#define I86_PIC_OCW2_MASK_ROTATE    0x80    //10000000

// Command Word 3 bit masks. Use when sending commands
#define I86_PIC_OCW3_MASK_RIS       1       //00000001
#define I86_PIC_OCW3_MASK_RIR       2       //00000010
#define I86_PIC_OCW3_MASK_MODE      4       //00000100
#define I86_PIC_OCW3_MASK_SMM       0x20    //00100000
#define I86_PIC_OCW3_MASK_ESMM      0x40    //01000000
#define I86_PIC_OCW3_MASK_D7        0x80    //10000000


// Read data byte from pic
extern uint8b i86_pic_read_data (uint8b picNum);

// Send a data byte to pic
extern void i86_pic_send_data (uint8b data, uint8b picNum);

// Send operational command to pic
extern void i86_pic_send_command (uint8b cmd, uint8b picNum);

// Enables and disables interrupts
extern void i86_pic_mask_irq (uint8b irqmask, uint8b picNum);

// initialise pic
extern void i86_pic_initialise (uint8b base0, uint8b base1);

// enable all interrupt requests
extern  void i86_enable_irqs();

// disable all interrupt requests
extern  void i86_disable_irqs();

// enable interrupt request
extern  void i86_enable_irq(uint8b irq);

// disable interrupt request
extern  void i86_disable_irq(uint8b irq);

#endif
