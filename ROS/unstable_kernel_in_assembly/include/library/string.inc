
digitsList    db '0123456789ABCDEF'

scanner dd 0        ; address for get/has..._next...word/line/int...

;************************************************
;   string length
; in:   esi = input string
; out:  eax = length of string
;************************************************

strlen:
    push    esi
  .loop:
    lodsb
    cmp     al, 0
    jne    .loop
  .done:
    mov     eax, esi
    pop     esi
    sub     eax, esi
    dec     eax
    ret


;************************************************
;   string copy
; in:   esi = input string
;       edi = output string
; out:  .edi = end value of edi
;************************************************

strcpy:
    push    eax
    push    esi
    push    edi
  .loop:
    lodsb
    stosb
    cmp     al, 0
    jne    .loop
  .done:
    mov     [.edi], edi
    pop     edi
    pop     esi
    pop     eax
    ret
  .edi dd 0                 ; end value of edi


;************************************************
;   string compare
;
; in:   esi = start of primary string
;       edi = start of secondary string
; out:  stc = equal
;       clc = not equal
;      .edi = end value of edi
;************************************************

strcmp:
    push    eax
    push    esi
    push    edi
  .loop:
    lodsb
    cmp     al, [edi]
    jne    .not_equal
    cmp     al, 0            ; if end of esi string
    je     .equal
    inc     edi
    jmp    .loop
  .not_equal:
    clc
    jmp    .done
  .equal:
    stc
  .done:
    mov     [.edi], edi
    pop     edi
    pop     esi
    pop     eax
    ret
  .edi dd 0                 ; end value of edi


;************************************************
;   trim string everywhere
;   Render group of spaces to 1 space
; in:   esi = input string
; out:  esi = modified version
;************************************************

trim_all:
    push    ax
    push    esi
    push    edi
    mov     edi, esi
    mov     ah, 1           ; assume space found before the beginning
  .loop:
    lodsb
    cmp     al, 0
    je     .done
    cmp     al, ' '
    je    .isSpace
    stosb
    mov     ah, 0
    jmp    .loop
  .isSpace:
    cmp     ah, 1           ; if previous character was a space,
    je     .loop           ; then avoid the duplicate
    stosb                   ; else store the space
    mov     ah, 1           ; and note this information
    jmp    .loop
  .done:
    stosb                   ; store null character
    pop     edi
    pop     esi
    pop     ax
    ret


;************************************************
;   has next word ?
;   delimiters are ' ', '\n', '\t'
;
; in:   scanner = input string
; out:  stc = yes
;       clc = no
;************************************************

has_next_word:
    push    ax
    push    esi
    mov     esi, [scanner]
  .loop:
    lodsb
    cmp     al, ' '         ; if space
    je     .loop
    cmp     al, 9           ; if horizontal tab
    je     .loop
    cmp     al, 11          ; if vertical tab
    je     .loop
    cmp     al, 12          ; if form feed
    je     .loop
    cmp     al, 10          ; if line feed
    je     .loop
    cmp     al, 13          ; if newline
    je     .loop
    cmp     al, 0
    jne     .has
    clc
    jmp    .done
  .has:
    stc
  .done:
    pop     esi
    pop     ax
    ret


;************************************************
;   get next word
;   delimiters are ' ', '\n', '\t'
;
; in:   scanner = input string
;       edi     = output string
; out:  stc     = word found
;       clc     = no word
;       scanner = end of extracted string
;************************************************

get_next_word:
    push    ax
    push    esi
    push    edi
    mov     esi, [scanner]
  .loop1:
    lodsb
    cmp     al, ' '         ; if space
    je     .loop1
    cmp     al, 9           ; if horizontal tab
    je     .loop1
    cmp     al, 11          ; if vertical tab
    je     .loop1
    cmp     al, 12          ; if form feed
    je     .loop1
    cmp     al, 10          ; if line feed
    je     .loop1
    cmp     al, 13          ; if newline
    je     .loop1
    cmp     al, 0
    jne     .loop2
    clc
    jmp    .done
  .loop2:
    stosb
    lodsb
    cmp     al, ' '         ; if space
    je     .end
    cmp     al, 9           ; if horizontal tab
    je     .end
    cmp     al, 11          ; if vertical tab
    je     .end
    cmp     al, 12          ; if form feed
    je     .end
    cmp     al, 10          ; if line feed
    je     .end
    cmp     al, 13          ; if newline
    je     .end
    cmp     al, 0
    jne    .loop2
  .end:
    mov     byte [edi], 0   ; null-terminated string
    mov     [scanner], esi  ; update scanner
    stc
  .done:
    pop     edi
    pop     esi
    pop     ax
    ret



