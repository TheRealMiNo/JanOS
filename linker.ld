ENTRY(_start)  /* Or whichever symbol is your entry point */

SECTIONS
{
    . = 0x1000;  /* Base address for the kernel binary */
    .init : { *(.init) }
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}