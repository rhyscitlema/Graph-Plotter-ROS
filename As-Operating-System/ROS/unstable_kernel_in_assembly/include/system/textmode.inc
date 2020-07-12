
;----------------------------------------------------
;
; Text mode screen functions.
;
;----------------------------------------------------

	include "include/system/char_font.inc"

	STN	equ	0xF4		; special textmode newline byte


;----------------------------------
;   variables
;----------------------------------

cursorPos:
	.X	db 0
	.Y	db 0
charsize:
	.X	db 0
	.Y	db 0
TextSize:
	.X	db 0
	.Y	db 0
TextColour:
	.foreground	dd 0
	.background	dd 0

initialiseTextMode:
	pusha
	mov	ax, [vbeModeInfo_XResolution]
	mov	cx, 8
	mov	[charsize.X], cl
	div	cx
	mov	[TextSize.X], al
	mov	ax, [vbeModeInfo_YResolution]
	mov	cx, 16
	mov	[charsize.Y], cl
	div	cx
	mov	[TextSize.Y], al
	mov dword [TextColour.foreground], 0x00FFFFFF
	mov dword [TextColour.background], 0x00000022
	popa
	ret



;---------------------------------------------
;   print a character to screen
;	in: al = character to print
;---------------------------------------------

printchr:
	pusha

	mov	ebx, [TEXT_MODE_SPACE-8]
	cmp	ebx, [TEXT_MODE_SPACE-12]
	jbe	.cont1

	mov	[TEXT_MODE_SPACE-12], ebx
	call	displayWholeTextMode

   .cont1:
	cmp	al, 13					; if newline character
	jne	.cont2
	call	newline
	jmp	.done
   .cont2:
	cmp	al, 10					; if newline character
	jne	.cont3
	call	newline
	jmp	.done

   .cont3:
	mov	edi, [TEXT_MODE_SPACE-4]
	cmp	al, 0
	jne	.cont3_else
	mov byte [edi], ' '				; store ' ' text_mode_space
	jmp	.cont4
   .cont3_else:
	mov byte [edi], al				; store character in text_mode_space

   .cont4:
	mov	esi, charFontsList			; get location of character font
	movzx	edx, [charsize.Y]
	and	eax, 0x000000FF
	mul	edx
	add	esi, eax				; esi = charFontsList + dl*(bytes per char)

	movzx	eax, [cursorPos.Y]
	movzx	ecx, [charsize.Y]
	mul	cx
	shl	eax, 16					; store the pixel Y_location in higher 16 bits of eax
	movzx	ax, [cursorPos.X]
	movzx	cx, [charsize.X]
	mul	cx					    ; eax low 16 bits = pixel X_location
	push	eax					; will be used when calling displayToScreen
	call	computeScreenOffset
	mov	edi, SCREEN_BUFFER
	add	edi, ebx

	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]
	mov	ebx, [TextColour.foreground]
	mov	edx, [TextColour.background]
   .is32BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 32
	jne	.is24BPP
	call	printchr_32BPP
	jmp	.cont5
   .is24BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 24
	jne	.cont5
	call	printchr_24BPP

   .cont5:
	pop	eax
	movzx	cx, [charsize.Y]
	shl	ecx, 16
	movzx	cx, [charsize.X]
	call	displayToScreen
	call	inccursor			; textscreen.inc

   .done:
	popa
	ret


printchr_32BPP:
	xor	eax, eax
	mov	cl, [charsize.Y]
	.outerLoop:
		push	ecx
		push	edi
		lodsb						; bits pattern for row pixels
		mov	ecx, 10000000b				; assuming charsize.X = 8 bits
	   .innerLoop:
		test	al, cl
		jz	.isBackGrd
	   .isForeGrd:
		mov	[edi], ebx
		jmp	.next
	   .isBackGrd:
		mov	[edi], edx
	   .next:
		add	edi, 4
		shr	cl, 1				; prepare to test next bit of al
		jnz	.innerLoop
	pop	edi
	add	edi, ebp
	pop	ecx
	dec	ecx
	jnz	.outerLoop
	ret

printchr_24BPP:
	xor	eax, eax
	mov	cl, [charsize.Y]
	.outerLoop:
		push	ecx
		push	edi
		lodsb						; bits pattern for row pixels
		mov	ecx, 10000000b				; assuming charsize.X = 8 bits
	   .innerLoop:
		test	al, cl
		jz	.isBackGrd
	   .isForeGrd:
		mov	[edi], bx
		ror	ebx, 8
		mov	[edi+2], bh
		rol	ebx, 8
		jmp	.next
	   .isBackGrd:
		mov	[edi], dx
		ror	edx, 8
		mov	[edi+2], dh
		rol	edx, 8
	   .next:
		add	edi, 3
		shr	cl, 1				; prepare to test next bit of al
		jnz	.innerLoop
	pop	edi
	add	edi, ebp
	pop	ecx
	dec	ecx
	jnz	.outerLoop
	ret



