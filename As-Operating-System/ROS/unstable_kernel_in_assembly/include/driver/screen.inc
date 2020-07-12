

putPixel:
	push	bx
	mov	bl, [vbeModeInfo_BitsPerPixel]
   .is32BPP:
	cmp	bl, 32
	jne	.is24BPP
	stosd
	jmp	.done
   .is24BPP:
	cmp	bl, 24
	jne	.done
	mov	[edi], ax
	ror	eax, 8
	mov	[edi+2], ah
	rol	eax, 8
	add	edi, 3
   .done:
	pop	bx
	ret


;------------------------------------------------------
; in: eax lower 16 bits = X position
;     eax higher 16 bits = Y position
; out: ebx = offset
;------------------------------------------------------

computeScreenOffset:
	push	eax
	push	ecx

	mov	ecx, eax

	movzx	eax, byte [vbeModeInfo_BitsPerPixel]
	shr	eax, 3
	mul	cx				; ax = X*BPP
	mov	ebx, eax

	shr	ecx, 16				; get Y position
	movzx	eax, word [vbeModeInfo_BytesPerScanLine]
	mul	ecx
	add	ebx, eax

	pop	ecx
	pop	eax
	ret


;-------------------------------------
; in: edx lower 16 bits = X position
;     edx higher 16 bits = Y position
; out: eax = colour
;-------------------------------------

getScreenBufferPixel:
	push	ebx
	push	edx
	push	esi

	mov	esi, SCREEN_BUFFER
	mov	eax, edx
	call	computeScreenOffset
	add	esi, ebx
	lodsd

	pop	esi
	pop	edx
	pop	ebx
	ret



;--------------------------------------------------------------------------
; Display content of buffer to video card screen
; This is in the form of a rectangular shape image
;
; in: eax = position on screen to start
;     ecx lower 16 bits = X size of rectangle
;     ecx higher 16 bits = Y size of rectangle
;--------------------------------------------------------------------------

displayToScreen:
	pusha

	cld
	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]

	mov	esi, SCREEN_BUFFER
	mov	edi, [vbeModeInfo_PhysBasePtr]
	call	computeScreenOffset
	add	esi, ebx
	add	edi, ebx

	movzx	eax, cx				; get ax = X size
	shr	ecx, 16				; get cx = Y size
   .loop:
	push	ecx
	push	esi
	push	edi
	mov	ecx, eax
	rep	movsd
	pop	edi
	pop	esi
	add	edi, ebp
	add	esi, ebp
	pop	ecx
	dec	ecx
	jnz	.loop

	call	displayMouse
	popa
	ret


;------------------------------------------
; Display whole screen
;------------------------------------------
displayWholeScreen:
    push    eax
	push	ecx
	push	esi
	push	edi
	cli
	cld
	movzx	eax, word [vbeModeInfo_BytesPerScanLine]
	movzx	ecx, word [vbeModeInfo_YResolution]
	shr	ecx, 2
	mul	ecx
	mov	ecx, eax
	mov	esi, SCREEN_BUFFER
	mov	edi, [vbeModeInfo_PhysBasePtr]
	rep	movsd
	call	displayMouse
	sti
	pop	edi
	pop	esi
	pop	ecx
	pop eax
	ret

