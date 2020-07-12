
;***********************************************************
;
; Floppy Drive Controller (FDC) Driver
;
;***********************************************************

	fdd_exists	db	0


; Controller I/O Ports
	FLPYDSK_DOR	equ	0x3F2
	FLPYDSK_MSR	equ	0x3F4
	FLPYDSK_FIFO	equ	0x3F5
	FLPYDSK_CTRL	equ	0x3F7

; Bits 0-4 of command byte
	FDC_CMD_READ_TRACK	equ	2
	FDC_CMD_SPECIFY		equ	3
	FDC_CMD_CHECK_STAT	equ	4
	FDC_CMD_WRITE_SECT	equ	5
	FDC_CMD_READ_SECT	equ	6
	FDC_CMD_CALIBRATE	equ	7
	FDC_CMD_CHECK_INT	equ	8
	FDC_CMD_WRITE_DEL_S	equ	9
	FDC_CMD_READ_ID_S	equ	10
	FDC_CMD_READ_DEL_S	equ	12
	FDC_CMD_FORMAT_TRACK	equ	13
	FDC_CMD_SEEK		equ	15

; Additional command masks. Can be masked with above commands.
	FDC_CMD_EXT_SKIP	equ	00100000b
	FDC_CMD_EXT_DENSITY	equ	01000000b
	FDC_CMD_EXT_MULTITRACK	equ	10000000b

; Digital Output Register (DOR)
	FLPYDSK_DOR_MASK_DRIVE0		equ	00000000b
	FLPYDSK_DOR_MASK_DRIVE1		equ	00000001b
	FLPYDSK_DOR_MASK_DRIVE2		equ	00000010b
	FLPYDSK_DOR_MASK_DRIVE3		equ	00000011b
	FLPYDSK_DOR_MASK_RESET		equ	00000100b
	FLPYDSK_DOR_MASK_DMA		equ	00001000b
	FLPYDSK_DOR_MASK_DRIVE0_MOTOR	equ	00010000b
	FLPYDSK_DOR_MASK_DRIVE1_MOTOR	equ	00100000b
	FLPYDSK_DOR_MASK_DRIVE2_MOTOR	equ	01000000b
	FLPYDSK_DOR_MASK_DRIVE3_MOTOR	equ	10000000b

; Main Status Register
	FLPYDSK_MSR_MASK_DRIVE1_POS_MODE	equ	00000001b
	FLPYDSK_MSR_MASK_DRIVE2_POS_MODE	equ	00000010b
	FLPYDSK_MSR_MASK_DRIVE3_POS_MODE	equ	00000100b
	FLPYDSK_MSR_MASK_DRIVE4_POS_MODE	equ	00001000b
	FLPYDSK_MSR_MASK_BUSY			equ	00010000b
	FLPYDSK_MSR_MASK_DMA			equ	00100000b
	FLPYDSK_MSR_MASK_DATAIO			equ	01000000b
	FLPYDSK_MSR_MASK_DATAREG		equ	10000000b

; Controller Status Port 0 (ST0)
	FLPYDSK_ST0_MASK_DRIVE0		equ	00000000b
	FLPYDSK_ST0_MASK_DRIVE1		equ	00000001b
	FLPYDSK_ST0_MASK_DRIVE2		equ	00000010b
	FLPYDSK_ST0_MASK_DRIVE3		equ	00000011b
	FLPYDSK_ST0_MASK_HEADACTIVE	equ	00000100b
	FLPYDSK_ST0_MASK_NOTREADY	equ	00001000b
	FLPYDSK_ST0_MASK_UNITCHECK	equ	00010000b
	FLPYDSK_ST0_MASK_SEEKEND	equ	00100000b
	FLPYDSK_ST0_MASK_INTCODE	equ	01000000b

; LPYDSK_ST0_MASK_INTCODE types
	FLPYDSK_ST0_TYP_NORMAL		equ	0
	FLPYDSK_ST0_TYP_ABNORMAL_ERR	equ	1
	FLPYDSK_ST0_TYP_INVALID_ERR	equ	2
	FLPYDSK_ST0_TYP_NOTREADY	equ	3

; GAP 3 sizes
	FLPYDSK_GAP3_LENGTH_STD		equ	42
	FLPYDSK_GAP3_LENGTH_5_14	equ	32
	FLPYDSK_GAP3_LENGTH_3_5		equ	27

