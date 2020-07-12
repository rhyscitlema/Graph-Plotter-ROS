;*********************************************
;
;    YEMELITC OPERATING SYSTEM
;
;    KRNLDR.ASM = kernelloader.asm
;
;*********************************************
; Content:
;    - Included files
;    - Function to reboot computer
;    - Kernel loader entry point
;    - Load extra files
;    - Load kernel file
;    - Check kernel file
;    - Enter protected mode
;    - Go into 32 bits code
;    - Move kernel
;    - Execute kernel
;    - Constants and variables
;*********************************************


use16                               ; 16 bits real mode

org 0                               ; segment regisers will be set later to KRNLDR_LOCATION

jmp start_kernelloader


    %include "./general_info.inc"   ; contains no code (only macros)
    %include "include/stdio.inc"    ; input/output
    %include "include/Gdt.inc"      ; Gdt routines
    %include "include/Idt.inc"      ; Idt routines
    %include "include/A20.inc"      ; A20 enabling
    %include "include/Fat12.inc"    ; FAT12 driver
    %include "include/Vbe.inc"      ; VESA VBE enabling
    %include "include/Fpu.inc"      ; FPU enabling
    TEMP_MEMORY equ 0xA000          ; assuming size of KRNLDR.SYS < 0xA000 bytes



;*********************************************
; Reboot Computer
;*********************************************

reboot:
    mov     si, msgReboot
    call    printstr
    mov     ah, 0x00
    int     0x16                    ; wait for key press

    mov     al, 0xFE
    out     0x64, al                ; reboot computer

    cli                             ; If we get here then something really went wrong!
    hlt
    jmp $



;*******************************************************
;    KERNEL LOADER ENTRY POINT
;*******************************************************

start_kernelloader:

    ;-------------------------------
    ;   Setup segments
    ;-------------------------------

    cli                                 ; disable interrupts
    mov    eax, KRNLDR_LOCATION
    shr    eax, 4
    mov    ds, ax                       ; setup segement registers
    mov    es, ax                       ; to point to begining
    mov    fs, ax                       ; of our bootloader
    mov    gs, ax

    ;-------------------------------
    ; Setup stack
    ;-------------------------------
    xor    ax, ax
    mov    ss, ax
    mov    sp, KRNLDR_LOCATION          ; start of stack pointer

    sti                                 ; restore interrupts



;----------------------------------------
; Check if user wants a quick launch
; That is without being asked questions
;----------------------------------------

QuickLaunch:
    mov     si, msgQuickLaunch
    call    printstr
    int     0x16                        ; wait for key press
    cmp     al, 'n'
    je      .done                       ; no quick launch by default
    cmp     al, 'N'
    je      .done
    mov byte [quickLaunch], 1           ; quick lauch enabled
  .done:
    call    printnewline



;-------------------------------
; Load Extra File(s)
;-------------------------------

;LoadExtraFile1:
;    mov    ebx, EXTRA_FILE              ; location to load file to
;    shr    ebx, 4                       ; make this into a segment
;    mov    si, nameOfExtraFile1         ; ExtraFile name
;    call   LoadFile                     ; don't care if file is not successfully loaded



;-------------------------------
; Load Kernel
;-------------------------------

GetKernelFileName:
    mov     si, msgSearchKernel
    call    printstr

    mov     di, KernelName
    cmp byte [quickLaunch] , 0          ; if user decided a quickLaunch then
    jne     LoadKernel                  ; use default and skip asking questions!
    mov     cx, 11                      ; no more than 11 characters
    call    getUserInput

    mov     si, strQuit                 ; check if user entered "quit"
    call    strcmp
    jc      reboot

LoadKernel:
    mov     ebx, FIRST_KERNEL_LOCATION  ; location to load kernel to
    shr     ebx, 4                      ; make this into a segment
    mov     si, KernelName              ; kernel file name
    call    LoadFile                    ; Fat12.inc
    jc      .success
  .fail:
    mov     si, msgMissingKernel
    call    printstr                    ; print error message
    jmp     GetKernelFileName           ; and restart
  .success:
    mov     dword [KernelSize], ecx     ; save ecx=size of kernel


