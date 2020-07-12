;*********************************************
;	YEMELITC-OS
;
;	stdio.inc
;
;*********************************************


;------------------------
; print a new line
;------------------------
printnewline:
    push    ax
    mov     ax, 0E0Dh
    int     10h
    mov     ax, 0E0Ah
    int     10h
    pop     ax
    ret


;----------------------------------------------------------
;   print a string, of 1-byte characters, to screen
;	input: esi = address of string
;----------------------------------------------------------

printstr:
	push	si
	push	ax
   .loop:
	lodsb				; load next byte from string from si to al
	or	al, al			; check if al = null terminator?
	jz	.done			; if yes, then we are done
	mov	ah, 0eh			; else, print the character
	int	10h
	jmp	.loop			; repeat until null terminator is found
   .done:
	pop	ax
	pop	si
	ret


;----------------------------------------------------------
;   print the content of the AX register
;	input: AX = hex number to print
;----------------------------------------------------------

printAX:
	pusha

	mov	di, .value_str_16
	mov	si, .digitsList
	mov	cx, 4			;four places

.loop:
	rol	ax, 4
	mov	bx, ax
	rol	bx, 4
	mov	bh, 0
	ror	bx, 4
	mov	bh, 0

	mov	bl, [si + bx]		        ;index into digitsList
	mov	[di], bl
	inc	di
	loop	.loop

	mov	si, .value_str_16
	call	printstr

	popa
	ret

.value_str_16	db '0000 ', 0		;value value string

.digitsList	db "0123456789ABCDEF"



;----------------------------------------------------------
;   Get user keyboard input as a string
;	in: DI = destination to store user input
;   in: CX = size of destination
;----------------------------------------------------------

getUserInput:
    push    ax
    push    cx
    push    si
    push    di
    mov     si, di          ; save start of destination address

  .loop:
    mov     ah, 00h         ; get user input character
    int     16h
   ;call    printAX

  .enter_key:
    cmp     al, 13          ; if 'Enter' key pressed
    jne     .backspace
    mov     ah, 0Eh         ; print a newline
    int     10h
    mov     ah, 0Eh
    mov     al, 10
    int     10h
    jmp     .done

  .backspace:
    cmp     al, 8           ; if 'Backspace' key pressed
    jne     .check
    cmp     si, di          ; check if at start of destination address
    je      .loop
    dec     di              ; else remove last character
    inc     cx
    mov     ah, 0Eh
    int     10h             ; move one character back
    mov     al, 0
    int     10h             ; clear character
    mov     al, 8
    int     10h             ; move one character back again
    jmp     .loop

  .check:
    cmp     al, ' '
    je     .continue
    cmp     al, '_'
    je      .continue
    cmp     al, '0'         ; check if character is valid
    jl      .loop
    cmp     al, '9'
    jle     .continue
    cmp     al, 'A'
    jl      .loop
    cmp     al, 'Z'
    jle     .continue
    cmp     al, 'a'
    jl      .loop
    cmp     al, 'z'
    jle     .continue
    jmp     .loop

  .continue:
    cmp     cx, 0           ; if destination address is full
    je      .loop           ; then do not add the new user input
	dec     cx
	mov     [di], al        ; store character to destination
	inc     di              ; independent of direction of 'stosb'
	mov	    ah, 0Eh
	int	    10h   		    ; print the character
    jmp     .loop

  .done:
    mov byte [di], 0        ; null termination
    pop     di
    pop     si
    pop     cx
    pop     ax
    ret





;************************************************
;   string compare
;
; in:   si = start of primary string
;       di = start of secondary string
; out:  stc = equal
;       clc = not equal
;      .di = end value of edi
;************************************************

strcmp:
    push    ax
    push    si
    push    di
  .loop:
    lodsb
    cmp     al, [di]
    jne     .not_equal
    cmp     al, 0           ; if end of si string
    je      .equal
    inc     di
    jmp     .loop
  .not_equal:
    clc
    jmp     .done
  .equal:
    stc
  .done:
    mov     [.di], di
    pop     di
    pop     si
    pop     ax
    ret
  .di dd 0                  ; end value of di







;------------------------------------------------------------------
;   string to hexadecimal number
; in:   si = string form of hex number
; out:  ax = hex number
;-------------------------------------------------------------------

strtohex:
    push    si
    push    bx
    xor     ax, ax
    xor     bx, bx

  .loop:
    lodsb
    cmp     ax, 0
    je      .done

    cmp     ax, '0'
    jb      .error
    cmp     ax, '9'
    ja      .ABCDEF
    sub     ax, '0'
    shl     bx, 4
    add     bx, ax
    jmp     .loop

  .ABCDEF:
    cmp     ax, 'A'
    jb      .error
    cmp     ax, 'F'
    ja      .abcdef
    sub     ax, 'A'
    add     ax, 10
    shl     bx, 4
    add     bx, ax
    jmp     .loop

  .abcdef:
    cmp     ax, 'a'
    jb      .error
    cmp     ax, 'f'
    ja      .error
    sub     ax, 'a'
    add     ax, 10
    shl     bx, 4
    add     bx, ax
    jmp     .loop

  .done:
    mov     ax, bx
    pop     bx
    pop     si
    stc
    ret

  .error:
    pop     bx
    pop     si
    clc
    ret


