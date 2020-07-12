
;----------------------------------------------------------
;
; 8237 ISA Direct Memory Access Controller (DMAC)
;
;----------------------------------------------------------

;----------------------------------------------------------
; 2 DMACs, 32 bit master & 16bit slave each having 8 channels
;----------------------------------------------------------
	DMA_MAX_CHANNELS	equ	16
	DMA_CHANNELS_PER_DMAC	equ	8

;----------------------------------------------------------
; DMA0 address/count registers
;----------------------------------------------------------
	DMA0_CHAN0_ADDR_REG	equ	0
	DMA0_CHAN0_COUNT_REG	equ	1
	DMA0_CHAN1_ADDR_REG	equ	2
	DMA0_CHAN1_COUNT_REG	equ	3
	DMA0_CHAN2_ADDR_REG	equ	4
	DMA0_CHAN2_COUNT_REG	equ	5
	DMA0_CHAN3_ADDR_REG	equ	6
	DMA0_CHAN3_COUNT_REG	equ	7

;----------------------------------------------------------
; Generic DMA0 registers
;----------------------------------------------------------
	DMA0_STATUS_REG			equ	0x08
	DMA0_COMMAND_REG		equ	0x08
	DMA0_REQUEST_REG		equ	0x09
	DMA0_CHANMASK_REG		equ	0x0A
	DMA0_MODE_REG			equ	0x0B
	DMA0_CLEARBYTE_FLIPFLOP_REG	equ	0x0C
	DMA0_TEMP_REG			equ	0x0D
	DMA0_MASTER_CLEAR_REG		equ	0x0D
	DMA0_CLEAR_MASK_REG		equ	0x0E
	DMA0_MASK_REG			equ	0x0F

;----------------------------------------------------------
; DMA1 address/count registers
;----------------------------------------------------------
	DMA1_CHAN4_ADDR_REG	equ	0xC0
	DMA1_CHAN4_COUNT_REG	equ	0xC2
	DMA1_CHAN5_ADDR_REG	equ	0xC4
	DMA1_CHAN5_COUNT_REG	equ	0xC6
	DMA1_CHAN6_ADDR_REG	equ	0xC8
	DMA1_CHAN6_COUNT_REG	equ	0xCA
	DMA1_CHAN7_ADDR_REG	equ	0xCC
	DMA1_CHAN7_COUNT_REG	equ	0xCE

;----------------------------------------------------------
; DMA External Page Registers
;----------------------------------------------------------
	DMA_PAGE_EXTRA0			equ	0x80		; Also diagnostics port
	DMA_PAGE_CHAN2_ADDRBYTE2	equ	0x81
	DMA_PAGE_CHAN3_ADDRBYTE2	equ	0x82
	DMA_PAGE_CHAN1_ADDRBYTE2	equ	0x83
	DMA_PAGE_EXTRA1			equ	0x84
	DMA_PAGE_EXTRA2			equ	0x85
	DMA_PAGE_EXTRA3			equ	0x86
	DMA_PAGE_CHAN6_ADDRBYTE2	equ	0x87
	DMA_PAGE_CHAN7_ADDRBYTE2	equ	0x88
	DMA_PAGE_CHAN5_ADDRBYTE2	equ	0x89
	DMA_PAGE_EXTRA4			equ	0x8C
	DMA_PAGE_EXTRA5			equ	0x8D
	DMA_PAGE_EXTRA6			equ	0x8E
	DMA_PAGE_DRAM_REFRESH		equ	0x8F		;no longer used in new PCs

;----------------------------------------------------------
; Generic DMA1 registers
;----------------------------------------------------------
	DMA1_STATUS_REG			equ	0xD0
	DMA1_COMMAND_REG		equ	0xD0
	DMA1_REQUEST_REG		equ	0xD2
	DMA1_CHANMASK_REG		equ	0xD4
	DMA1_MODE_REG			equ	0xD6
	DMA1_CLEARBYTE_FLIPFLOP_REG	equ	0xD8
	DMA1_INTER_REG			equ	0xDA
	DMA1_UNMASK_ALL_REG		equ	0xDC
	DMA1_MASK_REG			equ	0xDE

;----------------------------------------------------------
; DMA mode bit mask (Non MASK Values can be ORed together)
;----------------------------------------------------------
	DMA_MODE_MASK_SEL		equ	3

	DMA_MODE_MASK_TRA		equ	0xC
	DMA_MODE_SELF_TEST		equ	0
	DMA_MODE_READ_TRANSFER		equ	4
	DMA_MODE_WRITE_TRANSFER		equ	8

	DMA_MODE_MASK_AUTO		equ	0x10
	DMA_MODE_MASK_IDEC		equ	0x20

	DMA_MODE_MASK			equ	0xC0
	DMA_MODE_TRANSFER_ON_DEMAND	equ	0
	DMA_MODE_TRANSFER_SINGLE	equ	0x40
	DMA_MODE_TRANSFER_BLOCK		equ	0x80
	DMA_MODE_TRANSFER_CASCADE	equ	0xC0