CheckKernel:        ; Insure kernel is a PE program (test for signiture)
    push    es
    mov     ebx, FIRST_KERNEL_LOCATION
    shr     ebx, 4                      ; make it into a segment
    mov     es, bx
    mov     bx, [es:60]                 ; es:bx points to file signature

    cmp     word [es:bx], 'PE'          ; check if signature is PE00
    jne     .curruptKernel
    cmp     word [es:bx+2], 00
    je      .success                    ; if signature is PE00

  .curruptKernel:
    pop     es
    mov     esi, msgCurruptKernel
    call    printstr                    ; print error message
    jmp     GetKernelFileName           ; and restart
  .success:
    pop     es
    mov     si, msgLoadedKernel
    call    printstr



;-----------------------------------
;   Go into protected mode!
;-----------------------------------

EnterPMode:

    cld                                     ; ensure direction is clear (does that mean going upward or downward?)

    call    EnableFPU                       ; Fpu.inc

    call    EnableA20_KKbrd_Out             ; A20.inc

    call    SetVideoMode                    ; vbe.inc

    call    LoadGDT                         ; Gdt.inc
   ;call    LoadIDT                         ; Idt.inc

    cli                                     ; clear interrupts, do not re-enable

    mov     eax, CR0                        ; setting bit 0 in CR0 => entering protected mode!
    or      eax, 1
    mov     cr0, eax


use32                                       ; start using 32-bits codes

    push    byte KERNEL_CODE_DESC           ; do far jump to fix CS
    push dword MoveKernel+KRNLDR_LOCATION   ; Remember that KERNEL_CODE_DESC = 0x08
    retf



;******************************************************
;    OUF! WE ARE IN PROTECTED MODE !
;******************************************************

MoveKernel:

    mov    ax, KERNEL_DATA_DESC             ; set data segments to data selector (0x10)
    mov    es, ax
    mov    ds, ax

    mov    eax, dword [KernelSize+KRNLDR_LOCATION]         ; add because segmentation was done
    movzx  ebx, word [bpbBytesPerSector+KRNLDR_LOCATION]
    mul    ebx                              ; kernel size was the number of sectors
    shr    eax, 2                           ; now prepare to write 4 bytes at a time
    mov    ecx, eax

    mov    esi, FIRST_KERNEL_LOCATION
    mov    edi, FINAL_KERNEL_LOCATION
    rep    movsd                            ; move kernel to its protected mode address

ExecuteKernel:
    ;-------------------------------------------------------------------------
    ; parse the kernel's header info structures to get its entry point
    ;-------------------------------------------------------------------------
    mov    ebx, [FINAL_KERNEL_LOCATION+60]
    add    ebx, FINAL_KERNEL_LOCATION       ; ebx now points to file signature (PE00)
    add    ebx, 24
    mov    eax, [ebx]                       ; _IMAGE_FILE_HEADER is 20 bytes + size of sig (4 bytes)
    add    ebx, 20-4                        ; address of entry point
    mov    edi, [ebx]                       ; get entry point offset in code section    
    add    ebx, 12                          ; image base is offset 8 bytes from entry point
    mov    eax, [ebx]                       ; add image base
    add    edi, eax

    push    word KERNEL_CODE_DESC           ; Final preparations
    push    edi
    retf                                    ; Finally, Execute Kernel

    cli                                     ; we should normally never get here
    hlt



;*********************************************
; Constants and Variables
;*********************************************

msgReboot           db 13, 10, "Press Any Key to Reboot", 13, 10, 0

KernelSize          dd 0                ; size of kernel file in bytes
KernelName          db "KERNEL1 EXE", 0, "buffer to store the kernel file name"

msgSearchKernel     db 13, 10, "Enter Kernel file name (Enter 'quit' to quit)"
                    db 13, 10, "(Default kernel is 'KERNEL1 EXE')"
                    db 13, 10, ": ", 0

msgMissingKernel    db 13, 10, "ERROR: File not found.", 13, 10, 0
msgCurruptKernel    db 13, 10, "ERROR: Kernel file corrupted.", 13, 10, 0

strQuit             db "quit", 0

msgLoadedKernel     db 13, 10, "Operating System Loaded Successfully!", 13, 10, 13, 10, 0

msgQuickLaunch      db 13, 10, 13, 10, 13, 10, "Quick launch? (y/n) : ", 0
quickLaunch         db 0

