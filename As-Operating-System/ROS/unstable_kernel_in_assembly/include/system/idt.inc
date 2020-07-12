
;----------------------------------------------------------
;
; set and/or modify IDT entries
;
;----------------------------------------------------------


;----------------------------------------------------------
; initialise IDT
;----------------------------------------------------------

initialise_idt:
	push	eax
	push	ecx
	push	edi

	mov	edi, unhandled_isr				; isr.inc
	mov	ecx, 0
     .loop:							; loop
	mov	eax, edi					; and
	mov	[(IDT_LOCATION+ecx)], ax			; set whole IDT
	add	ecx, 2						; to one ISR

	mov	dword [(IDT_LOCATION+ecx)], 0x8E000008		; IDT_LOCATION is specified
	add	ecx, 4						; in general_info.inc

	mov	eax, edi
	shr	eax, 16
	mov	[(IDT_LOCATION+ecx)], ax
	add	ecx, 2

	cmp	ecx, 0x800					; 256 ints
	jb	.loop 						; * 8 bytes each

	pop	edi
	pop	ecx
	pop	eax
	ret



;-----------------------------------------------
; modify IDT, set any int
;	in: cl  = int number
;	    edi = int function pointer
;-----------------------------------------------

set_int:
	push	eax
	push	ecx
	push	edi

	mov	al, 8 						; 8 bytes for each int
	mul	cl						; cl * al = ax
	movzx   ecx, ax						; ecx = offset in IDT (1/2 for dword list)
;	mov	[(idt_list+ecx/2)], edi				; add new int to idt entry list

	mov	eax, edi
	mov	[(IDT_LOCATION+ecx)], ax
	add	ecx, 2

	mov	dword [(IDT_LOCATION+ecx)], 0x8E000008
	add	ecx, 4

	mov	eax, edi
	shr	eax, 16
	mov	[(IDT_LOCATION+ecx)], ax

	pop	edi
	pop	ecx
	pop	eax
	ret


;---------------------------------------------------
; get int address
;	in:  cl  = int number
;	out: esi = address or 0 if none present
;---------------------------------------------------

get_int:
	push	eax
	mov	eax, 4						; 4 bytes for each address
	mul	cl						; cl * al = ax
	mov	esi, idt_list
	add	esi, eax
	pop	eax
	ret


;-----------------------------------------------
; set interupts from list
;-----------------------------------------------

set_idt_list:
	push	eax
	push	ecx
	push	esi
	push	edi

	mov	esi, idt_list
	mov	ecx, 0
     .loop:
	lodsd
	or	eax, eax
	jz	.next
	mov	edi, eax
	call	set_int
     .next:
	inc	ecx
	cmp	ecx, 0x100
	jb	.loop

	pop	edi
	pop	esi
	pop	ecx
	pop	eax
	ret



;--------------------------------------------
;   list of idt entries.
;           0 = not handled
;--------------------------------------------
idt_list:	   dd   isr0
		   dd   isr1
		   dd   isr2
		   dd   isr3
		   dd   isr4
		   dd   isr5
		   dd   isr6
		   dd   isr7
		   dd   isr8
		   dd   isr9
		   dd   isr10
		   dd   isr11
		   dd   isr12
		   dd   isr13
		   dd   isr14
		   dd   0
		   dd   isr16
		   dd   isr17
		   dd   isr18
		   dd   isr19
	times 12 dd	0			; 12 unused ints
		   dd   isr32
		   dd   isr33
		   dd   0
		   dd   0
		   dd   0
		   dd   0
		   dd   isr38
		   dd   0
		   dd   0
		   dd   0
		   dd   0
		   dd   0
		   dd   isr44
	times 71 dd	0
		   dd   isr116
		   dd   isr117
		   dd   isr118
	times 137 dd	0			; 210 unused ints

