
;*********************************************
;	YEMELITCOS
;
;	fat12_fdd.inc
;
;*********************************************
; Content: (Last checked: 21 Nov 2012)
;	-
;	-
;*********************************************

	include "include/driver/fdc.inc"			; floppy disk controller driver

	fdd_ReservedSectors: 	DW 1
	fdd_SectorsPerCluster	DB 1
	fdd_NumberOfFATs: 	DB 2
	fdd_RootEntries: 	DW 224
	fdd_SectorsPerFAT: 	DW 9

	datasector	dw 0
	cluster		dw 0

	TEMP_MEMORY	dd KERNEL_TEMP_BUFFER


;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
	push	cx
	sub	ax, 2					; zero base cluster number
	xor	cx, cx
	mov	cl, [fdd_SectorsPerCluster]		; convert byte to word
	mul	cx
	add	ax, [datasector]			; base data sector
	pop	cx
	ret



;*******************************************
; Load Root Directory Table
;*******************************************

LoadRoot:
	pusha

	; compute size of root directory and store in "cx"
	xor	ecx, ecx
	xor	edx, edx
	mov	eax, 32						; 32 bytes directory entry
	mul	WORD [fdd_RootEntries]				; total size of directory
	div	WORD [fdd_BytesPerSector]			; fdc.inc
	xchg	ax, cx						; exchange values

	; compute location of root directory and store in "ax"
	xor	ax, ax
	mov	al, [fdd_NumberOfFATs]				; number of FATs
	mul	WORD [fdd_SectorsPerFAT]			; sectors used by FATs
	add	ax, [fdd_ReservedSectors]			; adjust for bootsector
	mov	[datasector], ax				; base of root directory
	add	[datasector], cx

	; read root directory into memory
	mov	edi, [TEMP_MEMORY]
	call	fdc_read_sectors				; fdc.inc

	popa							; restore registers and return
	ret


;***********************************************************************
; FindFile - Search for filename in root table
;	in: DS:ESI = file name
;	out: eax = points to file entry in root directory table
;***********************************************************************

FindFile:
	push	ebx
	push	ecx					; store registers
	push	edx
	push	esi
	push	edi

	cmp byte [fdd_exists], 0xFF
	jne	.notFound

	call	LoadRoot				; Load root directory table

	; browse root directory for binary image
	mov	ebx, esi				; save filename for later
	movzx	ecx, WORD [fdd_RootEntries]		; load loop counter
	mov	edi, [TEMP_MEMORY]			; locate first root entry
	cld						; clear direction flag

.loop:
	mov	esi, ebx				; File name is in EBX
 	push	edi					; save current directory entry

   .checkName:
	mov	al, [esi]
	cmp	al, '.'
	je	.checkExtension
	cmpsb						; cmp [esi], [edi]
	jne	.nextEntry
	jmp	.checkName
   .checkExtension:
	pop	edi					; recover the initial value of edi
	push	edi
	add	edi, 8					; get to the extension zone
	inc	esi					; skip '.'
	cmpsw						; cmp [esi], [edi]
	jne	.nextEntry
	cmpsb						; cmp last byte of name
	jne	.nextEntry
	cmp byte [esi], 0
	je	.found
	cmp byte [esi], ' '				; check if end of string is 0 or ' '
	je	.found

   .nextEntry:
	pop	edi
	add	edi, 32					; queue next directory entry
	loop	.loop

.notFound:
	clc						; set error code
	jmp	.done
.found:
	pop	eax					; = pop edi = file entry in root directory table
	stc
.done:
	pop	edi
	pop	esi
	pop	edx
	pop	ecx
	pop	ebx					; restore registers and return
	ret



;*******************************************
; Load File Allocation Table
;*******************************************

LoadFAT:
	pusha

	; compute size of FAT and store in "cx"
	xor	ax, ax
	mov	al, [fdd_NumberOfFATs]
	mul	WORD [fdd_SectorsPerFAT]
	mov	cx, ax

	; compute location of FAT and store in "ax"
	mov	ax, WORD [fdd_ReservedSectors]		; adjust for bootsector

	; read FAT into memory
	mov	edi, [TEMP_MEMORY]
	call	fdc_read_sectors			; fdc.inc

	popa						; restore registers and return
	ret



;*******************************************
; LoadFile
;	in: ESI = file to load
;	in: EDI = where to load file
;	out: eax = number of bytes read
;*******************************************

LoadFile:
	pusha
	mov word [.numberOfBytesRead], 0

	call	FindFile				; find our file. ES:ESI contains our filename
	jnc	.done

.load_fat:
	; get starting cluster
	mov	dx, WORD [eax + 26]			; eax points to file entry in root directory table
	mov	WORD [cluster], dx			; get file's first cluster
	call	LoadFAT

.load_file:

	; load the cluster
	mov	ax, WORD [cluster]			; cluster to read
	call	ClusterLBA
	xor	cx, cx
	mov	cl, BYTE [fdd_SectorsPerCluster]
	call	fdc_read_sectors

	xor	eax, eax
	mov	ax, [fdd_BytesPerSector]
	mul	cx
	add	edi, eax				; update location to load file
	add	[.numberOfBytesRead], eax

	; compute next cluster
	mov	ax, WORD [cluster]			; identify current cluster
	mov	cx, ax					; copy current cluster
	mov	dx, ax
	shr	dx, 1					; divide by two
	add	cx, dx					; sum for (3/2)

	mov	esi, [TEMP_MEMORY]			;location of fat in memory
	add	esi, ecx
	mov	dx, WORD [esi]				; dx = new cluster
	test	ax, 0x0001				; test for odd or even cluster
	jnz	.odd_cluster

   .even_cluster:
	and	dx, 0x0FFF				; take low twelve bits
	jmp	.cont
   .odd_cluster:
	shr	dx, 4					; take high twelve bits
   .cont:
	mov	WORD [cluster], dx			; store new cluster
	cmp	dx, 0x0FF0				; test for end of file
	jb	.load_file

	stc						; everything went well
.done:
	popa
	mov	eax, [.numberOfBytesRead]
	ret

   .numberOfBytesRead dd 0