; Formula: 2^sector_number * 128, where ^ denotes "to the power of"
	FLPYDSK_SECTOR_DTL_128		equ	0
	FLPYDSK_SECTOR_DTL_256		equ	1
	FLPYDSK_SECTOR_DTL_512		equ	2
	FLPYDSK_SECTOR_DTL_1024		equ	4

; Constants
	FLOPPY_IRQ		equ	6
	FLPY_SECTORS_PER_TRACK	equ	18
	FDC_DMA_CHANNEL		equ	2

	fdd_BytesPerSector	DW 512
	fdd_TotalSectors	DW 2880		;for 1.44 MB disk
	fdd_SectorsPerTrack	DW 18
	fdd_HeadsPerCylinder	DW 2

; Variables
	CurrentDrive	db 0
	FDC_IRQ_FIRED	db 0

	absoluteSector	db 0
	absoluteHead	db 0
	absoluteTrack	db 0



;**********************************************
;	Direct Memory Access (DMA) routines
;**********************************************


;------------------------------------------------------------------
; Initialize DMA
;	in: edi = buffer to read/write to
;	    cx = size of buffer (in bytes)
;------------------------------------------------------------------
dma_initialize_floppy:
	push	ax
	push	ebx

	mov	al, 1
	call	dma_reset
	mov	al, FDC_DMA_CHANNEL
	call	dma_mask_channel			; Mask channel 2

	mov	al, 1
	call	dma_reset_flipflop			; Flipflop reset on DMA 1
	mov	al, FDC_DMA_CHANNEL
	mov	ebx, edi				; get bx=di (offset)
	call	dma_set_address
	shr	ebx, 8					; get third byte of ebx into bh
	mov	ah, bh					; ah=bh=page, where edi = page:offset
	call	dma_set_external_page_register

	mov	al, 1
	call	dma_reset_flipflop			; Flipflop reset on DMA 1
	mov	al, FDC_DMA_CHANNEL
	mov	bx, cx
	call	dma_set_count

	call	dma_set_read
	mov	al, 1
	call	dma_unmask_all				; Unmask channel 2

	pop	ebx
	pop	ax
	ret