;************************************************
;   goto next word
;   delimiters are ' ', '\n', '\t'
; in:   esi = input string
; out:  esi = begining of next word
;************************************************

goto_next_word:
    push    ax
  .loop1:
    lodsb
    cmp     al, ' '         ; if space
    je     .loop2
    cmp     al, 9           ; if horizontal tab
    je     .loop2
    cmp     al, 11          ; if vertical tab
    je     .loop2
    cmp     al, 12          ; if form feed
    je     .loop2
    cmp     al, 10          ; if line feed
    je     .loop2
    cmp     al, 13          ; if newline
    je     .loop2
    cmp     al, 0
    je     .done
    jmp    .loop1
  .loop2:
    lodsb
    cmp     al, ' '         ; if space
    je     .loop2
    cmp     al, 9           ; if horizontal tab
    je     .loop2
    cmp     al, 11          ; if vertical tab
    je     .loop2
    cmp     al, 12          ; if form feed
    je     .loop2
    cmp     al, 10          ; if line feed
    je     .loop2
    cmp     al, 13          ; if newline
    je     .loop2
    cmp     al, 0
    je     .done
  .end:
    dec     esi
  .done:
    pop     ax
    ret


;************************************************
;   goto next line
;   delimiter is '\n'
; in:   esi = input string
; out:  esi = begining of next line
;************************************************

goto_next_line:
    push    ax
  .loop1:
    lodsb
    cmp     al, 12          ; if form feed
    je     .loop2
    cmp     al, 10          ; if line feed
    je     .loop2
    cmp     al, 13          ; if newline
    je     .loop2
    cmp     al, 0
    je     .done
    jmp    .loop1
  .loop2:
    lodsb
    cmp     al, 12          ; if form feed
    je     .loop2
    cmp     al, 10          ; if line feed
    je     .loop2
    cmp     al, 13          ; if newline
    je     .loop2
    cmp     al, 0
    je     .done
  .end:
    dec     esi
  .done:
    pop     ax
    ret


;------------------------------------------------------------------
;   get the next integer after the scanner position
; in:   scanner = position to start
; out:  eax = integer
;       scanner = end of extracted integer
;-------------------------------------------------------------------

get_next_int:
    push    esi
    push    edi
    mov     edi, .str_of_int
    call    get_next_word
    jnc    .done
    mov     esi, .str_of_int
    call    strtoint          ; gives success notification with carry flag
  .done:
    pop     edi
    pop     esi
    ret
  .str_of_int:  times 11  db 0



;------------------------------------------------------------------
;   string to integer
; in:   esi = string form of integer
; out:  eax = integer
;-------------------------------------------------------------------

strtoint:
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi

    call    strlen            ; eax = string length
    add     esi, eax
    mov     ecx, eax
    mov     ebx, 1
    mov     edi, 0
  .loop:
    dec     esi
    mov     al, [esi]
    cmp     al, '0'
    jb      .error
    cmp     al, '9'
    ja      .error
    and     eax, 0x000000FF
    sub     al, '0'
    mul     ebx
    add     edi, eax
    mov     eax, ebx
    mov     ebx, 10
    mul     ebx
    mov     ebx, eax
    loop    .loop
    mov     eax, edi
    stc
  .done:
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    ret

  .error:
    clc
    jmp     .done


;-----------------------------------------------
;   get hex value as a string
; in:   eax = value
;       cl  = number of bytes to extract
; out:  .hexstr = string form of value
;-----------------------------------------------

gethex:
	pusha
	and     ecx, 0x000000FF
	shl     ecx, 1              ; multiply by 2
    mov     ebx, eax            ; ebx maintains the value
	mov     edi, .hexstr
	add     edi, ecx
    mov     byte [edi], 0       ; null termination
  .loop:
	dec     edi
    mov     esi, ebx            ; new bl required
    and     esi, 0x0000000F
    add     esi, digitsList
    lodsb
    mov     [edi], al
    ror     ebx, 4
    dec     ecx
    jnz     .loop
  .done:
    popa
    ret
  .hexstr db "FFFFFFFF", 0

