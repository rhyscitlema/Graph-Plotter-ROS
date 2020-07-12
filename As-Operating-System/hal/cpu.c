/****************************************************************************
    cpu.c
****************************************************************************/

#include "cpu.h"
#include "gdt.h"
#include "idt.h"

void i86_cpu_initialise ()      // initialises cpu resources
{
	// initialise processor tables
	i86_gdt_initialise ();
	i86_idt_initialise (0x8);
}

void i86_cpu_shutdown ()        // shutdown computer
{
    // Not available yet! Perform a restart!
    i86_cpu_restart ();
}

void i86_cpu_restart ()         // restart computer
{
    #ifdef _MSC_VER
	_asm {
        mov     al, 0xFE
        out     0x64, al
        jmp     $        ; If we get here then something really went wrong!
	}
    #endif
}

char* i86_cpu_get_vender ()     // returns vender name of cpu
{
	static char	vender[32] = {0};
    #ifdef _MSC_VER
	_asm {
		mov		eax, 0
		cpuid
		mov		dword ptr [vender], ebx
		mov		dword ptr [vender+4], edx
		mov		dword ptr [vender+8], ecx
	}
    #endif
	return vender;
}

// flush all internal and external processor caches
void i86_cpu_flush_caches ()
{
    #ifdef _MSC_VER
	_asm {
		cli
		invd
		sti
	}
    #endif
}

// same as above but writes the data back into memory first
void i86_cpu_flush_caches_write ()
{
    #ifdef _MSC_VER
	_asm {
		cli
		wbinvd
		sti
	}
    #endif
}

// flush translation lookaside buffer (TLB) entry
void i86_cpu_flush_tlb_entry (uint32b addr)
{
    #ifdef _MSC_VER
	_asm {
		cli
		invlpg	addr
		sti
	}
    #endif
}
