;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;   Mouse.inc
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


mouseSizeX	equ 8
mouseSizeY	equ 12

mousePos:						; in units of pixel
	.X	dw 0
	.Y	dw 0
PRVmousePos:
	.X	dw 0
	.Y	dw 0



;------------------------------------------------
; get and set mouse position
;	in: eax-H = y position
;	    eax-L = x position
;------------------------------------------------

getMousePos:
	mov	eax, [mousePos]
	ret
setMousePos:
	mov	[mousePos], eax
	ret


mouse_cycle	db 0
mouse_flag	db 0
delta_x		db 0
delta_y		db 0


mouse_isr:
	push	ax
   .flag:
	cmp	[mouse_cycle], 0
	jne	.deltaX
	in	al, 0x60
	test	al, 0x08				; check bit 3 so to check if the byte is
	jz	.done					; the flag/first byte of the packet.
	mov	[mouse_flag], al
	inc	[mouse_cycle]
	jmp	.done
   .deltaX:
	cmp	[mouse_cycle], 1
	jne	.deltaY
	in	al, 0x60
	mov	[delta_x], al				; get x position
	inc	[mouse_cycle]
	jmp	.done
   .deltaY:
	cmp	[mouse_cycle], 2
	jne	.Zmotion
	in	al, 0x60
	mov	[delta_y], al				; get y-position
;	inc	[mouse_cycle]
;	jmp	.done
   .Zmotion:
;	in	al, 0x60				; discard 4th byte of packet
	mov	[mouse_cycle], 0			; prepare for next packet

	call	update_mouse
   .done:
	pop	ax
	ret


update_mouse:
	push	ax
	push	bx
	push	cx

	movzx	bx, [mouse_flag]

	movzx	ax, [delta_x]				; clear ah, al remains
	test	bl, 0x10				; check if delta_x is negative
	jnz	.Xneg

	add	[mousePos.X], ax			; augment mousePosX by delta_x
	mov	cx, [vbeModeInfo_XResolution]
	sub	cx, mouseSizeX
	cmp	[mousePos.X], cx
	jbe	.cont1
	mov	[mousePos.X], cx			; mousePosX was too high
	jmp	.cont1
   .Xneg:
	neg	al					; change from two's complement to absolute value
	cmp	[mousePos.X], ax
	jbe	.Xtoolow
	sub	[mousePos.X], ax
	jmp	.cont1
   .Xtoolow:
	mov word [mousePos.X], 0

   .cont1:
	movzx	ax, [delta_y]
	test	bl, 0x20				; check if delta_y is negative
	jnz	.Yneg

	cmp	[mousePos.Y], ax
	jbe	.Ytoolow
	sub	[mousePos.Y], ax
	jmp	.cont2
   .Ytoolow:
	mov word [mousePos.Y], 0
	jmp	.cont2

   .Yneg:
	neg	al					; change from two's complement to absolute value
	add	[mousePos.Y], ax
	mov	cx, [vbeModeInfo_YResolution]
	sub	cx, 3
	cmp	[mousePos.Y], cx
	jbe	.cont2
	mov	[mousePos.Y], cx			; mousePosX was too high

   .cont2:
	call	underPreviousMousePos
	call	displayMouse
	pop	cx
	pop	bx
	pop	ax
	ret


underPreviousMousePos:
	pusha

	mov	edi, [vbeModeInfo_PhysBasePtr]		; edi = base + y*BPSL + x*BPP
	mov	eax, [PRVmousePos]
	call	computeScreenOffset
	add	edi, ebx

	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]
	mov	edx, [PRVmousePos]
	mov	ecx, mouseSizeY
   .outerLoop:
	push	ecx
	push	edi
	mov	ecx, mouseSizeX
	mov	dx, [PRVmousePos.X]
   .innerLoop:
	call	getScreenBufferPixel
	call	putPixel			; write the colour (in eax) to video memory
	inc	dx				; increment lower 16 bits of edx
	dec	ecx
	jnz	.innerLoop
	pop	edi
	add	edi, ebp			; recall that ebp = BPSL
	add	edx, 0x10000			; increment higher 16 bits of edx
	pop	ecx
	dec	ecx
	jnz	.outerLoop

	popa
	ret

displayMouse:
	pusha

	mov	edi, [vbeModeInfo_PhysBasePtr]		; edi = base + y*BPSL + x*BPP
	mov	eax, [mousePos]
	mov	[PRVmousePos], eax			; update mouse position
	call	computeScreenOffset
	add	edi, ebx

	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]
	movzx	ebx, byte [vbeModeInfo_BitsPerPixel]
	shr	ebx, 3				; get ebx = BytesPerPixel
	mov	esi, mouse_image
	mov	ch, mouseSizeY
   .outerLoop:
	mov	edx, edi
	mov	cl, mouseSizeX
   .innerLoop:
	cmp byte [esi], 0x11
	je	.skip				; skip pixel
	lodsd
	call	putPixel
	dec	cl
	jnz	.innerLoop
	jmp	.next
   .skip:
	add	esi, 4
	add	edi, ebx			; edi += BytesPerPixel
	dec	cl
	jnz	.innerLoop
   .next:
	mov	edi, edx
	add	edi, ebp
	dec	ch
	jnz	.outerLoop

	popa
	ret



