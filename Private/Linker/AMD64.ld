OUTPUT_FORMAT("elf64-x86-64")

ENTRY(KMain)

PHDRS {
    null    PT_NULL    FLAGS(0) ;
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2) | (1 << 3));
    rodata  PT_LOAD    FLAGS((1 << 2)) ;
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ;
}

SECTIONS {
    . = 0x00080000;

    .text : {
	  	KEEP(*(.multiboot))
        *(.text .text.*)
    } :text

    . += CONSTANT(MAXPAGESIZE);

     .init : {
       	*(.initl)
     }

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
}