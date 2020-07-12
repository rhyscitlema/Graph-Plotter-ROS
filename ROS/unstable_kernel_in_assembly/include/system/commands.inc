
;----------------------------------------------------------
; Command table
; Mainly for the Command Line
;----------------------------------------------------------

commands:
    db    "help", 0
    db    "version", 0
    db    "about", 0
    db    "time", 0
    db    "date", 0
    db    "image", 0
    db    "suspend", 0
    db    "vbemodeinfo", 0
    db    "cat", 0
    db    "print", 0
    db    "testfunction", 0
    db    "cls", 0
    db    "reboot", 0
    db    0xFF              ; end of command table

call_table:
    dd    help              ; see below
    dd    version           ; see below
    dd    about             ; see below
    dd    print_time        ; cmos.inc
    dd    print_date        ; cmos.inc
    dd    displayBMPImage   ; see below
    dd    suspend_cmdline   ; see below
    dd    printVBEModeInfo  ; vbemodeinfo.inc
    dd    cat_cmdline       ; see below
    dd    print_cmdline     ; see below
    dd    testfunction      ; see below
    dd    clrscr            ; textmode.inc
    dd    reboot            ; kernel.asm


;-------------------------
; Help message
;-------------------------
    yemelitcos_help_str    db    \
        "   List of commands:",13,\
        " help              ? display this help message",13,\
        " version           ? display yemelitcos version",13,\
        " about             ? information about yemelitcos",13,\
        " time              ? print current time",13,\
        " date              ? print current date",13,\
        " image             ? display image loaded by krnldr",13,\
        " suspend           ? suspend for the given time in seconds",13,\
        " vbemodeinfo       ? print VBE Mode Info",13,\
        " cat               ? concatenate text file(s)",13,\
        " print             ? print content of file",13,\
        " testfunction      ? run the recently developed function",13,\
        " cls               ? clear screen and whole text mode",13,\
        " reboot            ? restart computer", 13,\
        13, "To scroll, press the character ` (scroll down) or ~ (scroll up)", 13,\
        13, "Note, only USA-Keyboard style (QWERTY) is supported for now!", 13, \
        13, 0
help:
    push    esi
    mov     esi, yemelitcos_help_str
    call    printstr
    pop     esi
    ret


;-----------------------------------------
; Display yemelitcos version
;-----------------------------------------
    yemelitcos_version_str    db    \
        "   YEMELITCOS version 0.0 - 2012", 13,\
        "   by Yemeli Tasse Cyrille", 13, 0
version:
    push    esi
    mov     esi, yemelitcos_version_str
    call    printstr
    pop     esi
    ret


;----------------------------------------
; Information about yemelitcos
;----------------------------------------
    yemelitcos_about_str    db    \
        "   About YEMILITCOS:",13,\
        "... (not yet available) ...", 13, 0
about:
    push    esi
    mov     esi, yemelitcos_about_str
    call    printstr
    pop     esi
    ret


;---------------------------------------------------
; Run the recently implemented function/program
;---------------------------------------------------

testfunction:
    pusha

    mov     esi, 0x50000            ; first copy the program to PROGRAM_LOCATION
    mov     edi, PROGRAM_LOCATION
    mov     ecx, 0x10000            ; assumming size of file < 0x40000 = 262144 bytes
    rep movsd

;    call    PROGRAM_LOCATION
    jmp     load_program.PE_Program ; programs.inc

    popa
    ret



;------------------------------------------------------------
; suspend command line
;    in: scanner = position in cmd_buffer after 'suspend'
; Expecting a number as from edi position
;------------------------------------------------------------

    threedots    db    " ... ", 0
suspend_cmdline:
    push    eax
    push    ecx
    push    esi

    call    get_next_int
    jnc     .invalidNumber          ; eax = next int = number of seconds
    mov     esi, threedots
    call    printstr
    call    newline
    mov     ecx, ONE_SECOND         ; delay for one second
  .loop:
    call    delay
    dec     eax
    jnz     .loop
  .done:
    pop     esi
    pop     ecx
    pop     eax
    ret

  .invalidNumber:
    mov     esi, .invalidNumberMsg
    call    printstr
    jmp     .done

  .invalidNumberMsg    db    "   invalid input number", 13, 0



;-------------------------------------------------------------
; cat inputted file
;    in: scanner = position in cmd_buffer after 'cat'
; Expecting a file name as from edi position
;-------------------------------------------------------------

cat_cmdline:
    push    ax
    push    esi
    push    edi

    mov     al, [fdd_exists]
    call    printhex1
    call    newline

    mov     edi, .filename
    call    get_next_word
    jnc     .noFile
    mov     esi, .filename
    mov     edi, PROGRAM_LOCATION   ; general_info.inc
    call    LoadFile                ; fat12_fdd.inc
    jnc     .fileNotFound

    mov     esi, PROGRAM_LOCATION
    call    printstr                ; finally print content of text file
  .done:
    pop     edi
    pop     esi
    pop     ax
    ret

  .noFile:
    mov     esi, .noFileMsg
    call    printstr
    jmp     .done
  .fileNotFound:
    mov     esi, .fileNotFoundMsg
    call    printstr
    jmp     .done

  .filename: times 20 db 0
  .noFileMsg        db "   no/invalid input", 13, 0
  .fileNotFoundMsg  db "   Error: file could not be loaded", 13, 0



;---------------------------------------------------
; Print bytes content of file
;    in: scanner = position in cmd_buffer after 'cat'
;---------------------------------------------------

