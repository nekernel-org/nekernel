OUTPUT_FORMAT("elf64-x86-64")

ENTRY(Main)

PHDRS {
    null    PT_NULL    FLAGS(0) ;
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2) | (1 << 3));
    rodata  PT_LOAD    FLAGS((1 << 2)) ;
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ;
}

SECTIONS {
    . = 0x80000000;

    .text : {
        *(.NewBoot)
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

     .init : {
        __SYSTEM_INIT = .;
       	*(.initl)
        __SYSTEM_FINI = .;
     }

    __SYSTEM_INIT_END = .;

    . += CONSTANT(MAXPAGESIZE);

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    . += CONSTANT(MAXPAGESIZE);

    .data : {
        *(.data .data.*)
    } :data

    . += CONSTANT(MAXPAGESIZE);

    .bss : {
        *(COMMON)
        *(.bss .bss.*)
    } :data

    __SYSTEM_STACK_PTR = .;
    __SYSTEM_STACK_END = . + 0x4000;
}
