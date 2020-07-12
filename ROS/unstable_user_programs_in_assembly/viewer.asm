
;*********************************************
;	YEMELITCOS User Program
;
;	viewer.asm
;
;*********************************************
; Content: (Last checked: 21 Nov 2012)
;	-
;	-
;*********************************************

use32

include "../general_info.inc"			; contains no code (only MACROs)

org	PROGRAM_LOCATION

jmp	start_viewer


startMsg	db 13, "Viewer program is running...", 13, 0
endMsg		db "Viewer program has ended.", 13, 0

start_viewer:
	pusha

	mov	esi, startMsg
	call	printstr

	mov	esi, .imageFileName
	mov	edi, PROGRAM_LOCATION + 0x100000
	call dword [_LoadFile]

	mov	edi, BMP_Header
	mov	esi, PROGRAM_LOCATION + 0x100000
	mov	ecx, 27
	rep	movsw

	cmp word [BMP_Header.signature], 0x4D42
	jne	.error
	cmp word [BMP_Header.numberOfPlanes], 1
	jne	.error
	cmp word [BMP_Header.bitsPerPixel], 24
	jne	.error

	mov	edi, SCREEN_BUFFER
	mov	eax, 0x02000000
	call	computeScreenOffset
	add	edi, ebx
	mov	ecx, [BMP_Header.YSize]
	mov	esi, 0x40000
	add	esi, [BMP_Header.offsetToStartOfImageData]

	movzx	ebp, word [vbeModeInfo_BytesPerScanLine]
	mov	al, [vbeModeInfo_BitsPerPixel]

   .is32BPP:
	cmp	al, 32
	jne	.is24BPP
	.loop1:
	push	ecx
	push	edi
	mov	ecx, [BMP_Header.XSize]
	  .loop2:
		movsd
		inc	edi
		dec	ecx
		jnz	.loop2
	pop	edi
	sub	edi, ebp
	pop	ecx
	dec	ecx
	jnz	.loop1
	jmp	.cont

   .is24BPP:
	cmp	al, 32
	jne	.done
	.loop3:
	push	ecx
	push	edi
	mov	ecx, [BMP_Header.XSize]
	  .loop4:
		movsw
		movsb
		inc	edi
		dec	ecx
		jnz	.loop4
	pop	edi
	sub	edi, ebp
	pop	ecx
	dec	ecx
	jnz	.loop3

   .cont:
	int	0x76				; display whole screen buffer
	mov	ecx, 0x2000000
   .wait:
	loop	.wait

   .done:
	mov	esi, endMsg
	call	printstr
	popa
	ret

   .error:
	mov	esi, .imageError
	call	printstr
	jmp	.done

   .imageFileName	db "IMAGE.BMP",0
   .imageError		db "   error: image file is not valid", 13, 0


BMP_Header:
	.signature:			dw 0		; must be 0x4D42
	.file_size:			dd 0
	.reserved:			dd 0
	.offsetToStartOfImageData:	dd 0
	.sizeOfBitmapInfoHeader:	dd 0		; must be 40
	.XSize:				dd 0
	.YSize:				dd 0
	.numberOfPlanes:		dw 0		; must be 1
	.bitsPerPixel:			dw 0
	.compressionType:		dd 0
	.sizeOfImageData:		dd 0		; includes padding
	.horizontalResolutionPPM:	dd 0
	.verticalResolutionPPM:		dd 0
	.numberOfImageColors:		dd 0
	.numberOfImportantColors:	dd 0


include "include/stdio.inc"
include "include/string.inc"
include "include/math.inc"


