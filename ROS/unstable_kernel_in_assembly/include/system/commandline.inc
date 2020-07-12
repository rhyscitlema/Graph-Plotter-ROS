

;----------------------------------------------
;   Constants, Variables and Arrays
;----------------------------------------------

welcome_to_cmdline  db  "Welcome to YEMELITCOS.",13, \
                        "You are in the Command Line.",13, \
                        "Enter 'help' for more information.",13, 0

prompt_yemelitcos   db      "yemelitcos->", 0

BufS    equ     256                     ; command buffer size
BufEtr  equ     100                     ; number of stored entries

head_node       dd dlinked_list         ; head_node is always an empty string
current_node    dd dlinked_list
head_current    db 1

dlinked_list:   times   ((BufS+8)*BufEtr)   db 0    ; double linked list to stroe entries
cmd_buffer:     times   BufS    db 0



;-------------------------------------
;   Start of Command Line
; in: nothing
; out: nothing
;-------------------------------------

start_commandline:

    mov     byte [kbd_status], 0        ; reset LEDs to 0..
    call    update_leds
    call    clrscr                      ; first clear screen
    mov     esi, welcome_to_cmdline
    call    printstr


initialise_dlinked_list:

    mov     eax, dlinked_list
    mov     ebx, dlinked_list
    mov     dword [eax], dlinked_list + ((BufS+8)*(BufEtr-1))   ; == tail_node
    add     eax, BufS+8
    mov     ecx, BufEtr-1
  .loop:
    mov     [eax], ebx
    sub     eax, 4
    add     ebx, BufS+8
    mov     [eax], ebx
    add     eax, BufS+12
    loop    .loop
    sub     eax, 4
    mov     dword [eax], dlinked_list
    call    newline


commandline:

    mov     esi, prompt_yemelitcos
    call    printstr

    mov     ebx, [head_node]            ; reset current_node to head_node
    mov     [current_node], ebx
    mov     byte [ebx+4], 0             ; for next trial, head_node is always empty
    mov     [head_current], 1
    xor     ecx, ecx                    ; ecx = number of chars entered, max is BufS-1
    mov     edi, cmd_buffer

  .loop:                                ; 'char number BufS' is reserved for null

    call    getc                        ; keyboard.inc

    cmp     ah, 28                      ; 'enter' key pressed
    je      .enter
    cmp     ah, 14                      ; 'backspace' key pressed
    je      .backspace

    cmp     ah, 0x48                    ; 'up' key pressed
    je      browse_entries.up
    cmp     ah, 0x50                    ; 'down' key pressed
    je      browse_entries.down

    cmp     al, '~'
    je      .scroll_up
    cmp     al, '`'
    je      .scroll_down

    cmp     al, 0                       ; no normal key,
    je      .loop                       ; not usable here.
    cmp     ecx, BufS-1                 ; limit reached,
    jae     .loop                       ; so do nothing.

    stosb                               ; everything is fine,
    inc     ecx                         ; so store char in buffer,
    call    printchr

    cmp     [head_current], 1           ; is current_node = head_node
    jne     .loop                       ; if not, avoid changing old entries
    mov     esi, [head_node]
    add     esi, 4
    add     esi, ecx
    mov     [esi-1], al                 ; add entry into head_node content
    mov     byte [esi], 0               ; head node was initially empty
    jmp    .loop

  .enter:
    call    newline
    mov     al, 0                       ; finish user entry with null character
    stosb
    mov     dword [scanner], cmd_buffer
    mov     esi, cmd_buffer
    call    trim_all                    ; remove unecessary spaces
    call    run_cmd                     ; run command
    jnc     commandline
    call    store_node                  ; store if command ran
    jmp     commandline

  .backspace:
    cmp     ecx, 0                      ; cannot delete the prompt.. :-)
    je      .loop
    dec     edi                         ; 'remove' one char from buffer
    call    backspace                   ; do backspace on screen
    dec     ecx                         ; decrease buffer counter

    cmp     [head_current], 1
    jne     .loop
    mov     esi, [head_node]            ; head was initially empty
    add     esi, 4
    add     esi, ecx
    mov     byte [esi], 0               ; backspace on head_node content
    jmp     .loop

  .scroll_up:
    call    scroll_up
    jmp     .loop
  .scroll_down:
    call    scroll_down
    jmp     .loop



;---------------------------------------------------------------------------
;   browse up or down on entries/nodes in the double linked list
;    in: ecx=length of current entry
;    out: NOT callable
;---------------------------------------------------------------------------

