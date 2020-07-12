

 Note: This file code is NOT complete and is NOT used either.


; The Command/Status Port returns a bit mask referring to the status of a channel when read.
	ATA_SR_BSY	equ	0x80
	ATA_SR_DRDY	equ	0x40
	ATA_SR_DF	equ	0x20
	ATA_SR_DSC	equ	0x10
	ATA_SR_DRQ	equ	0x08
	ATA_SR_CORR	equ	0x04
	ATA_SR_IDX	equ	0x02
	ATA_SR_ERR	equ	0x01

; The Features/Error Port, which returns the most recent error upon read, has these possible bit masks.
	ATA_ER_BBK	equ	0x80
	ATA_ER_UNC	equ	0x40
	ATA_ER_MC	equ	0x20
	ATA_ER_IDNF	equ	0x10
	ATA_ER_MCR	equ	0x08
	ATA_ER_ABRT	equ	0x04
	ATA_ER_TK0NF	equ	0x02
	ATA_ER_AMN	equ	0x01

; When you write to the Command/Status port, you are executing one of these commands.
	ATA_CMD_READ_PIO	equ	0x20
	ATA_CMD_READ_PIO_EXT	equ	0x24
	ATA_CMD_READ_DMA	equ	0xC8
	ATA_CMD_READ_DMA_EXT	equ	0x25
	ATA_CMD_WRITE_PIO	equ	0x30
	ATA_CMD_WRITE_PIO_EXT	equ	0x34
	ATA_CMD_WRITE_DMA	equ	0xCA
	ATA_CMD_WRITE_DMA_EXT	equ	0x35
	ATA_CMD_CACHE_FLUSH	equ	0xE7
	ATA_CMD_CACHE_FLUSH_EXT	equ	0xEA
	ATA_CMD_PACKET		equ	0xA0
	ATA_CMD_IDENTIFY_PACKET	equ	0xA1
	ATA_CMD_IDENTIFY	equ	0xEC

; These commands are for ATAPI devices, which will be understood soon.
	ATAPI_CMD_READ		equ	0xA8
	ATAPI_CMD_EJECT		equ	0x1B

; ATA_CMD_IDENTIFY_PACKET and ATA_CMD_IDENTIFY return a buffer of 512 bytes called the identification space; the following definitions are used to read information from the identification space.
	ATA_IDENT_DEVICETYPE	equ	0
	ATA_IDENT_CYLINDERS	equ	2
	ATA_IDENT_HEADS		equ	6
	ATA_IDENT_SECTORS	equ	12
	ATA_IDENT_SERIAL	equ	20
	ATA_IDENT_MODEL		equ	54
	ATA_IDENT_CAPABILITIES	equ	98
	ATA_IDENT_FIELDVALID	equ	106
	ATA_IDENT_MAX_LBA	equ	120
	ATA_IDENT_COMMANDSETS	equ	164
	ATA_IDENT_MAX_LBA_EXT	equ	200

; When you select a drive, you should specify the interface type and whether it is the master or slave:
	IDE_ATA		equ	0x00
	IDE_ATAPI	equ	0x01
 
	ATA_MASTER	equ	0x00
	ATA_SLAVE	equ	0x01

; Task File is a range of 8 ports which are offsets from BAR0 (primary channel) and/or BAR2 (secondary channel). To exemplify:
; BAR0 + 0 is first port.
; BAR0 + 1 is second port.
; BAR0 + 2 is the third 
	ATA_REG_DATA		equ	0x00
	ATA_REG_ERROR		equ	0x01
	ATA_REG_FEATURES	equ	0x01
	ATA_REG_SECCOUNT0	equ	0x02
	ATA_REG_LBA0		equ	0x03
	ATA_REG_LBA1		equ	0x04
	ATA_REG_LBA2		equ	0x05
	ATA_REG_HDDEVSEL	equ	0x06
	ATA_REG_COMMAND		equ	0x07
	ATA_REG_STATUS		equ	0x07
	ATA_REG_SECCOUNT1	equ	0x08
	ATA_REG_LBA3		equ	0x09
	ATA_REG_LBA4		equ	0x0A
	ATA_REG_LBA5		equ	0x0B
	ATA_REG_CONTROL		equ	0x0C
	ATA_REG_ALTSTATUS	equ	0x0C
	ATA_REG_DEVADDRESS	equ	0x0D

