OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY
{   
  ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  /*
   * Ensure that entry.S / _entry is at 0x80000000, where qemu's -kernel jumps.
   *
   * In this example, if the program defines _etext (with a leading underscore), 
   * the linker will give a duplicate definition error. On the other hand, if the 
   * program defines etext (without a leading underscore), the linker will 
   * default to using the program's definition.
   * If the program references etext but does not define it, the linker will use 
   * the definition from the linker script.
   */

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    _trap = .;
    *(trampsec)
    . = ALIGN(0x1000);
    ASSERT(. - _trap == 0x1000, "error: trampoline larger than one page");
    PROVIDE(etext = .);
  } >ram

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  } >ram

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  } >ram

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
  } >ram

  . = ALIGN(0x1000);

  PROVIDE(free_start = .);
  PROVIDE(end = ORIGIN(ram) + LENGTH(ram));
}