browse_entries:
  .down:
    mov     ebx, [current_node]
    mov     esi, [ebx]                  ; esi = address of previous_node
    mov     [current_node], esi         ; variable now contains address to previous_node
    cmp     byte [esi+4], 0
    jne     .clr_entry
    cmp     esi, [head_node]            ; empty node found, yet it is not
    jne     .down                       ; the head_node, so skip it
    jmp     .clr_entry
  .up:
    mov     ebx, [current_node]
    mov     esi, [ebx+4+BufS]           ; esi = address of next_node
    mov     [current_node], esi         ; variable now contains address to next_node
    cmp     byte [esi+4], 0
    jne     .clr_entry
    cmp     esi, [head_node]            ; empty node found, yet it is not
    jne     .up                         ; the head_node, so skip it

  .clr_entry:                           ; ecx contained length of entry
    cmp     ecx, 0
    jbe     .lastly
    call    backspace                   ; delete current entry from screen
    dec     ecx
    jmp     .clr_entry

  .lastly:
    add     esi, 4                      ; esi contained address of new_current_node
    call    printstr                    ; put the new entry onto screen
    call    strlen
    mov     ecx, eax                    ; ecx = length of new entry (printed just now)
    mov     edi, cmd_buffer             ; copy new entry to cmd_buffer
    call    strcpy
    mov     edi, [strcpy.edi]           ; get to end of string after strcpy
    dec     edi                         ; skip null character

    sub     esi, 4                      ; now let's check whether
    cmp     esi, [head_node]            ; new_current_node = head_node
    je     .equal
    mov     [head_current], 0           ; current_node != head_node
    jmp     commandline.loop
  .equal:
    mov     [head_current], 1
    jmp     commandline.loop


;-----------------------------------------------------
;   store new entry into double linked list
;    in: an entry in cmd_buffer
;-----------------------------------------------------

store_node:                             ; entry will be stored
    push    ebx
    push    ecx
    push    esi
    push    edi
    push    dx

.search_entry:
    mov     ecx, BufEtr-1
    mov     esi, dlinked_list-4-BufS
    mov     edi, cmd_buffer
    mov     ebx, [head_node]
    add     ebx, 4
  .loop:
    add     esi, 8+BufS
    cmp     ebx, esi
    je      .loop
    call    strcmp
    jc      .update_node                ; entry found, so update it
    dec     ecx
    jnz    .loop

.new_node:                              ; entry not found, so add it
    mov     ebx, [head_node]            ; ebx = address of head_node
    mov     ecx, [ebx]                  ; ecx = address of tail_node
    mov     [head_node], ecx            ; head_node variable now contains address of tail_node
    add     ebx, 4
    mov     edi, ebx
    mov     esi, cmd_buffer
    call    strcpy                      ; copy entry to head_node (will become second node)
    jmp     .storedone

.update_node:                           ; make the found entry/node become first entry/node
  .delete:
    sub     esi, 4                      ; esi = address of found_node
    mov     ebx, [esi]                  ; ebx = address of previous_node
    mov     ecx, [esi+4+BufS]           ; ecx = address of next_node
    mov     [ebx+4+BufS], ecx           ; previous_node now contains address of next_node
    mov     [ecx], ebx                  ; next_node now contains address of previous_node
  .update:
    mov     ebx, [head_node]            ; ebx = address of head_node
    mov     ecx, [ebx+4+BufS]           ; ecx = address of second_node
    mov     [ecx], esi                  ; second_node now contains address of found_node
    mov     [esi+4+BufS], ecx           ; found_node now contains address of second_node
    mov     [esi], ebx                  ; found_node now contains address of head_node
    mov     [ebx+4+BufS], esi           ; head_node now contains address of found_node
                                        ; found_node is now new_second_node
.storedone:
    pop     dx
    pop     edi
    pop     esi
    pop     ecx
    pop     ebx
    ret


;---------------------------------------------
;   Run command
; in: scanner = command string
;---------------------------------------------

run_cmd:
    pusha
    mov     edi, .cmd_name
    call    get_next_word               ; get command name
    jnc     .done
    mov     esi, commands               ; commands.inc
    mov     ebp, 0                      ; command-table counter

  .next_cmd:
    call    strcmp
    jc      .cmd_found
    call    goto_next_word              ; moves esi to next word
    cmp     byte [esi], 0xFF            ; if no more commands
    je      .program_file
    inc     ebp                         ; inc command counter
    jmp     .next_cmd

  .cmd_found:
    shl     ebp, 2                      ; ebp*4 since call_table contains dd entries
    call    dword [call_table+ebp]
    stc
    jmp     .done
  .program_file:                        ; check if entry is a program in the disk
    mov     esi, .cmd_name
    call    load_program                ; programs.inc
    jc     .done
  .unknown_cmd:
    mov     esi, .prompt_unknown_cmd
    call    printstr
    clc
  .done:
    popa
    ret
  .prompt_unknown_cmd  db      "   unknown command", 13, 0
  .cmd_name: times BufS db 0


