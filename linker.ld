ENTRY(_start)

SECTIONS
{
    /* .text is at qemu 64bit RISC-V RAM-start address */
    . = 0x80000000;
    .text : { *(.text*) }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) }
}

_stack_top = . + 0x10000; /* 64kB of RAM after the .bss */
/* 0001 0000 0000 0000 0000 */
/* stack grows downard, so .bss+64kB and then it moves
toward .bss as it allocates more ram for stack */