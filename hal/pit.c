/****************************************************************************
    pit.c
    8253 Programmable Interval Timer
****************************************************************************/

#include "pit.h"
#include "hal.h"


//-----------------------------------------------
//    Controller Registers
//-----------------------------------------------

#define I86_PIT_REG_COUNTER0    0x40
#define I86_PIT_REG_COUNTER1    0x41
#define I86_PIT_REG_COUNTER2    0x42
#define I86_PIT_REG_COMMAND     0x43


// Global Tick count
static volatile uint32b _pit_ticks=0;

// Test if pit is initialised
static bool _pit_bIsInit=false;

// pit timer interrupt handler
static void i86_pit_i_handler ()
{
    ASM_START_I_HANDLER
    _pit_ticks++;           // increment tick count
    interruptdone(0);       // tell hal we are done
    ASM_STOP_I_HANDLER
}


// Set new pit tick count and return previous value
uint32b i86_pit_set_tick_count (uint32b i)
{
    uint32b ret = _pit_ticks;
    _pit_ticks = i;
    return ret;
}


// return current tick count
uint32b i86_pit_get_tick_count ()
{
    return _pit_ticks;
}


// send command to pit
void i86_pit_send_command (uint8b cmd)
{
    outportb (I86_PIT_REG_COMMAND, cmd);
}


// send data to a counter
void i86_pit_send_data (uint16b data, uint8b counter)
{
    uint8b port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
 ((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

    outportb (port, (uint8b)data);
}


// read data from counter
uint8b i86_pit_read_data (uint16b counter)
{
    uint8b port= (counter==I86_PIT_OCW_COUNTER_0) ? I86_PIT_REG_COUNTER0 :
 ((counter==I86_PIT_OCW_COUNTER_1) ? I86_PIT_REG_COUNTER1 : I86_PIT_REG_COUNTER2);

    return inportb (port);
}


// starts a counter
void i86_pit_start_counter (uint32b freq, uint8b counter, uint8b mode)
{
    uint8b ocw;
    uint16b divisor;

    if (freq==0) return;

    divisor = 1193181 / (uint16b)freq;

    // send operational command
    ocw=0;
    ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
    ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
    ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
    i86_pit_send_command (ocw);

    // set frequency rate
    i86_pit_send_data (divisor & 0xff, 0);
    i86_pit_send_data ((divisor >> 8) & 0xff, 0);

    // reset tick count
    _pit_ticks=0;
}


// initialise minidriver
void i86_pit_initialise ()
{
    // Install our interrupt handler (irq 0 uses interrupt 32)
    install_i_handler (32, i86_pit_i_handler);

    // we are initialised
    _pit_bIsInit = true;

    enable_irq(0);  // enable timer IRQ = 0
}


// test if pit interface is initialised
bool i86_pit_is_initialised ()
{
    return _pit_bIsInit;
}