;------------------------------------------------------------------
; Initialize DMA to use physical address 0x1000 up to 0x1000+64k
;------------------------------------------------------------------
flpydsk_initialize_dma:
	push	ax
	mov	al, 0x06
	out	0x0A, al	; mask dma channel 2
	mov	al, 0xFF
	out	0xD8, al	; reset master flip-flop
	mov	al, 0x00
	out	0x04, al	; address=0x1000
	mov	al, 0x10
	out	0x04, al
	mov	al, 0xFF
	out	0xD8, al	; reset master flip-flop
	mov	al, 0xFF
	out	0x05, al	; count to 0x23ff (number of bytes in a 3.5" floppy disk track)
	mov	al, 0x23
	out	0x05, al
	mov	al, 0x00
	out	0x80, al	; external page register = 0
	mov	al, 0x02
	out	0x0A, al	; unmask dma channel 2
	pop	ax
	ret


;---------------------------------------
; Prepare the DMA for read transfer
;---------------------------------------
flpydsk_dma_read:
	push	ax
	mov	al, 0x06
	out	0x0A, al	; mask dma channel 2
	mov	al, 0x56
	out	0x0B, al	; single transfer, address increment, autoinit, read, channel 2
	mov	al, 0x02
	out	0x0A, al	; unmask dma channel 2
	pop	ax
	ret


;-----------------------------------------
; Prepare the DMA for write transfer
;-----------------------------------------
flpydsk_dma_write:
	push	ax
	mov	al, 0x06
	out	0x0A, al	; mask dma channel 2
	mov	al, 0x5A
	out	0x0B, al	; single transfer, address increment, autoinit, write, channel 2
	mov	al, 0x02
	out	0x0A, al	; unmask dma channel 2
	pop	ax
	ret



;**********************************************
;	Basic Controller I/O Routines
;**********************************************

;------------------------------
; Send command byte to fdc
;	in: al = command
;------------------------------
flpydsk_send_command:
	push	ax
	push	dx
	push	ecx
	mov	ah, al
	mov	ecx, 500
	mov	dx, FLPYDSK_MSR
   .wait:			; wait until data register is ready. We send commands to the data register
	in	al, dx
	test	al, FLPYDSK_MSR_MASK_DATAREG
	jnz	.ready
	loop	.wait
	jmp	.done
   .ready:
	mov	al, ah
	mov	dx, FLPYDSK_FIFO
	out	dx, al
   .done:
	pop	ecx
	pop	dx
	pop	ax
	ret


;------------------------------
; Read data byte from fdc
;	out: al = data
;------------------------------
flpydsk_read_data:
	push	dx
	push	ecx
	mov	ecx, 500	; number of tries
	mov	dx, FLPYDSK_MSR
   .wait:			; wait until data register is ready. We send commands to the data register
	in	al, dx
	test	al, FLPYDSK_MSR_MASK_DATAREG
	jnz	.ready
	loop	.wait
	jmp	.done
   .ready:
	mov	dx, FLPYDSK_FIFO
	in	al, dx
   .done:
	pop	ecx
	pop	dx
	ret



;**********************************************
;	Interrupt Handling Routines
;**********************************************

;--------------------------
; Wait for irq to fire
;--------------------------
flpydsk_wait_irq:
   .loop:
	cmp byte [FDC_IRQ_FIRED], 0
	je	.loop
	mov byte [FDC_IRQ_FIRED], 0
	ret

;--------------
; FDC ISR
;--------------
fdc_isr:
	mov byte [FDC_IRQ_FIRED], 1
	ret



;**********************************************
;	Controller Command Routines
;**********************************************

;--------------------------------------
; Check interrupt status command
;	out: ah = ST0
;	    al = cylinder/track
;--------------------------------------
flpydsk_check_int:
	mov	al, FDC_CMD_CHECK_INT
	call	flpydsk_send_command
	call	flpydsk_read_data	; read first data from fdc
	mov	ah, al
	call	flpydsk_read_data	; reat second data from fdc
	ret

;------------------------------------------------------
; Turns the current floppy drives motor on/off
;	in: al = boolean, Turn on or Turn off
;------------------------------------------------------
flpydsk_control_motor:
	push	ax
	push	dx

	; select the correct mask based on current drive
	cmp	[CurrentDrive], 0
	jne	.cont0
	mov	[.motor], FLPYDSK_DOR_MASK_DRIVE0_MOTOR
	jmp	.next
   .cont0:
	cmp	[CurrentDrive], 1
	jne	.cont1
	mov	[.motor], FLPYDSK_DOR_MASK_DRIVE1_MOTOR
	jmp	.next
   .cont1:
	cmp	[CurrentDrive], 2
	jne	.cont2
	mov	[.motor], FLPYDSK_DOR_MASK_DRIVE2_MOTOR
	jmp	.next
   .cont2:
	cmp	[CurrentDrive], 3
	jne	.cont3
	mov	[.motor], FLPYDSK_DOR_MASK_DRIVE3_MOTOR
	jmp	.next
   .cont3:
	jmp	.done

   .next:
	cmp	al, 0				; turn OFF the motor of that drive
	jne	.TurnON
	mov	al, FLPYDSK_DOR_MASK_RESET
	mov	dx, FLPYDSK_DOR
	out	dx, al				; now turn off
	jmp	.done
   .TurnON:
	mov	al, [.motor]
	or	al, [CurrentDrive]
	or	al, FLPYDSK_DOR_MASK_RESET
	or	al, FLPYDSK_DOR_MASK_DMA
	mov	dx, FLPYDSK_DOR
	out	dx, al				; now turn on

   .done:
	mov	dx, 5
   .wait1:
	mov	ax, 0xFFFF
   .wait2:					; in all cases; wait a little bit for
	sub	ax, 1				; the motor to spin up/turn off
	cmp	ax, 0
	ja	.wait2
	sub	dx, 1
	cmp	dx, 0
	ja	.wait1

	pop	dx
	pop	ax
	ret
   .motor db 0


;--------------------------
; Configure drive
;	in: al = stepr
;	    bl = loadt
;	    ah = unloadt
;	    bh = dma (boolean)
;--------------------------
flpydsk_drive_data:
	push	ax
	mov	al, FDC_CMD_SPECIFY
	call	flpydsk_send_command

	and	al, 0x0F
	shl	al, 4
	and	ah, 0x0F
	or	al, ah
	call	flpydsk_send_command

	cmp	bh, 0
	je	.is0
	shl	bl, 1
	jnz	.is0
	mov	al, 1
	call	flpydsk_send_command
 	jmp	.done
   .is0:
	mov	al, 0
	call	flpydsk_send_command
   .done:
	pop	ax
	ret


;--------------------------------------
; Calibrates the drive
;	in: al = drive
;	out: al = 0  fine
;	     al = 1  not-fine
;	     al = 2  input not valid
;--------------------------------------
flpydsk_calibrate:
	push	ax
	push	ecx

	cmp	al, 3
	jbe	.cont				; if drive is greater than 3
	mov	al, 2
	jmp	.done
   .cont:
	mov	al, 1
	call	flpydsk_control_motor		; turn ON the motor
	mov	ah, al				; ah = drive
	mov	ecx, 10				; number of tries
   .loop:
	mov	al, FDC_CMD_CALIBRATE
	call	flpydsk_send_command
	mov	al, ah
	call	flpydsk_send_command

	call	flpydsk_wait_irq
	call	flpydsk_check_int		; returns al = cyl
	cmp	al, 0				; did we fine cylinder 0?
	je	.fine				; if so, we are done
	loop	.loop

	mov	al, 0
	call	flpydsk_control_motor		; turn OFF the motor
	inc	al
	jmp	.done
   .fine:
	mov	al, 0
	call	flpydsk_control_motor		; turn OFF the motor
   .done:
	pop	ecx
	pop	ax
	ret


;--------------------------
; Disable controller
;--------------------------
flpydsk_disable_controller:
	push	ax
	push	dx
	mov	al, 0
	mov	dx, FLPYDSK_DOR
	out	dx, al
	pop	dx
	pop	ax
	ret

;--------------------------
; Enable controller
;--------------------------
flpydsk_enable_controller:
	push	ax
	push	dx
	mov	al, FLPYDSK_DOR_MASK_RESET
	or	al, FLPYDSK_DOR_MASK_DMA
	mov	dx, FLPYDSK_DOR
	out	dx, al
	pop	dx
	pop	ax
	ret


;--------------------------
; Reset controller
;--------------------------
flpydsk_reset:
	push	ax
	push	dx

	call	flpydsk_disable_controller
	call	flpydsk_enable_controller
	call	flpydsk_wait_irq

	; send CHECK_INT/SENSE INTERRUPT command to all drives
	call flpydsk_check_int
	call flpydsk_check_int
	call flpydsk_check_int
	call flpydsk_check_int

	mov	al, 0				; transfer speed 500kb/s
	mov	dx, FLPYDSK_CTRL
	out	dx, al				; write to the configuation control

	mov	al, 3
	mov	bl, 16
	mov	ah, 240
	mov	bh, 1
	call	flpydsk_drive_data

	mov	al, [CurrentDrive]
	call	flpydsk_calibrate

	pop	dx
	pop	ax
	ret


;----------------------------------
; Install floppy driver
;----------------------------------
flpydsk_install:
	push	ax
	push	bx

	mov	al, 00001000b
	mov	dx, FLPYDSK_MSR			; check if fdd exists
	in	al, dx				; by reading the Main Status Register
	test	al, 10000000b			; check if data register is ready or not
	jz	.done
	test	al, 00001000b			; check if FDC is busy or not
	jnz	.done

   .cont:
	mov byte [fdd_exists], 0xFF		; note that fdd exists

	call	flpydsk_reset			; reset the fdc
	mov	al, 13				; set drive information
	mov	bl, 1
	mov	ah, 0x0F
	mov	bh, 1
	call	flpydsk_drive_data
   .done:
	pop	bx
	pop	ax
	ret


;---------------------------------
; Set current working drive
;	in: al = drive
;---------------------------------
flpydsk_set_working_drive:
	cmp	al, 3
	ja	.done
	mov	[CurrentDrive], al
   .done:
	ret


;--------------------------------
; Get current working drive
;	out: al = drive
;--------------------------------
flpydsk_get_working_drive:
	mov	al, [CurrentDrive]
	ret



;----------------------------------
; Seek to given track/cylinder
;	out: cf = 1	fine
;	     cf = 0	not-fine
;----------------------------------
flpydsk_seek:
	push	ax
	push	ecx
	cmp	[CurrentDrive], 3
	ja	.notFine

	mov	ecx, 10				; number of tries
   .loop:
	mov	al, FDC_CMD_SEEK
	call	flpydsk_send_command
	mov	al, [absoluteHead]
	shl	al, 2
	or	al, [CurrentDrive]
	call	flpydsk_send_command
	mov	al, [absoluteTrack]
	call	flpydsk_send_command

	mov	al, 0xFF			; make al != [absoluteTrack]
	call	flpydsk_wait_irq
	call	flpydsk_check_int		; returns al = cylinder
	cmp	al, [absoluteTrack]
	je	.fine
	loop	.loop
   .notFine:
	clc
	jmp	.done
   .fine:
	stc
   .done:
	pop	ecx
	pop	ax
	ret



;---------------------------------
; Read a sector using CHS
;---------------------------------

flpydsk_read_sector_CHS:
	push	ax
	push	cx

	call	flpydsk_seek			; seek to position to read sector
	jnc	.done				; there was an error

	mov	cx, 512-1
	call	dma_initialize_floppy

;-------------------------------------------------------------------------------------
	mov	al, FDC_CMD_READ_SECT
	or	al, FDC_CMD_EXT_MULTITRACK
	or	al, FDC_CMD_EXT_SKIP
	or	al, FDC_CMD_EXT_DENSITY
	call	flpydsk_send_command

	mov	al, [absoluteHead]
	shl	al, 2
	or	al, [CurrentDrive]
	call	flpydsk_send_command

	mov	al, [absoluteTrack]
	call	flpydsk_send_command
	mov	al, [absoluteHead]
	call	flpydsk_send_command
	mov	al, [absoluteSector]
	call	flpydsk_send_command
	mov	al, FLPYDSK_SECTOR_DTL_512	; for bytes per sector
	call	flpydsk_send_command

	mov	al, [absoluteSector]
	inc	al				; al = sector+1
	cmp	al, FLPY_SECTORS_PER_TRACK
	jb	.cont
	mov	al, FLPY_SECTORS_PER_TRACK
   .cont:
	call	flpydsk_send_command

	mov	al, FLPYDSK_GAP3_LENGTH_3_5
	call	flpydsk_send_command
	mov	al, 0xFF			; end of sending parameters
	call	flpydsk_send_command
;-------------------------------------------------------------------------------------

	call	flpydsk_wait_irq

	call	flpydsk_read_data		;  read status info, 7 bytes
	call	flpydsk_read_data
	call	flpydsk_read_data
	call	flpydsk_read_data		; I prefer this repetition with 7 statements
	call	flpydsk_read_data		; rather than doing a loop with 4 statements
	call	flpydsk_read_data
	call	flpydsk_read_data

	call	flpydsk_check_int		; let FDC know we handled interrupt
	stc
   .done:
	pop	cx
	pop	ax
	ret



;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector MOD sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

flpydsk_lba_to_chs:
	push	ax
	push	cx
	push	dx

	xor	dx, dx				; prepare dx:ax for operation
	div	WORD [fdd_SectorsPerTrack]	; calculate
	inc	dl				; adjust for sector 0
	mov	BYTE [absoluteSector], dl
	xor	dx, dx				; prepare dx:ax for operation
	div	WORD [fdd_HeadsPerCylinder]	; calculate
	mov	BYTE [absoluteHead], dl
	mov	BYTE [absoluteTrack], al

	pop	dx
	pop	cx
	pop	ax
	ret



;----------------------------------------------------
; Read a series of sectors (LBA)
;	in: cx = number of sectors to read
;	    ax = starting sector
;	    es:edi = buffer/memory to read to
;	out: CF = successful or not
;----------------------------------------------------

fdc_read_sectors:
	pusha

	cmp	[CurrentDrive], 3
	ja	.error

	push	ax
	mov	al, 1
	call	flpydsk_control_motor		; turn motor on
	pop	ax

	mov	bx, cx

   .nextSector:
	mov	ecx, 5				; number of tries
   .retry:
	call    flpydsk_lba_to_chs		; convert starting sector to CHS
	call	flpydsk_read_sector_CHS
	jc	.success
	call	flpydsk_reset			; reset fdc
	loop	.retry				; attempt to read again
	mov	al, 0
	call	flpydsk_control_motor		; turn motor off
   .error:
	clc
	jmp	.done

   .success:
	inc	ax				; queue next sector
	add	edi, 512
	dec	bx
	jnz	.nextSector			; read next sector

	mov	al, 0
	call	flpydsk_control_motor		; turn motor off
	stc					; everything went well
   .done:
	popa
	ret

