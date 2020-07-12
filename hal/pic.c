
/****************************************************************************
    pic.c
    8259 Programmable Interrupt Controller
****************************************************************************/

#ifndef ARCH_X86
#error "[pic.cpp for i86] requires i86 architecture. Define ARCH_X86"
#endif

#include "pic.h"
#include "hal.h"


//-----------------------------------------------
//    Controller Registers
//-----------------------------------------------

// PIC 1 register port addresses
#define I86_PIC1_REG_COMMAND                0x20
#define I86_PIC1_REG_STATUS                 0x20
#define I86_PIC1_REG_DATA                   0x21
#define I86_PIC1_REG_IMR                    0x21

// PIC 2 register port addresses
#define I86_PIC2_REG_COMMAND                0xA0
#define I86_PIC2_REG_STATUS                 0xA0
#define I86_PIC2_REG_DATA                   0xA1
#define I86_PIC2_REG_IMR                    0xA1

//-----------------------------------------------
//    Initialization Command Bit Masks
//-----------------------------------------------

// Initialization Control Word 1 bit masks
#define I86_PIC_ICW1_MASK_IC4               0x1     //00000001
#define I86_PIC_ICW1_MASK_SNGL              0x2     //00000010
#define I86_PIC_ICW1_MASK_ADI               0x4     //00000100
#define I86_PIC_ICW1_MASK_LTIM              0x8     //00001000
#define I86_PIC_ICW1_MASK_INIT              0x10    //00010000

// Initialization Control Words 2 and 3 do not require bit masks

// Initialization Control Word 4 bit masks
#define I86_PIC_ICW4_MASK_UPM               0x1     //00000001
#define I86_PIC_ICW4_MASK_AEOI              0x2     //00000010
#define I86_PIC_ICW4_MASK_MS                0x4     //00000100
#define I86_PIC_ICW4_MASK_BUF               0x8     //00001000
#define I86_PIC_ICW4_MASK_SFNM              0x10    //00010000

//-----------------------------------------------
//    Initialization Command 1 control bits
//-----------------------------------------------

#define I86_PIC_ICW1_IC4_EXPECT             1       //1
#define I86_PIC_ICW1_IC4_NO                 0       //0
#define I86_PIC_ICW1_SNGL_YES               2       //10
#define I86_PIC_ICW1_SNGL_NO                0       //00
#define I86_PIC_ICW1_ADI_CALLINTERVAL4      4       //100
#define I86_PIC_ICW1_ADI_CALLINTERVAL8      0       //000
#define I86_PIC_ICW1_LTIM_LEVELTRIGGERED    8       //1000
#define I86_PIC_ICW1_LTIM_EDGETRIGGERED     0       //0000
#define I86_PIC_ICW1_INIT_YES               0x10    //10000
#define I86_PIC_ICW1_INIT_NO                0       //00000

//-----------------------------------------------
//    Initialization Command 4 control bits
//-----------------------------------------------

#define I86_PIC_ICW4_UPM_86MODE             1       //1
#define I86_PIC_ICW4_UPM_MCSMODE            0       //0
#define I86_PIC_ICW4_AEOI_AUTOEOI           2       //10
#define I86_PIC_ICW4_AEOI_NOAUTOEOI         0       //0
#define I86_PIC_ICW4_MS_BUFFERMASTER        4       //100
#define I86_PIC_ICW4_MS_BUFFERSLAVE         0       //0
#define I86_PIC_ICW4_BUF_MODEYES            8       //1000
#define I86_PIC_ICW4_BUF_MODENO             0       //0
#define I86_PIC_ICW4_SFNM_NESTEDMODE        0x10    //10000
#define I86_PIC_ICW4_SFNM_NOTNESTED         0       //a binary 2 (futurama joke hehe ;)


// send command to PICs
void i86_pic_send_command (uint8b cmd, uint8b pic)
{
    uint8b reg;
    if (pic!=1 && pic!=2)
        return;
    reg = (pic==1) ? I86_PIC1_REG_COMMAND : I86_PIC2_REG_COMMAND;
    outportb (reg, cmd);
}


// send data to PICs
void i86_pic_send_data (uint8b data, uint8b pic)
{
    uint8b reg;
    if (pic!=1 && pic!=2)
        return;
    reg = (pic==1) ? I86_PIC1_REG_DATA : I86_PIC2_REG_DATA;
    outportb (reg, data);
}


// read data from PICs
uint8b i86_pic_read_data (uint8b pic)
{
    uint8b reg;
    if (pic!=1 && pic!=2)
        return 0;
    reg = (pic==1) ? I86_PIC1_REG_DATA : I86_PIC2_REG_DATA;
    return inportb (reg);
}


void i86_pic_initialise (uint8b base1, uint8b base2)
{
    uint8b icw=0;
    disable_ints(); // disable hardware interrupts

    // Begin initialization of PIC
    icw = (icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;
    icw = (icw & ~I86_PIC_ICW1_MASK_IC4) | I86_PIC_ICW1_IC4_EXPECT;

    i86_pic_send_command (icw, 1);
    i86_pic_send_command (icw, 2);

    // Send initialization control word 2. This is the base addresses of the irq's
    i86_pic_send_data (base1, 1);
    i86_pic_send_data (base2, 2);

    // Send initialization control word 3. This is the connection between master and slave.
    // ICW3 for master PIC is the IR that connects to secondary pic in binary format
    // ICW3 for secondary PIC is the IR that connects to master pic in decimal format

    i86_pic_send_data (0x04, 1);
    i86_pic_send_data (0x02, 2);

    // Send Initialization control word 4. Enables i86 mode

    icw = (icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;
    i86_pic_send_data (icw, 1);
    i86_pic_send_data (icw, 2);
} 


// enable all interrupt requests
void i86_enable_irqs()
{
    i86_pic_send_data(0x00, 1);
    i86_pic_send_data(0x00, 2);
}

// disable all interrupt requests
void i86_disable_irqs()
{
    i86_pic_send_data(0xFF, 1);
    i86_pic_send_data(0xFF, 2);
}


// enable specific interrupt request
void i86_enable_irq(uint8b irq)
{
    if(irq<8)   // if is master
        i86_pic_send_data( i86_pic_read_data(1) & ~(1<<irq), 1);
    else
    {   irq-=8; // else is slave
        i86_pic_send_data( i86_pic_read_data(2) & ~(1<<irq), 2);
    }
}

// disable specific interrupt request
void i86_disable_irq(uint8b irq)
{
    if(irq<8)   // if is master
        i86_pic_send_data( i86_pic_read_data(1) | (1<<irq), 1);
    else
    {   irq-=8; // else is slave
        i86_pic_send_data( i86_pic_read_data(2) | (1<<irq), 2);
    }
}