mouse_image:
 dd	0x00000000, 0x00000000, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111
 dd	0x00000000, 0x00000000, 0x00000000, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x11111111, 0x11111111, 0x11111111, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x11111111, 0x11111111, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x11111111, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x11111111
 dd	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x11111111, 0x11111111
 dd	0x00000000, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x11111111, 0x11111111
 dd	0x00000000, 0x11111111, 0x00000000, 0x00000000, 0x00FFFFFF, 0x00000000, 0x11111111, 0x11111111
 dd	0x11111111, 0x11111111, 0x11111111, 0x00000000, 0x00000000, 0x00000000, 0x11111111, 0x11111111

;	11
;	111
;	1011
;	10011
;	100011
;	1000011
;	10000011
;	1000011
;	100001
;	111001
;	1 1101
;	   111



;-------------------------------------------------------
; Enabling the ps2 mouse
;	This functioon is used
;	only once, at os-initialisation.
;-------------------------------------------------------


enable_ps2_mouse:
	pusha
	cli

	mov	bl, 0xA8		; enable mouse device
	call	kb_cmd
	call	kb_read			; acknowledge

	mov	bl, 0x20		; prepare to get command byte
	call	kb_cmd
	call	kb_read			; get command byte
	or	al, 3			; bit 1 set = enable mouse interrupt (IRQ12)

	mov	bl, 0x60		; prepare to write modified command byte
	push	eax
	call	kb_cmd
	pop	eax
	call	kb_write		; write modified command byte
	call	kb_read			; acknowledge

	mov	bl, 0xD4
	call	kb_cmd
	mov	al, 0xF6		; set default values (100 packets/s, 4 pixels/mm)
	call	kb_write
	call	kb_read			; acknowledge

	mov	bl, 0xD4
	call	kb_cmd
	mov	al, 0xF4		; enable automatic streaming
	call	kb_write
	call	kb_read			; acknowledge

	mov	ax, [vbeModeInfo_XResolution]
	shr	ax, 1
	mov	[mousePos.X], ax
	mov	[PRVmousePos.X], ax

	mov	ax, [vbeModeInfo_YResolution]
	shr	ax, 1
	mov	[mousePos.Y], ax
	mov	[PRVmousePos.Y], ax

	sti
	popa
	ret



;-------------------------------------------------------
; These three (3) functions below are
; used only by the enable_ps2_mouse function
; above.
;-------------------------------------------------------


kb_read:
	push	ecx

	mov	ecx, 0x1000		; number of tries
   .wait:				; getting ready to read from port 0x60
	call	delayABit
	in	al, 0x64
	test	al, 1			; check bit 0
	jnz	.ready			; Output buffer full, so can be read
	loop	.wait
	mov	ah, 1			; there seem to be a problem
	jmp	.exit
   .ready:
	in	al, 0x60		; finally read
	xor	ah, ah
   .exit:
	pop	ecx
	ret


kb_cmd:
	push	ecx
	mov	ecx, 0x1000
   .wait:				; getting ready to write to port 0x64
	call	delayABit
	in	al, 0x64
	test	al, 2			; check bit 1
	jz	.ready			; Input buffer empty, can be written
	loop	.wait
	mov	ah, 1			; there seem to be a problem
	jmp	.exit
   .ready:
	mov	al, bl
	out	0x64, al		; finally write
	xor	ah, ah
   .exit:
	pop	ecx
	ret


kb_write:
	push	ecx
	push	ax			; save command byte

	mov	ecx, 0x1000
   .wait:				; getting ready to write to port 0x64
	call	delayABit
	in	al, 0x64
	test	al, 2			; check bit 1
	jz	.ready			; Input buffer empty, can be written
	loop	.wait
	mov	ah, 1			; there seem to be a problem
	jmp	.exit
   .ready:
	pop	ax			; recover al = command byte to write
	out	0x60, al		; finally write
	xor	ah, ah
   .exit:
	pop	ecx
	ret



;mouseGUI:
;	push	ebx
;	push	ecx
;	push	esi
;
;	sub	bx, [mousePos.X]
;	cmp	bh, 0
;	jne	.noPixel
;	cmp	bl, [mouseSize.X]
;	jae	.noPixel
;
;	mov	eax, ebx
;	shr	eax, 16					; ax = Y pixel
;	sub	ax, [mousePos.Y]
;	cmp	ah, 0
;	jne	.noPixel
;	cmp	al, [mouseSize.Y]
;	jae	.noPixel
;
;	mov	esi, mouse_image
;	mul	[mouseSize.X]				; ax = Y*10
;	mov	cx, 4
;	mul	cx
;	add	esi, eax
;	mov	eax, 4
;	mul	bx					; ax = bx*4
;	add	esi, eax
;	lodsd						; load colour (into eax)
;	cmp	al, 0x11
;	je	.noPixel
;
;	pop	esi
;	pop	ecx
;	pop	ebx
;	stc
;	ret
;   .noPixel:
;	pop	esi
;	pop	ecx
;	pop	ebx
;	clc
;	ret

