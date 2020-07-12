

;*********************************************
;   YEMELITCOS Initialisation Stage
;*********************************************

os_initialisation:

	;------------------------
	;  remap PICs
	;------------------------
	mov	cl, 0x20				; PIC 1, irq0-irq7 -> int 0x20-27.
	mov	ch, 0x28				; PIC 2, irq8-irq15 -> int 0x28-30.
	call	remap_pics
	call	disable_irqs


	;-----------------------
	;  setup IDT
	;-----------------------
	call	initialise_idt				; idt.inc
	call	set_idt_list				; idt.inc

	sti						; Interrupts are back...!


	;-------------------------------------------------
	;  Set the Programmable Interval Timer (PIT)
	;  And enable timer IRQ
	;-------------------------------------------------
	call	set_pit_freq				; timer.inc
	mov	cl, 0
	call	enable_irq				; Enable IRQ 0.


	;---------------------------
	;  enable keyboard IRQ
	;---------------------------
	mov	cl, 1					; Enable IRQ 1,
	call	enable_irq				; the keyboard IRQ.


	;---------------------------
	;  enable mouse IRQ
	;---------------------------
	call	enable_ps2_mouse
	mov	cl, 12					; Enable IRQ 12,
	call	enable_irq				; the mouse default IRQ.

	mov	cl, 2					; Enable IRQ 2,
	call	enable_irq				; Cascade interrupt for IRQs 8-15.


	;------------------------------------------
	;  enable Floppy Drive Controller IRQ
	;------------------------------------------
;	mov	cl, 6					; Enable IRQ 6,
;	call	enable_irq				; the FDC IRQ.
;	call	flpydsk_install				; fdc.inc


	;------------------------------------------------------------------------
	; Kernel functions that cannot yet be handled by interrupts
	; To call them, use " call dword [_theFunctionName] "
	;------------------------------------------------------------------------

	mov dword [_LoadFile], LoadFile


	call	initialiseTextMode

	ret