;----------------------------------
;   clear the screen
;----------------------------------

clrscr:
	push	eax
	push	ecx
	push	esi
	push	edi

	mov dword [TEXT_MODE_SPACE-4], TEXT_MODE_SPACE
	mov dword [TEXT_MODE_SPACE-8], TEXT_MODE_SPACE
	mov dword [TEXT_MODE_SPACE-12], TEXT_MODE_SPACE
	mov	ax, 0
	call	setcursor				; position cursor at far top-left
	call	displayWholeTextMode

	pop	edi
	pop	esi
	pop	ecx
	pop	eax
	ret


;---------------------------------------
;  backspace - delete last typed char
;---------------------------------------
backspace:				; note: I assume this will NEVER
	push	ax			; be on the very first cursor position!
	call	deccursor
	mov	al, 0			; replace with null character
	call	printchr
	call	deccursor		; cursor was just moved
	pop	ax
	ret


;-------------------------------
;  make a new line (CR, LF)
;-------------------------------
newline:				; quick-and-dirty algorithm!
	push	eax
   .GoDown:
	mov	eax, [TEXT_MODE_SPACE-4]
	cmp	eax, TEXT_MODE_SPACE+TEXT_MODE_SIZE
	jae	.done
	mov byte [eax], STN			; special_textmode_newline character
	inc	eax
	mov	[TEXT_MODE_SPACE-4], eax

	mov	al, [TextSize.Y]
	mov	ah, [cursorPos.Y]
	inc	ah
	cmp	ah, al
	jb	.now_go_down			; else, cursor is at or exceeds last position
	call	createNewCmdLine
	mov	ah, al				; note: ah might be > TextSize.Y!
	dec	ah
   .now_go_down:
	mov	al, 0				; set to beginning of row
	call	setcursor
   .done:
	pop	eax
	ret



;-----------------------------------------
;   set the cursor
;	in: al = x position
;	    ah = y position
;-----------------------------------------

setcursor:
	push	eax
	mov	[cursorPos], ax				; store position
	mov	eax, [TEXT_MODE_SPACE-4]
	mov dword [eax], 0				; end of text/everything character
	pop	eax
	ret


;----------------------------------
;  increment cursor position
;----------------------------------

inccursor:
	push	ax
	cmp dword [TEXT_MODE_SPACE-4], TEXT_MODE_SPACE+TEXT_MODE_SIZE
	jae	.done
	inc dword [TEXT_MODE_SPACE-4]
	mov	ax, [cursorPos]
   .GoRight:
	inc	al
	cmp	al, [TextSize.X]
	jae	.newline
	call	setcursor
	jmp	.done
   .newline:
	call	newline
   .done:
	pop	ax
	ret


;----------------------------------
;  decrement cursor position
;----------------------------------

deccursor:
	push	eax

	mov	eax, [TEXT_MODE_SPACE-4]
	cmp	eax, TEXT_MODE_SPACE
	jbe	.done
	dec	eax
	mov	[TEXT_MODE_SPACE-4], eax
	cmp byte [eax], STN
	jne	.cont
	dec dword [TEXT_MODE_SPACE-4]

   .cont:
	mov	ax, [cursorPos]
   .GoLeft:
	cmp	al, 0
	jbe	.GoUp
	dec	al
	call	setcursor
	jmp	.done
   .GoUp:
	cmp	ah, 0
	jbe	.done				; cursor is at very first position
	dec	ah
	mov	al, [TextSize.X]		; set to beginning of row
	dec	al
	call	setcursor
   .done:
	pop	eax
	ret



;-------------------------------------
;  scroll the screen up
;-------------------------------------

scroll_up:
	push	eax
	push	esi
	push	cx

	mov	esi, [TEXT_MODE_SPACE-12]
	cmp	esi, TEXT_MODE_SPACE
	jbe	.done

	dec	esi
	xor	eax, eax
	mov	cx, 3				; number of new lines to skip
   .loop:
	inc	eax
	dec	esi
	cmp	esi, TEXT_MODE_SPACE
	jbe	.tooLow
	cmp byte [esi], STN
	jne	.loop
	dec	cx
	jnz	.loop	
   .cont:
	sub	[TEXT_MODE_SPACE-12], eax
	call	displayWholeTextMode
	jmp	.done
   .tooLow:
	mov dword [TEXT_MODE_SPACE-12], TEXT_MODE_SPACE
	call	displayWholeTextMode
   .done:
	pop	cx
	pop	esi
	pop	eax
	ret


