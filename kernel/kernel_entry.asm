SECTION .init

[bits 32]
[extern main]
call main
jmp $