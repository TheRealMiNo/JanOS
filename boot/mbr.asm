; A boot sector that boots a C kernel in 32 - bit protected mode
[ org 0x7c00 ]
KERNEL_OFFSET equ 0x1000 ; This is the memory offset to which we will load our kernel
mov [ BOOT_DRIVE ], dl ; BIOS stores our boot drive in DL , so it ’s
; best to remember this for later.
mov bp , 0xF000 ; Set -up the stack.
mov sp , bp
mov bx , MSG_REAL_MODE ; Announce that we are starting
call print_string ; booting from 16 - bit real mode
call load_kernel ; Load our kernel
call switch_to_pm ; Switch to protected mode , from which
; we will not return
jmp $
; Include our useful , hard - earned routines
%include "boot/print/print_string.asm"
%include "boot/disk/disk_load.asm"
%include "boot/pm/gdt.asm"
%include "boot/pm/print_string_pm.asm"
%include "boot/pm/switch_to_pm.asm"
[bits 16]
; load_kernel
load_kernel :
mov bx , MSG_LOAD_KERNEL ; Print a message to say we are loading the kernel
call print_string
mov bx , KERNEL_OFFSET ; Set -up parameters for our disk_load routine , so
mov dh , 50 ; that we load the first 15 sectors ( excluding
mov dl , [ BOOT_DRIVE ] ; the boot sector ) from the boot disk ( i.e. our
call disk_load ; kernel code ) to address KERNEL_OFFSET
ret
[bits 32]
BEGIN_PM :
mov ebx , MSG_PROT_MODE ; Use our 32 - bit print routine to
call print_string_pm ; announce we are in protected mode
call KERNEL_OFFSET ; Now jump to the address of our loaded
; kernel code , assume the brace position ,
; and cross your fingers. Here we go!
jmp $ ; Hang.
; Global variables
BOOT_DRIVE db 0
MSG_REAL_MODE db " Started in 16 - bit Real Mode " , 0
MSG_PROT_MODE db " Successfully landed in 32 - bit Protected Mode " , 0
MSG_LOAD_KERNEL db " Loading kernel into memory. " , 0
; Bootsector padding

times 446 - ($ - $$) db 0  ; Bootloader space (446 bytes)

; Partition entry (16 bytes, points to FAT32 starting at sector 2048)
db 0x80                    ; Bootable flag
db 0x01, 0x01, 0x00        ; CHS address of partition start (dummy)
db 0x0B                    ; Partition type (FAT32)
db 0xFE, 0xFF, 0xFF        ; CHS address of partition end (dummy)
dd 2048                    ; LBA of partition start
dd 204800 - 2048           ; Number of sectors in the partition

times 510 -( $ - $$ ) db 0
dw 0xaa55