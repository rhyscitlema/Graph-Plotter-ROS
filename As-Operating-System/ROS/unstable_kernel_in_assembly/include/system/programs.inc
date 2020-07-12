
USER_CODE_DESC		equ	0x18
USER_DATA_DESC		equ	0x20

;-----------------------------------------------------
; Load then run a program from the disk
;	in: esi = file name of program to load
;-----------------------------------------------------

load_program:
	pusha

	mov	edi, PROGRAM_LOCATION
	call	LoadFile				; fat12_fdd.inc
	jnc	.done					; file cout NOT be loaded

	mov	edi, .BIN_ext
	call	checkExtension
	jnc	.PE_Program				; if file is not a flat binary program
	mov	esi, .loaded
	call	printstr
	call	PROGRAM_LOCATION			; run the program as flat binary
	jmp	.endProgram

   .PE_Program:
	;-----------------------------------------------
	; Insure its a PE program (test for signiture)
	;-----------------------------------------------
	mov	ebx, [PROGRAM_LOCATION+60]
	add	ebx, PROGRAM_LOCATION			; ebx now points to file sig (PE00)
	mov	esi, ebx
	mov	edi, .PESig
	cmpsw
	je	.execute
  	mov	esi, .badPEImage
	call	printstr
	stc
	jmp	.done

   .execute:
	;-------------------------------------------------------------------------
	; parse the programs header info structures to get its entry point
	; ebx points to _IMAGE_FILE_HEADER
	;-------------------------------------------------------------------------
	add	ebx, 24
	mov	eax, [ebx]			; _IMAGE_FILE_HEADER is 20 bytes + size of sig (4 bytes)
	add	ebx, 20-4			; address of entry point
	mov	edi, [ebx]			; get entry point offset in code section	
	add	ebx, 12				; image base is offset 8 bytes from entry point
	mov	eax, [ebx]			; add image base
	add	edi, eax

	mov	esi, .loaded
	call	printstr
	call	edi				; Execute Program

   .endProgram:
	call	displayWholeTextMode
	stc
   .done:
	popa
	ret

   .loaded	db "---Program loaded----", 13, \
		   "---------------------", 13, 0
   .BIN_ext	db "BIN"
   .PESig	db "PE"
   .badPEImage	db "   Error: invalid PE program file", 0



;-----------------------------------------------------------
; Check extension of file name
;	in: esi = file name (has only one '.' character)
;	    edi = the extension string to check
;	out: CF = result
;-----------------------------------------------------------
checkExtension:
	push	ax
	push	esi
	push	edi
   .loop:
	lodsb
	cmp	al, '.'				; file name should contain only one '.' character
	jne	.loop
	mov	ecx, 3
	rep cmpsb
	jne	.notEqual
	stc
	jmp	.done
   .notEqual:
	clc
   .done:
	pop	edi
	pop	esi
	pop	ax
	ret



switch_to_user_mode:
	cli
	mov	ax, USER_DATA_DESC+3		; set lower 2 bits for the RPL
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax

	push	USER_DATA_DESC+3		; SS
	mov	eax, PROGRAM_LOCATION+0x800000
	push	eax				; ESP

	pushfd					; EFLAGS
	pop	eax
	or	eax, 0x200			; set the IF flag == prepare to re-enable interrupts
	push	eax

	push	USER_CODE_DESC			; CS
	mov	eax, PROGRAM_LOCATION
	push	eax				; EIP

	iret					; switch to user mode

