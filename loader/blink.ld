SECTIONS
{
    . = 0x8000;
    .text :
    {
        *(.text)
    }
    . = ALIGN(4096); /* align to page size */
    .rodata :
    {
        *(.rodata)
    }
}
