
;*********************************************
;	YEMELITCOS User Program
;
;	editor.asm
;
;*********************************************
; Content: (Last checked: 21 Nov 2012)
;	-
;	-
;*********************************************

use32

include "../general_info.inc"			; contains no code (only MACROs)

org	PROGRAM_LOCATION

jmp	start_editor


startMsg	db 13, "Editor program is running...", 13, 0
endMsg		db "Editor program has ended.", 13, 0

start_editor:
	pusha

	mov	esi, startMsg
	call	printstr

	mov	esi, text
	call	printstr

	mov	esi, endMsg
	call	printstr

	popa
	ret


text:	times 400	db 'E'
	db 13, 0



include "include/stdio.inc"
include "include/string.inc"
include "include/math.inc"



