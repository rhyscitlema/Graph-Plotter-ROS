;*********************************************
;	YEMELITC-OS
;
;	Fat12.inc
;
;*********************************************

	%include "include/Floppy16.inc"				; floppy driver



;*******************************************
; LoadRoot ()
;	- Load Root Directory Table
;*******************************************

LoadRoot:

	pusha							; store registers

	; compute size of root directory and store in "cx"
	xor	cx, cx
	xor	dx, dx
	mov	ax, 32						; 32 byte directory entry
	mul	WORD [bpbRootEntries]				; total size of directory
	div	WORD [bpbBytesPerSector]			; sectors used by directory
	xchg	ax, cx						; exchange values

	; compute location of root directory and store in "ax"
	mov	al, BYTE [bpbNumberOfFATs]			; number of FATs
	mul	WORD [bpbSectorsPerFAT]				; sectors used by FATs
	add	ax, WORD [bpbReservedSectors]			; adjust for bootsector
	mov	WORD [datasector], ax				; base of root directory
	add	WORD [datasector], cx

	; read root directory into memory
	mov	bx, TEMP_MEMORY					; copy root directory table
	call	ReadSectors

	popa							; restore registers and return
	ret



;*******************************************
; LoadFAT ()
;
;	Parm/ ES:DI => Root Directory Table
;*******************************************

LoadFAT:

	pusha							; store registers

	; compute size of FAT and store in "cx"
	xor	ax, ax
	mov	al, BYTE [bpbNumberOfFATs]			; number of FATs
	mul	WORD [bpbSectorsPerFAT]				; sectors used by FATs
	mov	cx, ax

	; compute location of FAT and store in "ax"
	mov	ax, WORD [bpbReservedSectors]			; adjust for bootsector

	; read FAT into memory
	mov	bx, TEMP_MEMORY
	call	ReadSectors

	popa							; restore registers and return
	ret



;*******************************************
; FindFile ()
;	- Search for filename in root table
;
; parm/ DS:SI => File name
; ret/ AX => File index number in directory table. clc if error
;*******************************************

FindFile:

	push	cx						; store registers
	push	dx
	push	bx
	mov	bx, si						; copy filename for later

	; browse root directory for binary image
	mov	ax, WORD [bpbRootEntries]			; load loop counter
	mov	di, TEMP_MEMORY					; locate first root entry
	cld							; clear direction flag
.LOOP:
	mov	cx, 11						; eleven character name. Image name is in SI
	mov	si, bx						; image name is in BX
 	push	di
	rep  cmpsb						; test for entry match
	pop	di
	je	 .found
	add	di, 32						; queue next directory entry
	dec	ax
	jnz	.LOOP
.notFound:
	clc							; set error notifier
	jmp	.done
.found:
	mov	ax, cx						; ax = entry of file
	stc
.done:
	pop	bx						; restore registers and return
	pop	dx
	pop	cx
	ret



;*******************************************
; LoadFile ()
;	- Load file
; parm/ SI => File to load
; ret/ AX => clc on error, stc on success
; ret/ CX => number of sectors read
;*******************************************

LoadFile:

	call	LoadRoot			; Load root directory table

	push	es
	xor	ecx, ecx			; size of file in sectors
	push	ecx

.FIND_FILE:

	call	FindFile			; find our file. ES:SI contains our filename
	jnc	.done

.LOAD_IMAGE_PRE:

	; get starting cluster

	mov	dx, WORD [di + 26]		; DI points to file entry in root directory table. Refrence the table...
	mov	WORD [cluster], dx		; file's first cluster
	call	LoadFAT

	; read file into memory bx:0
	mov	es, bx


.LOAD_FILE:

	; load the cluster
	mov	ax, WORD [cluster]		; cluster to read
	call	ClusterLBA
	xor	cx, cx
	mov	cl, BYTE [bpbSectorsPerCluster]
	xor	bx, bx				; buffer = updated_es:0x0
	call	ReadSectors

	shr	bx, 4				; bx = number of bytes read
	mov	ax, es				; divide bx by 0x10 to cope with 'segment'
	add	ax, bx
	mov	es, ax				; so es is updated

	pop	ecx
	inc	ecx				; add one more sector to counter
	push	ecx

	; compute next cluster
	mov	ax, WORD [cluster]		; identify current cluster
	mov	cx, ax				; copy current cluster
	mov	dx, ax
	shr	dx, 1				; divide by two
	add	cx, dx				; sum for (3/2)
	mov	bx, TEMP_MEMORY			;location of fat in memory
	add	bx, cx
	mov	dx, WORD [bx]
	test	ax, 0x0001			; test for odd or even cluster
	jnz	.odd_cluster

   .even_cluster:
	and	dx, 0x0FFF		; take low twelve bits
	jmp	.finish
   .odd_cluster:
	shr	dx, 4			; take high twelve bits
   .finish:
	mov	WORD [cluster], dx	; store new cluster
	cmp	dx, 0x0FF0		; test for end of file
	jb	.LOAD_FILE
.success:
	stc
.done:
	pop	ecx
	pop	es
	ret

