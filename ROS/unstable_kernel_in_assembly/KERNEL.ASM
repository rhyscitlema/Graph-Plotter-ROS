
;*********************************************
;    YEMELITCOS
;
;    KERNEL.ASM
;
;*********************************************

use32                           ; 32 bits protected mode

include "../general_info.inc"   ; contains no code (only MACROs)

org FINAL_KERNEL_LOCATION



;*******************************************************
;    KERNEL ENTRY POINT
;*******************************************************

start_kernel:
    mov     ax, 0x10                    ; refresh all segment registers
    mov     ds, ax                      ; to our data descriptor.
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     esp, FINAL_KERNEL_LOCATION  ; stack starts at FINAL_KERNEL_LOCATION in RAM

    call    os_initialisation           ; interrupts are restored in here

    jmp     start_commandline

hang:
    hlt
    jmp     hang


;---------------------
;   reboot computer
;---------------------
reboot:
    call    kbd_wait
    mov     al, 0xFE
    out     0x64, al
    jmp     hang         ; If we get here then something really went wrong!


delayABit:              ; wait a bit, may be for some hardware job
    push    ecx
    mov     ecx,0xFF
   .delay:
    loop    .delay
    pop     ecx
    ret


    include "include/driver/keyboard.inc"
    include "include/driver/timer.inc"              ; Programmable Interval Timer (PIT)
    include "include/driver/cmos.inc"               ; Get CMOS data (time, date, etc)
    include "include/driver/pic.inc"                ; Programmable Interrupt Controller (PIC)
    include "include/driver/pc_speaker.inc"
    include "include/driver/mouse.inc"
    include "include/driver/screen.inc"
    include "include/driver/dma.inc"                ; Direct Memory Access (DMA)
    include "include/driver/fat12_fdd.inc"          ; FAT 12 file system for Floppy Disk Drive
;   include "include/driver/ide.inc"

    include "include/system/os_initialisation.inc"  ; Routines to initialise YEMELITCOS
    include "include/system/commandline.inc"
    include "include/system/commands.inc"           ; terminal/shell/console commands
    include "include/system/idt.inc"                ; Interrupt Description Table (IDT)
    include "include/system/isr.inc"                ; Interrupt Service Routines (ISR)
    include "include/system/vbemodeinfo.inc"
    include "include/system/textmode.inc"           ; textmode routines
    include "include/system/programs.inc"           ; search, load and execute program files

    include "include/library/stdio.inc"
    include "include/library/string.inc"


KERNEL_TEMP_BUFFER:    ; kernel file ends here, and start of kernel temporary buffer

