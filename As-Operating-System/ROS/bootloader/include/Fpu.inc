;*********************************************
;	YEMELITC-OS
;
;	Fpu.inc
;
;*********************************************


;-----------------------------------
;   Enable the Floating Point Unit
;-----------------------------------

EnableFPU:
    pusha

    mov     eax, CR0                    ; load control word
    and     eax, 0xFFFFFFF3             ; clear TS and EM to force fpu access
    mov     CR0, eax                    ; store control word

    FNINIT                              ; load defaults to FPU
    FNSTSW  [.testword]                 ; store status word
    CMP     word [.testword], 0         ; compare the written status with the expected FPU state
    je      .hasfpu                     ; jump if the FPU has written something (i.e. it is there)

    popa                                ; FPU not found
    mov     si, .noFPU_message          ; prepare to leave
    call    printstr
    jmp     reboot

  .hasfpu:
    mov     eax, CR0                    ; load control word

    or      eax, 0x00000222             ; set bits 1, 5 and 9 (set CR0.MP, CR0.NE and CR0.OSFXSR)
    and     eax, 0xFFFFFBDB             ; clear bits 2 and 10 (set CR0.EM and CR0.OSXMMEXCPT)

   ;or      eax, 0x00000622             ; set bits 1, 5 and 9 (set CR0.MP, CR0.NE and CR0.OSFXSR)
   ;and     eax, 0xFFFFFFDB             ; clear bits 2 and 10 (set CR0.EM and CR0.OSXMMEXCPT)

    mov     CR0, eax                    ; store control word
    FNINIT                              ; load default settings to FPU

    popa
    ret

  .testword: dw 0x55AA
  .noFPU_message    db 13, 10, 13, 10, "ERROR: No Floating Point Unit (FPU)", 13, 10, 0

