#ifndef _CPU_H_INCLUDED
#define _CPU_H_INCLUDED
/****************************************************************************
    cpu.h
****************************************************************************/


#include "regs.h"


extern void i86_cpu_initialise ();  // initialise the processors

extern void i86_cpu_shutdown ();    // shutdown computer

extern void i86_cpu_restart ();     // restart computer

extern char* i86_cpu_get_vender (); // get processor vender

// flush all internal and external processor caches
extern void i86_cpu_flush_caches ();

// same as above but writes the data back into memory first
extern void i86_cpu_flush_caches_write ();

// flush translation lookaside buffer (TLB) entry
extern void i86_cpu_flush_tlb_entry (uint32b addr);

#endif
