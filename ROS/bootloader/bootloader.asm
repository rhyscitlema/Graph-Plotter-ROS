;*********************************************
;
;    YEMELITC OPERATING SYSTEM
;
;    bootloader.asm
;
;*********************************************
; Content:
;    - BIOS Parameter Block
;    - Convert CHS to LBA
;    - Convert LBA to CHS
;    - Disk Read Sectors
;    - print function for strings
;    - Start bootloader
;    - Constants and Variables
;*********************************************



use16                               ; use 16 bits real mode

org 0                               ; segment regisers will be set later to 0x07C0

jmp start_bootloader


TEMP_MEMORY equ 0x0600              ; the size of this buffer is from '07C0:0600' to 'KRNLDR_LOCATION'

%include "./general_info.inc"       ; contains no code (only MACROS => names given to values)



;*********************************************
;    BIOS Parameter Block (BPB)
;*********************************************

; BPB begins 3 bytes from start. We did a far jump, which is 3 bytes in size.
; If a short jump is used (a branch), add a "nop" after it to offset the 3rd byte.

bpbOEM                  DB "YMLTC OS"
bpbBytesPerSector:      DW 512
bpbSectorsPerCluster:   DB 1
bpbReservedSectors:     DW 1
bpbNumberOfFATs:        DB 2
bpbRootEntries:         DW 224
bpbTotalSectors:        DW 2880             ; for 1.44 MB disk
bpbMedia:               DB 0xF0             ; fd = 2 sides 18 sectors
bpbSectorsPerFAT:       DW 9
bpbSectorsPerTrack:     DW 18
bpbHeadsPerCylinder:    DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber:          DB 0
bsUnused:               DB 0
bsExtBootSignature:     DB 0x29             ; extended boot signature
bsSerialNumber:         DD 0
bsVolumeLabel:          DB "YEMELITC_OS"
bsFileSystem:           DB "FAT12   "

datasector              dw 0
cluster                 dw 0

absoluteSector          db 0
absoluteHead            db 0
absoluteTrack           db 0



;************************************************
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************

CHS_to_LBA:
    sub    ax, 2                            ; zero base cluster number
    xor    cx, cx
    mov    cl, BYTE [bpbSectorsPerCluster]  ; convert byte to word
    mul    cx
    add    ax, WORD [datasector]            ; base data sector
    ret


;************************************************;
; Convert LBA to CHS
; AX = LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBA_to_CHS:
    xor    dx, dx                           ; prepare dx:ax for operation
    div    WORD [bpbSectorsPerTrack]        ; calculate
    inc    dl                               ; adjust for sector 0
    mov    BYTE [absoluteSector], dl
    xor    dx, dx                           ; prepare dx:ax for operation
    div    WORD [bpbHeadsPerCylinder]       ; calculate
    mov    BYTE [absoluteHead], dl
    mov    BYTE [absoluteTrack], al
    ret



;************************************************;
; Read a series of sectors
; CX = Number of sectors to read
; AX = Starting sector
; ES:BX = Buffer/Memory to store to
;************************************************;

ReadSectors:
    .MAIN:
        mov    di, 5                        ; five retries for error
    .SECTORLOOP:
        push    ax
        push    bx
        push    cx
        call    LBA_to_CHS                  ; convert starting sector to CHS

        mov    ah, 0x02                     ; BIOS read sector
        mov    al, 1                        ; read one sector
        mov    ch, BYTE [absoluteTrack]     ; track
        mov    cl, BYTE [absoluteSector]    ; sector
        mov    dh, BYTE [absoluteHead]      ; head
        mov    dl, BYTE [bsDriveNumber]     ; drive
        int    0x13                         ; invoke BIOS
        jnc    .SUCCESS                     ; test for read error

        xor    ax, ax                       ; BIOS reset disk
        int    0x13                         ; invoke BIOS
        dec    di                           ; decrement error counter
        pop    cx
        pop    bx
        pop    ax
        jnz    .SECTORLOOP                  ; attempt to read again
        int    0x18
    .SUCCESS:
        mov    si, msgProgress
        call   printstr
        pop    cx
        pop    bx
        pop    ax
        add    bx, WORD [bpbBytesPerSector] ; queue next buffer
        inc    ax                           ; queue next sector
        loop   .MAIN                        ; read next sector
        ret


;************************************************
;    Print a string
;    DS:SI = 0 terminated string
;************************************************
printstr:
    lodsb                           ; load next byte from string from si to al
    or    al, al                    ; check if al = null terminator
    jz    .done                     ; if yes, then we are done
    mov    ah, 0eh                  ; else, print the character
    int    10h
    jmp    printstr                 ; repeat until null terminator is found
   .done:
    ret                             ; ok done, so return


;*********************************************
;    BOOT LOADER ENTRY POINT
;*********************************************

start_bootloader:

    ;----------------------------------------------------
    ; code located at 0000:7C00, adjust segment registers
    ;----------------------------------------------------
    cli                             ; disable interrupts
    mov    ax, 0x07C0
    mov    ds, ax                   ; setup segement registers
    mov    es, ax                   ; to point to begining
    mov    fs, ax                   ; of our bootloader
    mov    gs, ax

    ;----------------------------------------------------
    ; create stack
    ;----------------------------------------------------
    xor    ax, ax
    mov    ss, ax
    mov    sp, 0x7C00               ; stack pointer starts at 7C00h

    sti                             ; restore interrupts


