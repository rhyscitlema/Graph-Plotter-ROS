;*********************************************
;    YEMELITC-OS
;
;    Floppy16.inc (Floppy Driver)
;
;*********************************************


;*********************************************
;    BIOS Parameter Block
;*********************************************

bpbOEM                  DB "YMLTC OS"
bpbBytesPerSector:      DW 512
bpbSectorsPerCluster:   DB 1
bpbReservedSectors:     DW 1
bpbNumberOfFATs:        DB 2
bpbRootEntries:         DW 224
bpbTotalSectors:        DW 2880        ;for 1.44 MB disk
bpbMedia:               DB 0xF0        ;fd = 2 sides 18 sectors
bpbSectorsPerFAT:       DW 9
bpbSectorsPerTrack:     DW 18
bpbHeadsPerCylinder:    DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber:          DB 0
bsUnused:               DB 0
bsExtBootSignature:     DB 0x29        ;extended boot signature
bsSerialNumber:         DD 0
bsVolumeLabel:          DB "YEMELITCOS "
bsFileSystem:           DB "FAT12   "

datasector              dw 0
cluster                 dw 0

absoluteSector          db 0
absoluteHead            db 0
absoluteTrack           db 0



;************************************************;
; Convert CHS to LBA
; LBA = (cluster - 2) * sectors per cluster
;************************************************;

ClusterLBA:
    sub     ax, 2                           ; zero base cluster number
    xor     cx, cx
    mov     cl, BYTE [bpbSectorsPerCluster] ; convert byte to word
    mul     cx
    add     ax, WORD [datasector]           ; base data sector
    ret


;************************************************;
; Convert LBA to CHS
; AX=>LBA Address to convert
;
; absolute sector = (logical sector / sectors per track) + 1
; absolute head   = (logical sector / sectors per track) MOD number of heads
; absolute track  = logical sector / (sectors per track * number of heads)
;
;************************************************;

LBACHS:
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
; CX=>Number of sectors to read
; AX=>Starting sector
; ES:BX=>Buffer/Memory to read to
;************************************************;

ReadSectors:
  .MAIN:
    mov     di, 5                           ; five retries for error
  .SECTORLOOP:
    push    ax
    push    bx
    push    cx
    call    LBACHS                          ; convert starting sector to CHS

    mov     ah, 0x02                        ; BIOS read sector
    mov     al, 1                           ; read one sector
    mov     ch, BYTE [absoluteTrack]        ; track
    mov     cl, BYTE [absoluteSector]       ; sector
    mov     dh, BYTE [absoluteHead]         ; head
    mov     dl, BYTE [bsDriveNumber]        ; drive
    int     0x13                            ; invoke BIOS
    jnc     .SUCCESS                        ; test for read error

    xor     ax, ax                          ; BIOS reset disk
    int     0x13                            ; invoke BIOS
    dec     di                              ; decrement error counter
    pop     cx
    pop     bx
    pop     ax
    jnz     .SECTORLOOP                     ; attempt to read again
    int     0x18
  .SUCCESS:
    mov     si, msgProgress
    call    printstr
    pop     cx
    pop     bx
    pop     ax
    add     bx, WORD [bpbBytesPerSector]    ; queue next buffer
    inc     ax                              ; queue next sector
    loop    .MAIN                           ; read next sector
    ret

msgProgress db ".", 0

