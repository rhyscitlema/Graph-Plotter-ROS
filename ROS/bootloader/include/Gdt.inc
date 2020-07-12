;*********************************************
;	YEMELITC-OS
;
;	Gdt.inc
;
;*********************************************


;**************************************
; Load Global Descriptor Table (GDT)
; Copy it to GDT_LOCATION
;**************************************

LoadGDT:
	push	ax
	push	cx
	push	si
	push	di
	push	ds
	push	es

	xor	ax, ax				; getting ready
	mov	ds, ax
	mov	es, ax

	mov	si, gdt_start + KRNLDR_LOCATION
	mov	di, GDT_LOCATION
	mov	cx, gdt_end - gdt_start
	rep	movsb				; move GDT to memory GDT_LOCATION

	lgdt	[gdt_desc + KRNLDR_LOCATION]	; load GDT into GDTR

	pop	es
	pop	ds
	pop	di
	pop	si
	pop	cx
	pop	ax
	ret



;*******************************************
; Global Descriptor Table (GDT)
;*******************************************

gdt_start:

; gdt_null_descriptor: 
	dd 0, 0

; kernel_code_descriptor:
	dw 0xFFFF			; bits 0-15 of segment limit
	db 0,0,0			; bits 0-23 of base address
	db 10011010b			; access specifications
	db 11001111b			; bits 0-3 = bits 16-19 of segment limit
	db 0				; bits 24-31 of base address

; kernel_data_descriptor:
	dw 0xFFFF			; bits 0-15 of segment limit
	db 0,0,0			; bits 0-23 of base address
	db 10010010b			; access specifications
	db 11001111b			; bits 0-3 = bits 16-19 of segment limit
	db 0				; bits 24-31 of base address

; user_code_descriptor:
	dw 0xFFFF			; bits 0-15 of segment limit
	db 0,0,0			; bits 0-23 of base address
	db 10011010b			; access specifications
	db 11001111b			; bits 0-3 = bits 16-19 of segment limit
	db 0				; bits 24-31 of base address

; user_data_descriptor:
	dw 0xFFFF			; bits 0-15 of segment limit
	db 0,0,0			; bits 0-23 of base address
	db 11110010b			; access specifications
	db 11001111b			; bits 0-3 = bits 16-19 of segment limit
	db 0				; bits 24-31 of base address

gdt_end:

gdt_desc:
	dw gdt_end - gdt_start - 1 	; limit (Size of GDT)
	dd GDT_LOCATION			; base of GDT



; Descriptor offsets names
GDT_NULL_DESC		equ	0x00
KERNEL_CODE_DESC	equ	0x08
KERNEL_DATA_DESC	equ	0x10
USER_CODE_DESC		equ	0x18
USER_DATA_DESC		equ	0x20


