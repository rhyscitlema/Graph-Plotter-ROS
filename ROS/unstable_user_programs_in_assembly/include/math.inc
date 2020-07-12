
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