;-------------------------------------
;  scroll the screen down
;-------------------------------------

scroll_down:
	push	eax
	push	esi
	push	edi
	push	cx

	mov	esi, [TEXT_MODE_SPACE-12]
	mov	edi, [TEXT_MODE_SPACE-8]
	cmp 	esi, edi
	jae	.done

	dec	esi
	xor	eax, eax
	mov	cx, 3
   .loop:
	inc	eax
	inc	esi
	cmp	esi, edi
	jae	.tooHigh
	cmp byte [esi], STN
	jne	.loop
	dec	cx
	jnz	.loop
   .cont:
	add	[TEXT_MODE_SPACE-12], eax
	call	displayWholeTextMode
	jmp	.done
   .tooHigh:
	mov	[TEXT_MODE_SPACE-12], edi
	call	displayWholeTextMode
   .done:
	pop	cx
	pop	edi
	pop	esi
	pop	eax
	ret


;----------------------------------------------------
;  create new line for the commands
;----------------------------------------------------

createNewCmdLine:
	push	eax
	push	esi

	mov	esi, [TEXT_MODE_SPACE-8]
	push	esi
	dec	esi
	xor	eax, eax
   .loop:
	inc	eax
	inc	esi
	cmp byte [esi], STN
	jne	.loop
   .cont:
	pop	esi
	add	esi, eax
	mov	[TEXT_MODE_SPACE-8], esi
	mov	[TEXT_MODE_SPACE-12], esi
	call	displayWholeTextMode
   .done:
	pop	esi
	pop	eax
	ret



;---------------------------------------------------
; Display the whole text mode screen
; from the text mode space.
;---------------------------------------------------

displayWholeTextMode:
	pusha

	mov	edi, SCREEN_BUFFER			; clear entire screen
	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]
	movzx	ebx, word [vbeModeInfo_XResolution]
	movzx	ecx, word [vbeModeInfo_YResolution]

   .is32BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 32
	jne	.is24BPP
	mov	eax, ebp
	mul	ecx
	mov	ecx, eax
	shr	ecx, 2
	mov	eax, [TextColour.background]
	rep	stosd
	jmp	.cont

   .is24BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 24
	jne	.done
	mov	edx, [TextColour.background]
	.is24BPP_loop1:
		push	ecx
		push	edi
		mov	ecx, ebx
		.is24BPP_loop2:
			mov	eax, edx
			stosw
			shr	eax, 16
			stosb
			dec	ecx
			jnz	.is24BPP_loop2
		pop	edi
		add	edi, ebp
		pop	ecx
		dec	ecx
		jnz	.is24BPP_loop1


   .cont:
	mov	esi, [TEXT_MODE_SPACE-12]
	mov	edi, SCREEN_BUFFER

	mov byte [.cursorPosY], 0
	mov	ch, [TextSize.Y]
	mov	dl, [TextSize.X]
.loopingY:
	mov byte [.cursorPosX], 0
	mov	cl, dl
   .loopingX:
	lodsb
	cmp	al, STN					; check if special_textmode_newline
	je	.endX
	cmp	al, 0
	je	.done
	call	.printchr
	dec	cl
	jnz	.loopingX				; if end of X-axis reached
	inc	esi					; skip special_textmode_newline
   .endX:
	inc byte [.cursorPosY]
	dec	ch
	jnz	.loopingY				; if end of Y-axis reached

   .done:
	call	displayWholeScreen			; display finally
	popa
	ret


.printchr:			; a modified version of the original printchr function
	pusha

	mov	esi, charFontsList			; get location of character font
	movzx	edx, [charsize.Y]
	and	eax, 0x000000FF
	mul	edx
	add	esi, eax				; esi = charFontsList + dl*(bytes per char)

	movzx	eax, byte [.cursorPosY]
	movzx	ecx, byte [charsize.Y]
	mul	cx
	shl	eax, 16					; store the pixel Y_location in higher 16 bits of eax
	movzx	ax, byte [.cursorPosX]
	movzx	cx, byte [charsize.X]
	mul	cx					; eax low 16 bits = pixel X_location
	call	computeScreenOffset
	mov	edi, SCREEN_BUFFER
	add	edi, ebx

	mov	ebx, [TextColour.foreground]
	mov	edx, [TextColour.background]
   .printchr_is32BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 32
	jne	.printchr_is24BPP
	call	printchr_32BPP
	jmp	.printchr_done
   .printchr_is24BPP:
	cmp byte [vbeModeInfo_BitsPerPixel], 24
	jne	.printchr_done
	call	printchr_24BPP

   .printchr_done:
	inc byte [.cursorPosX]			; prepare for the next character
	popa
	ret

   .cursorPosX: db 0
   .cursorPosY: db 0