print_cmdline:
    pusha

    mov     ch, 0                   ; to check if a file was loaded
    mov     dh, 0                   ; to check if a command was found
  .getInput:
    mov     edi, .inputString
    call    get_next_word           ; stdio.inc
    jnc     .invalidInput

    mov    esi, .inputString
    cmp byte [esi], '-'
    jne    .isFile
    cmp    dh, 0                    ; check if a command was already found
    jne    .cmdError                ; if so then assume a command error

  .get_help:
    mov    edi, .helpCmd
    call    strcmp
    jc    .help
  .get_text:
    mov    edi, .textCmd
    call    strcmp
    jnc    .get_chars
    mov    dh, 1                    ; notify that -t command is found
    jmp    .getInput
  .get_chars:
    mov    edi, .charsCmd
    call    strcmp
    jnc    .get_bytes
    mov    dh, 2                    ; notify that -c command is found
    jmp    .getInput
  .get_bytes:
    mov    edi, .bytesCmd
    call    strcmp
    jnc    .cmdError
    mov    dh, 3                    ; notify that -b command is found
    jmp    .getInput

  .isFile:
    cmp    ch, 0                    ; check if a file was already found and loaded
    jne    .nowPrint                ; if so then assume that the input string is the file name

    mov    esi, .inputString
    mov    edi, PROGRAM_LOCATION    ; general_info.inc
    call    LoadFile                ; fat12_fdd.inc
    jnc    .fileNotFound
    mov    ch, 0xFF                 ; notify that file was loaded
    jmp    .getInput

  .nowPrint:
    mov    ecx, 3000                ; assumed size of file (for now)
    mov    esi, PROGRAM_LOCATION
    cmp    dh, 3                    ; if command is -b
    je    .bytes
    cmp    dh, 2                    ; if command is -c
    je    .chars

  .text:                            ; default command is -t
    call    printstr                ; print file as a text file
    jmp    .done
  .chars:
    lodsb
    call    printchr                ; print every character in the file
    loop    .chars
    jmp    .done
  .bytes:
    lodsb
    call    printhex1               ; print every byte number in the file
    cmp    al, 0x74
    jne    .ccc
    call    newline
  .ccc:
    loop    .bytes

  .done:
    popa
    ret

  .invalidInput:
    cmp    ch, 0                    ; first check if a file was found and loaded
    jne    .nowPrint
    mov    esi, .invalidInputMsg
    call    printstr
    jmp    .done
  .cmdError:
    mov    esi, .cmdErrorMsg
    call    printstr
    jmp    .done
  .fileNotFound:
    mov    esi, .fileNotFoundMsg
    call    printstr
    jmp    .done
  .help:
    mov    esi, .helpMsg
    call    printstr
    jmp    .done

  .inputString: times 20 db 0                               ; can be a file or a command
  .invalidInputMsg      db "   no/invalid input", 13, 0
  .fileNotFoundMsg      db "   Error: file could not be loaded", 13, 0
  .cmdErrorMsg          db "   Error: invalid command entry", 13, 0
  .helpMsg      db "   Usage: print [command] fileName",13,\
                "   Commands:",13,\
                "     -t (default)  ? print file as a text file",13,\
                "     -c            ? print characters content",13,\
                "     -b            ? print bytes content",13,\
                "     -h            ? display this help message",13, 0
  .helpCmd      db "-h", 0
  .textCmd      db "-t", 0
  .charsCmd     db "-c", 0
  .bytesCmd     db "-b", 0




;-----------------------------------------------------
; Display the bmp image loaded by kernelloader
;-----------------------------------------------------
displayBMPImage:
    pusha

    mov    edi, BMP_Header
    mov    esi, 0x40000                ; known from kernelloader
    mov    ecx, 27
    rep    movsw

    cmp word [BMP_Header.signature], 0x4D42
    jne    .error
    cmp word [BMP_Header.numberOfPlanes], 1
    jne    .error
    cmp word [BMP_Header.bitsPerPixel], 24
    jne    .error

    mov    edi, SCREEN_BUFFER
    mov    eax, 0x02000000
    call    computeScreenOffset
    add    edi, ebx
    mov    ecx, [BMP_Header.YSize]
    mov    esi, 0x40000
    add    esi, [BMP_Header.offsetToStartOfImageData]

    movzx    ebp, word [vbeModeInfo_BytesPerScanLine]
.loop1:
    push    ecx
    push    edi
    mov     ecx, [BMP_Header.XSize]
  .loop2:
    lodsd
    dec    esi
    and    eax, 0x00FFFFFF
    call    putPixel
    dec    ecx
    jnz    .loop2
    pop    edi
    sub    edi, ebp
    pop    ecx
    dec    ecx
    jnz    .loop1

    call    displayWholeScreen            ; display finally
    call    getc
    call    displayWholeTextMode
  .done:
    popa
    ret

  .error:
    mov     esi, .imageError
    call    printstr
    jmp     .done
  .imageError        db "   Error: image file is not valid", 13, 0

BMP_Header:
    .signature:                 dw 0        ; must be 0x4D42
    .file_size:                 dd 0
    .reserved:                  dd 0
    .offsetToStartOfImageData:  dd 0
    .sizeOfBitmapInfoHeader:    dd 0        ; must be 40
    .XSize:                     dd 0
    .YSize:                     dd 0
    .numberOfPlanes:            dw 0        ; must be 1
    .bitsPerPixel:              dw 0
    .compressionType:           dd 0
    .sizeOfImageData:           dd 0        ; includes padding
    .horizontalResolutionPPM:   dd 0
    .verticalResolutionPPM:     dd 0
    .numberOfImageColors:       dd 0
    .numberOfImportantColors:   dd 0


