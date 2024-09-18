print_hex:
    pusha
    mov ax, dx
    and ax, 0xF000
    shr ax, 12
    mov si, hex_chars
    add si, ax
    mov al, [si]
    mov bx, HEX_OUT
    mov [bx + 2], al    

    mov ax, dx
    and ax, 0x0F00
    shr ax, 8
    mov si, hex_chars
    add si, ax
    mov al, [si]
    mov [bx + 3], al    

    mov ax, dx
    and ax, 0x00F0
    shr ax, 4
    mov si, hex_chars
    add si, ax
    mov al, [si]
    mov [bx + 4], al    

    mov ax, dx
    and ax, 0x000F
    mov si, hex_chars
    add si, ax
    mov al, [si]
    mov [bx + 5], al    
    call print_string
    popa
    ret

HEX_OUT : 
    db "0x0000", 0
hex_chars:
    db "0123456789abcdef", 0