; Channels
	ATA_PRIMARY	equ	0x00
	ATA_SECONDARY	equ	0x01
 
; Directions
	ATA_READ	equ	0x00
	ATA_WRITE	equ	0x01

; global structure IDEChannelRegisters 0 and 1:
channels:
channel0:
	.base	dw 0		; I/O Base.
	.ctrl	dw 0		; Control Base
	.bmide	dw 0		; Bus Master IDE
	.nIEN	db 0		; nIEN (No Interrupt);
channel1:
	.base	dw 0		; I/O Base.
	.ctrl	dw 0		; Control Base
	.bmide	dw 0		; Bus Master IDE
	.nIEN	db 0		; nIEN (No Interrupt);


; We also need a buffer to read the identification space into, we need a variable that indicates if an irq is invoked or not, and finally we need an array of 6 words [12 bytes] for ATAPI Drives:

	ide_buf:   times 2048	db 0
	ide_irq_invoked		db 0
	atapi_packet:		db 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


; the IDE can contain up to 4 drives
ide_devices:
ide_device_0:
	.Reserved	db 0			; 0 (Empty) or 1 (This Drive really exists).
	.Channel	db 0			; 0 (Primary Channel) or 1 (Secondary Channel).
	.Drive		db 0			; 0 (Master Drive) or 1 (Slave Drive).
	.Type		dw 0			; 0: ATA, 1:ATAPI.
	.Signature	dw 0			; Drive Signature
	.Capabilities	dw 0			; Features.
	.CommandSets	dd 0			; Command Sets Supported.
	.Size		dd 0			; Size in Sectors.
	.Model: times 41	db 0			; Model in string.
ide_device_1:
	.Reserved	db 0			; 0 (Empty) or 1 (This Drive really exists).
	.Channel	db 0			; 0 (Primary Channel) or 1 (Secondary Channel).
	.Drive		db 0			; 0 (Master Drive) or 1 (Slave Drive).
	.Type		dw 0			; 0: ATA, 1:ATAPI.
	.Signature	dw 0			; Drive Signature
	.Capabilities	dw 0			; Features.
	.CommandSets	dd 0			; Command Sets Supported.
	.Size		dd 0			; Size in Sectors.
	.Model: times 41	db 0			; Model in string.
ide_device_2:
	.Reserved	db 0			; 0 (Empty) or 1 (This Drive really exists).
	.Channel	db 0			; 0 (Primary Channel) or 1 (Secondary Channel).
	.Drive		db 0			; 0 (Master Drive) or 1 (Slave Drive).
	.Type		dw 0			; 0: ATA, 1:ATAPI.
	.Signature	dw 0			; Drive Signature
	.Capabilities	dw 0			; Features.
	.CommandSets	dd 0			; Command Sets Supported.
	.Size		dd 0			; Size in Sectors.
	.Model: times 41	db 0			; Model in string.
ide_device_3:
	.Reserved	db 0			; 0 (Empty) or 1 (This Drive really exists).
	.Channel	db 0			; 0 (Primary Channel) or 1 (Secondary Channel).
	.Drive		db 0			; 0 (Master Drive) or 1 (Slave Drive).
	.Type		dw 0			; 0: ATA, 1:ATAPI.
	.Signature	dw 0			; Drive Signature
	.Capabilities	dw 0			; Features.
	.CommandSets	dd 0			; Command Sets Supported.
	.Size		dd 0			; Size in Sectors.
	.Model: times 41	db 0			; Model in string.


;---------------------------------------------------------------------------------------------
; When we read a register in a channel, like STATUS Register, it is easy to execute:
;	in: al = channel
;	    ah = register
;	out bl = byte been read
;---------------------------------------------------------------------------------------------

ide_read:
	push	ax
	push	cx
	push	dx
	movzx	cx, ah
	cmp	al, 1
	je	.for_channel_1