;----------------------------------------------------------
; DMA command register mask
;----------------------------------------------------------
	DMA_CMD_MASK_MEMTOMEM		equ	1
	DMA_CMD_MASK_CHAN0ADDRHOLD	equ	2
	DMA_CMD_MASK_ENABLE		equ	4
	DMA_CMD_MASK_TIMING		equ	8
	DMA_CMD_MASK_PRIORITY		equ	0x10
	DMA_CMD_MASK_WRITESEL		equ	0x20
	DMA_CMD_MASK_DREQ		equ	0x40
	DMA_CMD_MASK_DACK		equ	0x80


;----------------------------------------------------------
; masks a channel
;	in: al = channel
;----------------------------------------------------------
dma_mask_channel:
	push	ax
	push	cx
	mov	cl, al
	mov	al, 1

	cmp	cl, 4
	ja	.DMA1
	sub	cl, 1
	shl	al, cl
	out	DMA0_CHANMASK_REG, al		; outportb(DMA0_CHANMASK_REG, (1 << (channel-1)))
	jmp	.done
   .DMA1:
	sub	cl, 5
	shl	al, cl
	out	DMA1_CHANMASK_REG, al		; outportb(DMA1_CHANMASK_REG, (1 << (channel-5)))
   .done:
	pop	cx
	pop	ax
	ret


;----------------------------------------------------------
; unmasks a channel
;	in: al = channel
;----------------------------------------------------------
dma_unmask_channel:
	cmp	al, 4
	ja	.DMA1
	out	DMA0_CHANMASK_REG, al		; outportb(DMA0_CHANMASK_REG, channel)
	jmp	.done
   .DMA1:
	out	DMA1_CHANMASK_REG, al		; outportb(DMA1_CHANMASK_REG, channel)
   .done:
	ret


;----------------------------------------------------------
; unmasks all channels
;	in: al = dma (0 or 1)
;----------------------------------------------------------
dma_unmask_all:
	push	ax
;	cmp	al, 1
;	ja	.done
;	je	.DMA1
;	mov	al, 0xFF
;	out	DMA0_UNMASK_ALL_REG, al
;	jmp	.done
;   .DMA1:
	mov	al, 0xFF
	out	DMA1_UNMASK_ALL_REG, al		; it doesnt matter what is written to this register
;   .done:
	pop	ax
	ret


;----------------------------------------------------------
; reset
;	in: al = dma (0 or 1)
;----------------------------------------------------------
dma_reset:
	push	ax
;	cmp	al, 1
;	ja	.done
;	je	.DMA1
	mov	al, 0xFF
	out	DMA0_TEMP_REG, al
;	jmp	.done
;   .DMA1:
;	mov	al, 0xFF
;	out	DMA1_TEMP_REG, al			; it doesnt matter what is written to this register
;   .done:
	pop	ax
	ret


;----------------------------------------------------------
; resets flipflop
;	in: al = dma (0 or 1)
;----------------------------------------------------------
dma_reset_flipflop:
	cmp	al, 1
	ja	.done
	je	.DMA1
	mov	al, 0xFF
	out	DMA0_CLEARBYTE_FLIPFLOP_REG, al
	jmp	.done
   .DMA1:
	mov	al, 0xFF
	out	DMA1_CLEARBYTE_FLIPFLOP_REG, al		; it doesnt matter what is written to this register
   .done:
	ret


;----------------------------------------------------------
; sets the address of a channel
;	in: al = channel
;	    bl = low of address
;	    bh = high of address
;----------------------------------------------------------
dma_set_address:
	push	ax
	push	dx

	cmp	al, 7
	ja	.done

	cmp	al, 0
	jne	.is1
	mov	dx, DMA0_CHAN0_ADDR_REG
	jmp	.cont
   .is1:
	cmp	al, 1
	jne	.is2
	mov	dx, DMA0_CHAN1_ADDR_REG
	jmp	.cont
   .is2:
	cmp	al, 2
	jne	.is3
	mov	dx, DMA0_CHAN2_ADDR_REG
	jmp	.cont
   .is3:
	cmp	al, 3
	jne	.is4
	mov	dx, DMA0_CHAN3_ADDR_REG
	jmp	.cont
   .is4:
	cmp	al, 4
	jne	.is5
	mov	dx, DMA1_CHAN4_ADDR_REG
	jmp	.cont
   .is5:
	cmp	al, 5
	jne	.is6
	mov	dx, DMA1_CHAN5_ADDR_REG
	jmp	.cont
   .is6:
	cmp	al, 6
	jne	.is7
	mov	dx, DMA1_CHAN6_ADDR_REG
	jmp	.cont
   .is7:
	mov	dx, DMA1_CHAN7_ADDR_REG

   .cont:
	mov	al, bl
	out	dx, al					; write low of address
	mov	al, bh
	out	dx, al					; write high of address
   .done:
	pop	dx
	pop	ax
	ret


