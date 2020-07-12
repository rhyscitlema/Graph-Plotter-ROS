
;----------------------------------------------------------
;
; ISR (Interrupt Service Routines)
;
;----------------------------------------------------------


;---------------------------
;  Unhandled Interrupts
;---------------------------
unhandled_isr:
	call	reboot
	iret


;----------------------------
;   Timer IRQ 0. ISR 32.
;----------------------------
isr32:
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	timer_isr			; timer.inc

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret


;--------------------------
;  Keyboard IRQ 1. ISR 33.
;--------------------------
isr33:
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	keyboard_isr			; keyboard.inc

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret


;--------------------------
;  FDC IRQ 6. ISR 38.
;--------------------------
isr38:
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	fdc_isr				; fdc.inc
	mov	al, 0x20
	out	0x20, al

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret


;--------------------------
;  Mouse IRQ 12. ISR 44.
;--------------------------
isr44:
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	mouse_isr			; mouse.inc
	mov al,0x20
	out 0xA0,al
	out 0x20,al

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret



;----------------------------------------------
; YEMELITCOS SOFTWARE INTERRUPTS
;----------------------------------------------

isr116:						; isr 0x74
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	printchr			; al = character

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret


isr117:						; isr 0x75
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	LoadFile

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret


isr118:						; isr 0x76
	pusha
	push	ds
	push	es
	push	fs
	push	gs

	call	displayWholeScreen

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popa
	iret





;----------------------------------------------
; SPECIAL SYSTEM INTERRUPTS
; These will be set correctly over time.
;----------------------------------------------

isr0:
	call	reboot
	iret
isr1:
	call	reboot
	iret
isr2:
	call	reboot
	iret
isr3:
	call	reboot
	iret
isr4:
	call	reboot
	iret
isr5:
	call	reboot
	iret
isr6:
	call	reboot
	iret
isr7:
	call	reboot
	iret
isr8:
	call	reboot
	iret
isr9:
	call	reboot
	iret
isr10:
	call	reboot
	iret
isr11:
	call	reboot
	iret
isr12:
	call	reboot
	iret
isr13:
	call	reboot
	iret
isr14:
	call	reboot
	iret

; No isr15 !

isr16:
	call	reboot
	iret
isr17:
	call	reboot
	iret
isr18:
	call	reboot
	iret
isr19:
	call	reboot
	iret


