ENTRY(_start)

SECTIONS
{
	. = 0x02000000;
	.text BLOCK(4K) : ALIGN(4K)
	{
			*(.text)
	}
	. = 0x02100000;
	.data BLOCK(4K) : ALIGN(4K)
	{

	*(.rodata)
			*(.data)
	}
	.bss BLOCK(4K) (NOLOAD) : ALIGN(4K)
	{
			*(.bss)
	}
}