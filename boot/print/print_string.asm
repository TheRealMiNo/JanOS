print_string:
    mov ah, 0x0e              ; Teletype function (prints a character)
print_next_char:
    mov al, [bx]              ; Move the character at address [bx] into al
    cmp al, 0                 ; Check if we've reached the end of the string
    je print_string_done      ; If it's 0 (null terminator), we're done
    int 0x10                  ; BIOS interrupt to print the character
    inc bx                    ; Move to the next character in the string
    jmp print_next_char       ; Repeat for the next character
print_string_done:
    ret                       ; Return from the function