;----------------------------------------------------
; Load root directory table
;----------------------------------------------------

LOAD_ROOT:
    ; compute size of root directory and store in "cx"
    xor    cx, cx
    xor    dx, dx
    mov    ax, 32                               ; 32 byte directory entry
    mul    WORD [bpbRootEntries]                ; total size of directory
    div    WORD [bpbBytesPerSector]             ; sectors used by directory
    xchg   ax, cx                               ; exchange values

    ; compute location of root directory and store in "ax"
    mov    al, BYTE [bpbNumberOfFATs]           ; number of FATs
    mul    WORD [bpbSectorsPerFAT]              ; sectors used by FATs
    add    ax, WORD [bpbReservedSectors]        ; adjust for bootsector
    mov    WORD [datasector], ax                ; base of root directory
    add    WORD [datasector], cx

    ; read root directory into memory
    mov    bx, TEMP_MEMORY                      ; copy root directory table
    call   ReadSectors


;----------------------------------------------------
; Find kernelloader
;----------------------------------------------------

FIND_KRNLDR:
    ; browse root directory for binary image
    mov    ax, WORD [bpbRootEntries]            ; load loop counter
    mov    di, TEMP_MEMORY                      ; locate first root entry

    .LOOP:
        mov    cx, 11                           ; eleven character name
        mov    si, KrnldrName                   ; kernelloader name to find
        push   di
        rep cmpsb                               ; test for name match
        pop    di
        je     LOAD_FAT     ; found. now go to look for actual location of file
        add    di, 32       ; not found. queue next directory entry
        dec    ax
        jnz    .LOOP
        jmp    failure


;----------------------------------------------------
; Load FAT
;----------------------------------------------------

LOAD_FAT:
    ; save starting cluster of boot image
    mov    dx, WORD [di + 26]
    mov    WORD [cluster], dx                   ; file's first cluster

    ; compute size of FAT and store in "cx"
    xor    ax, ax
    mov    al, BYTE [bpbNumberOfFATs]           ; number of FATs
    mul    WORD [bpbSectorsPerFAT]              ; sectors used by FATs
    mov    cx, ax

    ; compute location of FAT and store in "ax"
    mov    ax, WORD [bpbReservedSectors]        ; adjust for bootsector

    ; read FAT into memory
    mov    bx, TEMP_MEMORY
    call   ReadSectors


;----------------------------------------------------
; Load kernelloader into memory
;----------------------------------------------------

    mov    ax, KRNLDR_LOCATION
    shr    ax, 4
    mov    es, ax                               ; destination for krnldr

load_krnldr:
    mov    ax, WORD [cluster]                   ; cluster to read
    call   CHS_to_LBA                           ; convert cluster to LBA
    xor    cx, cx
    mov    cl, BYTE [bpbSectorsPerCluster]      ; number of sectors to read
    xor    bx, bx                               ; buffer = updated_es:0x0
    call   ReadSectors

    shr    bx, 4                                ; bx = number of bytes read
    mov    ax, es                               ; divide bx by 0x10 to cope with 'segment'
    add    ax, bx
    mov    es, ax                               ; so es is updated

    ; compute next cluster
    mov    ax, WORD [cluster]                   ; identify current cluster
    mov    cx, ax                               ; copy current cluster
    mov    dx, ax                               ; copy current cluster
    shr    dx, 1                                ; divide by two
    add    cx, dx                               ; sum for (3/2)
    mov    bx, TEMP_MEMORY                      ; location of FAT in memory
    add    bx, cx                               ; index into FAT
    mov    dx, WORD [bx]                        ; read two bytes from FAT
    test   ax, 0x0001
    jnz    .odd_cluster

   .even_cluster:
    and    dx, 0x0FFF                           ; take low twelve bits
    jmp    .done
   .odd_cluster:
    shr    dx, 4                                ; take high twelve bits
   .done:
    mov    WORD [cluster], dx                   ; store new cluster
    cmp    dx, 0x0FF8                           ; test for end of file
    jb     load_krnldr


done:
    mov    si, msgStartKrnldr
    call   printstr

    mov    ax, KRNLDR_LOCATION
    shr    ax, 4

    push    ax
    push    WORD 0                          ; retf pops IP/EIP, then pops CS,
    retf                                    ; and then ... (see documentation)                        

    ; the above three lines of code can be replaced by one line:
    ; jmp KRNLDR_LOCATION_SEGMENT:0x0

failure:
    mov    si, msgFailure
    call   printstr
    mov    ah, 0x00
    int    0x16                             ; wait for keypress
    int    0x19                             ; warm boot computer



;*********************************************
; Constants and Variables
;*********************************************

KrnldrName      db "KRNLDR  SYS"              ; file name must be 11 characters
msgStartKrnldr  db 13, 10, 13, 10, "Starting Kernel loader ", 13, 10, 0
msgProgress     db ".", 0
msgFailure      db 13, 10, 13, 10, "ERROR LOADING FILE KRNLDR.SYS"
                db 13, 10, 13, 10, "Press Any Key to Reboot", 13, 10, 0


; END
    times 510-($-$$) db 0                   ; pad with 0s until file is 510 bytes
    dw 0xAA55                               ; write bootloader signature

