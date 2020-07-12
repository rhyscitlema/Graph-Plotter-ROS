;*********************************************************************************
;
;   YEMELITC OPERATING SYSTEM
;
;   general_info.inc
;
;   This file contains only 'equ' statements. There is no code.
;*********************************************************************************


KRNLDR_LOCATION         equ     0x00B000    ; 0x00B000 <= KRNLDR space <= 0x020000 --> 86016 bytes
FIRST_KERNEL_LOCATION   equ     0x020000
FINAL_KERNEL_LOCATION   equ     0x100000    ; IMPORTANT: the kernel must also agree to this value
;EXTRA_FILE_LOCATION    equ     0x040000


;------------------------
; stored information
;------------------------

GDT_LOCATION            equ    0x0500       ; total size = 0x800
IDT_LOCATION            equ    0x0D00       ; total size = 0x800

vbeModeInfo                     equ    0x1500               ;    total size = 0x100 = 256 bytes
vbeModeInfo_ModeAttributes      equ    vbeModeInfo + 0      ; word : mode attributes (flags)
vbeModeInfo_WinAAttributes      equ    vbeModeInfo + 2      ; byte : window A attributes
vbeModeInfo_WinBAttributes      equ    vbeModeInfo + 3      ; byte : window B attributes
vbeModeInfo_WinGranularity      equ    vbeModeInfo + 4      ; word : window granularity
vbeModeInfo_WinSize             equ    vbeModeInfo + 6      ; word : window size
vbeModeInfo_WinASegment         equ    vbeModeInfo + 8      ; word : window A start segment
vbeModeInfo_WinBSegment         equ    vbeModeInfo +10      ; word : window B start segment
vbeModeInfo_WinFuncPtr          equ    vbeModeInfo +12      ; dword: pointer to window function
vbeModeInfo_BytesPerScanLine    equ    vbeModeInfo +16      ; word : bytes per scan line
vbeModeInfo_XResolution         equ    vbeModeInfo +18      ; word : horizontal resolution in pixels or chars
vbeModeInfo_YResolution         equ    vbeModeInfo +20      ; word : vertical resolution in pixels or chars
vbeModeInfo_XCharSize           equ    vbeModeInfo +22      ; byte : character cell width in pixels
vbeModeInfo_YCharSize           equ    vbeModeInfo +23      ; byte : character cell height in pixels
vbeModeInfo_NumberOfPlanes      equ    vbeModeInfo +24      ; byte : number of memory planes
vbeModeInfo_BitsPerPixel        equ    vbeModeInfo +25      ; byte : bits per pixel
vbeModeInfo_NumberOfBanks       equ    vbeModeInfo +26      ; byte : number of banks
vbeModeInfo_MemoryModel         equ    vbeModeInfo +27      ; byte : memory model type
vbeModeInfo_BankSize            equ    vbeModeInfo +28      ; byte : bank size in KB
vbeModeInfo_NumberOfImagePages  equ    vbeModeInfo +29      ; byte : number of images
vbeModeInfo__Reserved1          equ    vbeModeInfo +30      ; byte : reserved for page function
vbeModeInfo_RedMaskSize         equ    vbeModeInfo +31      ; byte : size of direct colour red mask in bits
vbeModeInfo_RedFieldPosition    equ    vbeModeInfo +32      ; byte : bit position of lsb of red mask
vbeModeInfo_GreenMaskSize       equ    vbeModeInfo +33      ; byte : size of direct colour green mask in bits
vbeModeInfo_GreenFieldPosition  equ    vbeModeInfo +34      ; byte : bit position of lsb of green mask
vbeModeInfo_BlueMaskSize        equ    vbeModeInfo +35      ; byte : size of direct colour blue mask in bits
vbeModeInfo_BlueFieldPosition   equ    vbeModeInfo +36      ; byte : bit position of lsb of blue mask
vbeModeInfo_RsvdMaskSize        equ    vbeModeInfo +37      ; byte : size of direct colour reserved mask in bits
vbeModeInfo_RsvdFieldPosition   equ    vbeModeInfo +38      ; byte : bit position of lsb of reserved mask
vbeModeInfo_DirectColorModeInfo equ    vbeModeInfo +39      ; byte : direct colour mode attributes
vbeModeInfo_PhysBasePtr         equ    vbeModeInfo +40      ; dword: physical address for flat frame buffer
vbeModeInfo_OffScreenMemOffset  equ    vbeModeInfo +44      ; dword: pointer to start of off screen memory
vbeModeInfo_OffScreenMemSize    equ    vbeModeInfo +48      ; word : amount of off screen memory in 1k units
vbeModeInfo__Reserved2          equ    vbeModeInfo +50      ;    remainder of ModeInfoBlock = 206 bytes

VBE_VERSION     equ     0x1600  ; size = 4 bytes