;----------------------------------------------------------
; sets the counter of a channel
;	in: al = channel
;	    bl = low of address
;	    bh = high of address
;----------------------------------------------------------
dma_set_count:
	push	ax
	push	dx

	cmp	al, 7
	ja	.done

	cmp	al, 0
	jne	.is1
	mov	dx, DMA0_CHAN0_COUNT_REG
	jmp	.cont
   .is1:
	cmp	al, 1
	jne	.is2
	mov	dx, DMA0_CHAN1_COUNT_REG
	jmp	.cont
   .is2:
	cmp	al, 2
	jne	.is3
	mov	dx, DMA0_CHAN2_COUNT_REG
	jmp	.cont
   .is3:
	cmp	al, 3
	jne	.is4
	mov	dx, DMA0_CHAN3_COUNT_REG
	jmp	.cont
   .is4:
	cmp	al, 4
	jne	.is5
	mov	dx, DMA1_CHAN4_COUNT_REG
	jmp	.cont
   .is5:
	cmp	al, 5
	jne	.is6
	mov	dx, DMA1_CHAN5_COUNT_REG
	jmp	.cont
   .is6:
	cmp	al, 6
	jne	.is7
	mov	dx, DMA1_CHAN6_COUNT_REG
	jmp	.cont
   .is7:
	mov	dx, DMA1_CHAN7_COUNT_REG

   .cont:
	mov	al, bl
	out	dx, al					; write low of address
	mov	al, bh
	out	dx, al					; write high of address
   .done:
	pop	dx
	pop	ax
	ret


;----------------------------------------------------------
; DMA Set Mode
;	in: al = channel
;	    ah = mode
;----------------------------------------------------------
dma_set_mode:
	push	ax
	push	bx
	push	dx

	cmp	al, 4
	jae	.DMA1
	mov	bl, 0
	mov	bh, al
	mov	dx, DMA0_MODE_REG
	jmp	.cont
   .DMA1:
	mov	bl, 1
	mov	bh, al
	sub	bh, 4
	mov	dx, DMA1_MODE_REG

   .cont:
	call	dma_mask_channel
	mov	al, bh
	or	al, ah					; ah = mode
	out	dx, al					; set mode
	mov	al, bl
	call	dma_unmask_all
   .done:
	pop	dx
	pop	bx
	pop	ax
	ret


;----------------------------------------------------------
; prepares channel for read
;	in: al = channel
;----------------------------------------------------------
dma_set_read:
	push	ax
	mov	ah, DMA_MODE_READ_TRANSFER
	or	ah, DMA_MODE_TRANSFER_SINGLE
	call	dma_set_mode
	pop	ax
	ret


;----------------------------------------------------------
; prepares channel for write
;	in: al = channel
;----------------------------------------------------------
dma_set_write:
	push	ax
	mov	ah, DMA_MODE_WRITE_TRANSFER
	or	ah, DMA_MODE_TRANSFER_SINGLE
	call	dma_set_mode
	pop	ax
	ret



;----------------------------------------------------------
; writes to an external page register
;	in: al = register
;	    ah = value
;----------------------------------------------------------
dma_set_external_page_register:
	push	ax
	push	dx

	cmp	al, 14
	ja	.done

	cmp	al, 1
	jne	.is2
	mov	dx, DMA_PAGE_CHAN1_ADDRBYTE2
	jmp	.cont
   .is2:
	cmp	al, 2
	jne	.is3
	mov	dx, DMA_PAGE_CHAN2_ADDRBYTE2
	jmp	.cont
   .is3:
	cmp	al, 3
	jne	.is4
	mov	dx, DMA_PAGE_CHAN3_ADDRBYTE2
	jmp	.cont
   .is4:
	cmp	al, 4
	jne	.is5
	jmp	.done					; nothing should ever write to register 4
   .is5:
	cmp	al, 5
	jne	.is6
	mov	dx, DMA_PAGE_CHAN5_ADDRBYTE2
	jmp	.cont
   .is6:
	cmp	al, 6
	jne	.is7
	mov	dx, DMA_PAGE_CHAN6_ADDRBYTE2
	jmp	.cont
   .is7:
	cmp	al, 6
	jne	.done
	mov	dx, DMA_PAGE_CHAN7_ADDRBYTE2
	jmp	.cont

   .cont:
	mov	al, ah
	out	dx, al					; write value to register
   .done:
	pop	dx
	pop	ax
	ret
