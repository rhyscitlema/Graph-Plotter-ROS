;*********************************************
;	YEMELITC-OS
;
;	Idt.inc
;
;*********************************************

	IDT_SIZE equ 0x800


;*************************************************************
; Interrupt Descriptor Table (IDT)
;	- This is for initiasing the IDT only
;	- Initiasation is done during 16bit real mode
;	- Actual specification of the table is at the kernel
;*************************************************************

LoadIDT:
	push	ax
	push	cx
	push	si
	push	di
	push	ds

	xor	ax, ax				; getting ready
	mov	ds, ax

	lidt	[idt_desc + KRNLDR_LOCATION]	; load IDT into IDTR

	pop	ds
	pop	di
	pop	si
	pop	cx
	pop	ax
	ret

idt_desc: 
	dw IDT_SIZE		    ; limit (Size of IDT)
	dd IDT_LOCATION		; base of IDT


