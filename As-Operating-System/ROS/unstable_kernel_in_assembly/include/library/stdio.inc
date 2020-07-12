
;----------------------------------------------------------
;   print a string, of 1-byte characters, to screen
;    in: esi = address of string
;----------------------------------------------------------
printstr:
    push    ax
    push    esi
   .loop:
    lodsb
    or    al, al                ; check if character==0
    jz    .done
    call    printchr            ; this function uses a call inccursor,
    jmp    .loop                ; which is the main guide in printing the string.
   .done:
    pop    esi
    pop    ax
    ret


;------------------------------------------
;   print 1-byte 8-bits hex value
; in:    al = value
;------------------------------------------
printhex1:
    push    ax
    push    cx
    push    esi
    mov     cl, 1       ; number of bytes
    call    gethex
    mov     esi, gethex.hexstr
    call    printstr
    mov     al, ' '
    call    printchr
    pop     esi
    pop     cx
    pop     ax
    ret


;------------------------------------------
;   print 2-bytes 16-bits hex value
; in: ax = value
;------------------------------------------
printhex2:
    push    ax
    push    cx
    push    esi
    mov     cl, 2       ; number of bytes
    call    gethex
    mov     esi, gethex.hexstr
    call    printstr
    mov     al, ' '
    call    printchr
    pop     esi
    pop     cx
    pop     ax
    ret


;------------------------------------------
;   print 4-bytes 32-bits hex value
;    in: eax = value
;------------------------------------------
printhex4:
    push    ax
    push    cx
    push    esi
    mov     cl, 4       ; number of bytes
    call    gethex
    mov     esi, gethex.hexstr
    call    printstr
    mov     al, ' '
    call    printchr
    pop     esi
    pop     cx
    pop     ax
    ret

