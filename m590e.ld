ENTRY(Reset_Handler)

__stack_size = 2048;
__checksum = -(_stack_end + (Reset_Handler | 1) + (NMI_Handler | 1) + (HardFault_Handler | 1));

MEMORY
{
   flash(rx) : ORIGIN = 0x00000000, LENGTH = 32K
   ram(rw)   : ORIGIN = 0x10000000, LENGTH = 8K
}

_flash_start = ORIGIN(flash);
_flash_end = ORIGIN(flash) + LENGTH(flash);
_ram_start = ORIGIN(ram);
_ram_end = ORIGIN(ram) + LENGTH(ram);

SECTIONS
{
   .intvecs : {
      _intvecs_start = .;
      KEEP(*(.intvecs))
      . = ALIGN(0x10);
      _intvecs_end = .;
   } > flash = 0xaaaaaaaa

   .text : {
      _text_start = .;
      *(.text*)
      . = ALIGN(0x10);
      _text_end = .;
   } > flash = 0xbbbbbbbb

   .rodata : {
      _rodata_start = .;
      *(.rodata*)
      . = ALIGN(0x10);
      _rodata_end = .;
   } > flash = 0xcccccccc

   .stack (NOLOAD) : {
      _stack_start = .;
      . += __stack_size;
      . = ALIGN(0x10);
      _stack_end = .;
   } > ram

   .data : {
      _data_start = .;
      *(.data*)
      _data_end = .;
   } > ram AT>flash

   .bss (NOLOAD) :{
      _bss_start = .;
      *(.bss*)
      *(COMMON*)
      _bss_end = .;
   } > ram

   .heap (NOLOAD) : ALIGN(0x10)  {
      end = .;
      _heap_start = end;
      . = ORIGIN(ram) + LENGTH(ram) - 0x20;
      _heap_end = .;
   } > ram

   _flash_size = LENGTH(flash);
   _ram_size = LENGTH(ram);
   _data_start_lma = LOADADDR (.data);
   _intvecs_size = SIZEOF (.intvecs);
   _text_size = SIZEOF (.text);
   _rodata_size = SIZEOF (.rodata);
   _data_size = SIZEOF (.data);
   _bss_size = SIZEOF (.bss);
   _stack_size = SIZEOF(.stack);
   _heap_size = SIZEOF(.heap);
}
