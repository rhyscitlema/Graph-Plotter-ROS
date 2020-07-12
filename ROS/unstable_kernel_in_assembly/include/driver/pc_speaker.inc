
;----------------------------------------------------------
;
;     Internal speaker functions
;
;----------------------------------------------------------

;-----------------------
;  turn on PC speaker
;    ax = frequency
;-----------------------
speaker_on:
	push	ax
	push	bx
	mov	bx, ax			; Store frequency

	mov	al, 0xB6
	out	0x43, al
	mov	al, bl			; Set up frequency
	out	0x42, al
	mov	al, bh
	out	0x42, al

	in	al, 0x61		; Switch PC speaker on
	or	al, 0x03
	out	0x61, al

	pop	bx
	pop	ax
	ret


;-----------------------------
;  turn pc speaker off
;-----------------------------
speaker_off:
	push	ax
	in	al, 0x61
	and	al, 0xFC		; some people use 0xFD
	out	0x61, al
	pop	ax
	ret


;----------------------------------
; PC speaker beep
;----------------------------------
speaker_beep:
	push	eax
	push	ecx
	mov	ax, 3200
	call	speaker_on
	mov	ecx, ONE_SECOND/2
	call	delay			; timer.inc
	call	speaker_off
	pop	ecx
	pop	eax
	ret