.for_channel_0:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch0_cont1
	cmp	ah, 0x0C
	jae	.ch0_cont1
	push	ax				; save ah
	mov	ah, ATA_REG_CONTROL
	mov	bl, 0x80
	and	bl, [channel0.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	pop	ax
   .ch0_cont1:
	cmp	ah, 0x08			; if (reg < 0x08)
	jae	.ch0_cont2
	mov	dx, [channel0.base]		; inb(channels[channel].base + reg - 0x00);
	add	dx, cx
	in	al, dx				; READ
	jmp	.ch0_end
   .ch0_cont2:
	cmp	ah, 0x0C			; if (reg < 0x0C)
	jae	.ch0_cont3
	mov	dx, [channel0.base]		; inb(channels[channel].base + reg - 0x06);
	add	dx, cx
	sub	dx, 0x06
	in	al, dx				; READ
	jmp	.ch0_end
   .ch0_cont3:
	cmp	ah, 0x0E			; if (reg < 0x0E)
	jae	.ch0_cont4
	mov	dx, [channel0.ctrl]		; inb(channels[channel].ctrl + reg - 0x0A);
	add	dx, cx
	sub	dx, 0x0A
	in	al, dx				; READ
	jmp	.ch0_end
   .ch0_cont4:
	cmp	ah, 0x16			; if (reg < 0x16)
	jae	.ch0_end
	mov	dx, [channel0.bmide]		; inb(channels[channel].bmide + reg - 0x0E);
	add	dx, cx
	sub	dx, 0x0E
	in	al, dx				; READ

   .ch0_end:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch0_done
	cmp	ah, 0x0C
	jae	.ch0_done
	push	ax				; save al
	mov	al, 0				; channel 0
	mov	ah, ATA_REG_CONTROL
	mov	bl, [channel0.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	pop	ax
   .ch0_done:
	mov	bl, al
	pop	dx
	pop	cx
	pop	ax
	ret

.for_channel_1:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch1_cont1
	cmp	ah, 0x0C
	jae	.ch1_cont1
	push	ax				; save ah
	mov	ah, ATA_REG_CONTROL
	mov	bl, 0x80
	and	bl, [channel1.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	pop	ax
   .ch1_cont1:
	cmp	ah, 0x08			; if (reg < 0x08)
	jae	.ch1_cont2
	mov	dx, [channel1.base]		; inb(channels[channel].base + reg - 0x00);
	add	dx, cx
	in	al, dx				; READ
	jmp	.ch1_end
   .ch1_cont2:
	cmp	ah, 0x0C			; if (reg < 0x0C)
	jae	.ch1_cont3
	mov	dx, [channel1.base]		; inb(channels[channel].base + reg - 0x06);
	add	dx, cx
	sub	dx, 0x06
	in	al, dx				; READ
	jmp	.ch1_end
   .ch1_cont3:
	cmp	ah, 0x0E			; if (reg < 0x0E)
	jae	.ch1_cont4
	mov	dx, [channel1.ctrl]		; inb(channels[channel].ctrl + reg - 0x0A);
	add	dx, cx
	sub	dx, 0x0A
	in	al, dx				; READ
	jmp	.ch1_end
   .ch1_cont4:
	cmp	ah, 0x16			; if (reg < 0x16)
	jae	.ch1_end
	mov	dx, [channel1.bmide]		; inb(channels[channel].bmide + reg - 0x0E);
	add	dx, cx
	sub	dx, 0x0E
	in	al, dx				; READ

   .ch1_end:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch1_done
	cmp	ah, 0x0C
	jae	.ch1_done
	push	ax				; save al
	mov	al, 1				; channel 1
	mov	ah, ATA_REG_CONTROL
	mov	bl, [channel1.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	pop	ax
   .ch1_done:
	mov	bl, al
	pop	dx
	pop	cx
	pop	ax
	ret


;---------------------------------------------------------------------------------------------
; Writing to registers
;	in: al = channel
;	    ah = register
;	    bl = byte to be writen
;---------------------------------------------------------------------------------------------

ide_write:
	push	ax
	push	cx
	push	bx
	push	dx
	movzx	cx, ah
	cmp	al, 1
	je	.for_channel_1

.for_channel_0:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch0_cont1
	cmp	ah, 0x0C
	jae	.ch0_cont1
	push	ax				; save ah
	push	bx				; save bl
	mov	ah, ATA_REG_CONTROL
	mov	bl, 0x80
	and	bl, [channel0.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	pop	bx
	pop	ax
   .ch0_cont1:
	cmp	ah, 0x08			; if (reg < 0x08)
	jae	.ch0_cont2
	mov	dx, [channel0.base]		; outb(channels[channel].base  + reg - 0x00, data);
	add	dx, cx
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch0_end
   .ch0_cont2:
	cmp	ah, 0x0C			; else if (reg < 0x0C)
	jae	.ch0_cont3
	mov	dx, [channel0.base]		; outb(channels[channel].base  + reg - 0x06, data);
	add	dx, cx
	sub	dx, 0x06
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch0_end
   .ch0_cont3:
	cmp	ah, 0x0E			; else if (reg < 0x0E)
	jae	.ch0_cont4
	mov	dx, [channel0.ctrl]		; outb(channels[channel].ctrl  + reg - 0x0A, data);
	add	dx, cx
	sub	dx, 0x0A
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch0_end
   .ch0_cont4:
	cmp	ah, 0x16			; else if (reg < 0x16)
	jae	.ch0_end
	mov	dx, [channel0.bmide]		; outb(channels[channel].bmide + reg - 0x0E, data);
	add	dx, cx
	sub	dx, 0x0E
	mov	al, bl
	out	dx, al				; WRITE

   .ch0_end:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch0_done
	cmp	ah, 0x0C
	jae	.ch0_done
	mov	al, 0				; channel 0
	mov	ah, ATA_REG_CONTROL
	mov	bl, [channel0.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
   .ch0_done:
	pop	dx
	pop	bx
	pop	cx
	pop	ax
	ret

.for_channel_1:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch1_cont1
	cmp	ah, 0x0C
	jae	.ch1_cont1
	push	ax				; save ah
	push	bx				; save bl
	mov	ah, ATA_REG_CONTROL
	mov	bl, 0x80
	and	bl, [channel1.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	pop	bx
	pop	ax
   .ch1_cont1:
	cmp	ah, 0x08			; if (reg < 0x08)
	jae	.ch1_cont2
	mov	dx, [channel1.base]		; outb(channels[channel].base  + reg - 0x00, data);
	add	dx, cx
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch1_end
   .ch1_cont2:
	cmp	ah, 0x0C			; else if (reg < 0x0C)
	jae	.ch1_cont3
	mov	dx, [channel1.base]		; outb(channels[channel].base  + reg - 0x06, data);
	add	dx, cx
	sub	dx, 0x06
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch1_end
   .ch1_cont3:
	cmp	ah, 0x0E			; else if (reg < 0x0E)
	jae	.ch1_cont4
	mov	dx, [channel1.ctrl]		; outb(channels[channel].ctrl  + reg - 0x0A, data);
	add	dx, cx
	sub	dx, 0x0A
	mov	al, bl
	out	dx, al				; WRITE
	jmp	.ch1_end
   .ch1_cont4:
	cmp	ah, 0x16			; else if (reg < 0x16)
	jae	.ch1_end
	mov	dx, [channel1.bmide]		; outb(channels[channel].bmide + reg - 0x0E, data);
	add	dx, cx
	sub	dx, 0x0E
	mov	al, bl
	out	dx, al				; WRITE

   .ch1_end:
	cmp	ah, 0x07			; if (reg > 0x07 && reg < 0x0C)
	jbe	.ch1_done
	cmp	ah, 0x0C
	jae	.ch1_done
	mov	al, 1				; channel 1
	mov	ah, ATA_REG_CONTROL
	mov	bl, [channel1.nIEN]
	call	ide_write			; ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
   .ch1_done:
	pop	dx
	pop	bx
	pop	cx
	pop	ax
	ret


;------------------------------------------------------------
; To read the identification space, we should read
; the Data Register as a double word 128 times.
; We can then copy them to our buffer.
;	in: al = channel
;	    ah = register
;	   ebx = buffer
;	   ecx = quads
;------------------------------------------------------------


; ??? awaiting update !!!
; ??? awaiting update !!!
; ??? awaiting update !!!










;------------------------------------------------------------
; Eject a CD-ROM drive.
;------------------------------------------------------------

eject_cd_drive:
	pusha

	mov	dx, 0x177		; status register
   .loop1:
	call	delayABit
	in	al, dx			; sets al to status register (which is 8 bits).
	and	al, 10000000b		; If the first bit of the status register (BUSY-Bit) isn't 0, the device is busy,
	jne	.loop1			; so keep looping until it isn't.

	cli				; Clear interrupts so something doesn't interrupt the drive or controller
					; while this program is working.

	MOV	DX, 0x177		; status register again
   .LOOP2:
	call	delayABit
	IN	AL, DX			; sets al to status register again
	AND	AL, 01000000b		; If the second bit of the status register (DRDY) isn't 1, the device isn't
	JE	.LOOP2			; ready, so keep looping until it is.

	MOV	DX, 0x176		; device/head register
	MOV	AL, 0			; 0 selects device 0 (master). 0x10 would select device 1 (slave).
	OUT	DX, AL			; selects master device


	;IMPORTANT: Set nIEN before you send the PACKET command!
	;Let's set nIEN to 1 so we can skip the INTRQ_Wait state.

	MOV	DX, 0x3F6		; Device Control register
	MOV	AL, 00001010b		; nIEN is the second bit from the right here
	OUT	DX, AL			; nIEN is now one!

	MOV	DX, 0x177		;command register
	MOV	AL, 0x0A0		;PACKET command
	OUT	DX, AL			;sends the command!


	mov	cx, 0xFFFF		; After sending the PACKET command, the host is to wait 400 nanoseconds before
   .wait:				; doing anything else.
	call	delayABit
	dec	cx
	jnz	.wait


	MOV	DX, 0x177		; status register again
   .LOOP3:
	call	delayABit
	IN	AL, DX			; sets AL to status register again
	AND	AL, 10000000b		; Poll until BUSY bit is clear.
	JNE	.LOOP3


	MOV	DX, 0x177		; status register again
   .LOOP4:
	call	delayABit
	IN	AL, DX
	AND	AL, 00001000b		; Also, poll until DRQ is one.
	JE	.LOOP4

	;----------------------------------------------------------------------------
	; NOW WE START SENDING THE COMMAND PACKET!!!
	;----------------------------------------------------------------------------

	mov	esi, .atapi_packet
	cld				; clear direction flag so SI gets incremented, not decremented
mov ecx, 12
   .COMPACKLOOP:			; command packet sending loop
	MOV	DX, 0x170		; data register

	; Because we're going to need to write a word (2 bytes), we can't just use an
	; 8-bit register like AL. For this operation, we'll need to use the full width
	; of the 16-bit accumulator AX. We'll use the LODSW opcode, which loads AX
	; with whatever DS:SI points to. Not only this, but if the direction flag is
	; cleared (which we did a few lines above with the CLD instruction), LODSW
	; also auto-increments SI.
	lodsw
	out	dx, ax			; send the current word of the command packet!!!

	MOV	DX, 0x3F6		; Alternate Status Register
	IN	AL, DX			; wait one I/O cycle

	loop	.COMPACKLOOP


	MOV	DX, 0x3F6		; Once again, let's read the Alternate Status Register and ignore the result,
	IN	AL, DX			; since the spec says so.


	;Okay... That's done.
	;Time to poll the status register until BUSY is 0 again.

	MOV	DX, 0x177		; status register
   .LOOP5:
	call	delayABit
	IN	AL, DX
	AND	AL, 10000000b
	JNE	.LOOP5

	;BUSY is zero here.
	;We're also supposed to check DRQ, but hey, screw it.
   .done:
	sti				; restore interrupts
	popa
	ret

   .atapi_packet:	db 0x1B, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0


