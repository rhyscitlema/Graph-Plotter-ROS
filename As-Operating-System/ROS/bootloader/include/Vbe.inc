;*********************************************
;    YEMELITC-OS
;
;    Vbe.inc (VESA BIOS EXTENSION)
;
;*********************************************



;-------------------------------
; Set video mode
;-------------------------------

SetVideoMode:
    pusha


getVBEInfo:
    mov     ax, 0x4F00
    mov     di, vbeControllerInfo
    int     0x10
    cmp     ax, 0x004F              ; if operation was successfull
    je      printVbeModesList
    mov     si, msgVESAFail
    call    printstr
    popa
    jmp     reboot


printVbeModesList:
    mov     si, msgPrintVbeModesList
    call    printstr

    mov     si, [videoModesPtr]     ; was gotten from previous int call
    mov     cx, 30                  ; max number of searches

  .loop:
    lodsw
    cmp     ax, 0xFFFF
    je      selectVbeMode           ; if at end of list of vbe modes
    dec     cx
    jz      .vbeModesListInvalid    ; if 20 searches were made
    call    printAX                 ; print the 16-bits value to screen
    jmp     .loop

  .vbeModesListInvalid:
    mov     si, msgVbeModesListInvalid
    call    printstr


selectVbeMode:
    push    es
    xor     ax, ax
    mov     es, ax                  ; clear es

  .loop:
    mov     si, msgSelectVbeMode
    call    printstr

    mov     di, userVbeMode
    cmp byte [quickLaunch] , 0      ; if user decided a quickLaunch then
    jne     .continue               ; use default and skip asking questions!
    mov     cx, 4                   ; no more than 4 characters
    call    getUserInput

;   mov     si, strQuit             ; check if user entered "quit"
;   call    strcmp
;   jnc     .continue
;   pop     es
;   popa
;   jmp     reboot

  .continue:
    mov     si, userVbeMode
    call    strtohex                ; get hex number from string
    mov     cx, ax                  ; store mode value in CX

    mov     di, vbeModeInfo         ; see general_info.inc for 'vbeModeInfo'
    mov     ax, 0x4F01
    int     0x10                    ; get vbe mode info

    cmp     ax, 0x004F
    jne     .fail
    pop     es
    jmp     nowSetVbeMode

  .fail:
    mov     si, msgVbeModeFail
    call    printstr
    jmp     .loop                   ; try again


nowSetVbeMode:
    push    ds
    xor     ax, ax
    mov     ds, ax                  ; data segment = 0
    mov     ax, [version]           ; save the VESA version
    mov     [VBE_VERSION], ax
    pop     ds

    mov     ax, 0x4F02              ; function for setting vbe mode
    mov     bx, cx                  ; get video mode from CX
    or      bx, 0x4000              ; bit 14 set to 1 so to use linear frame buffer
    int     0x10
    cmp     ax, 0x004F              ; check if successful
    je      .done

    mov     si, msgModeSetFail      ; else print error message and quit
    call    printstr
    popa
    jmp     reboot
  .done:
    popa
    ret


;-------------------------------
; Constants and variables
;-------------------------------

msgPrintVbeModesList    db 13, 10, "The following video modes were found:", 13, 10, 0
msgVbeModesListInvalid  db 13, 10, "WARNING: The list above seems wrong.", 13, 10, 0

msgSelectVbeMode    db 13, 10, 13, 10, "Select a video mode. Typical modes are:"
                    db 13, 10, "0112 for 640x480x24 , 0115 for 800x600x24 , 0118 for 1024x768x24"
                    db 13, 10, ": ", 0

msgVESAFail         db 13, 10, "ERROR: Computer seems not to support VESA VBE.", 13, 10, 0
msgVbeModeFail      db 13, 10, "ERROR: Computer does not support the selected video mode.", 13, 10, 0
msgModeSetFail      db 13, 10, "ERROR: Setting video mode has failed.", 13, 10, 0

VBEMODE             dw 0
userVbeMode         db "0115", 0   ; buffer to store the vbe mode number from user input

vbeControllerInfo:
    signature       dd 0        ; == "VESA"
    version         dw 0        ; == 0x0300 for VBE 3.0
    oemString       dd 0        ; isa vbeFarPtr
    capabilities    dd 0
    videoModesPtr   dd 0        ; isa vbeFarPtr
    totalMemory     dw 0        ; as number of 64KB blocks
    .buffer: times 1024 db 0

