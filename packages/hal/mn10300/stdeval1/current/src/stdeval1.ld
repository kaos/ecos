STARTUP(vectors.o)
ENTRY(reset_vector)

GROUP(libtarget.a libgcc.a)

#include <pkgconf/hal.h>

#ifdef CYG_HAL_STARTUP_ROM
#define LOAD_BASE 0x40000000
#define DATA_BASE 0x48000000
#define VSR_BASE
#endif
#ifdef CYG_HAL_STARTUP_RAM
# define LOAD_BASE 0x48008000
# define DATA_BASE
# define VSR_BASE  0x48000000
#endif


SECTIONS
{

  /* Start of ROM */
  . = LOAD_BASE;

  .rom_vectors	 :
  {
    KEEP(*(.vectors))
  }

  .text      :
  {
    _stext = .;
    *(.text*)
    *(.gnu.warning)
    *(.gnu.linkonce*)
    *(.init)

  }
  _etext = .;
  PROVIDE (etext = .);
  .fini      : { *(.fini)    }
  .rodata    : { *(.rodata*)  }
  .rodata1   : { *(.rodata1) }
  .fixup     : { *(.fixup)   }
  .gcc_except_table : { *(.gcc_except_table) }

/*  __rom_data_start = __rom_ram_vectors_start + SIZEOF( .ram_vectors ); */
  __rom_data_start = ALIGN(4);
  .data DATA_BASE : 
  AT( __rom_data_start )
  {
    __ram_data_start = .;
    *(.data*)
    *(.data1)
    _GOT1_START_ = .;
    *(.got1) 
    _GOT1_END_ = .;
    /* Put .ctors and .dtors next to the .got2 section, so that the pointers
       get relocated with -mrelocatable. */
    _GOT2_START_ = .;
    *(.got2)
    _GOT2_END_ = .;
    . = ALIGN(4);
#ifdef CYG_KERNEL_USE_INIT_PRIORITY
    __CTOR_LIST__ = .;
    KEEP(*(SORT(.ctors*)))
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    KEEP(*(SORT(.dtors*)))
    __DTOR_END__ = .;
#else
    __CTOR_LIST__ = .;
    *sched.o(.ctors*)
    *clock.o(.ctors*)
    *thread.o(.ctors*)
    *(.ctors*)
    __CTOR_END__ = .;
    __DTOR_LIST__ = .;
    *(.dtors)
    __DTOR_END__ = .;
#endif
    _GOT_START_ = .;
    _GLOBAL_OFFSET_TABLE_ = . + 32768;
    _SDA_BASE_ = .;
    *(.got.plt)
    *(.got)
    _GOT_END_ = .;
    *(.dynamic)
    /* We want the small data sections together, so single-instruction offsets
       can access them all, and initialized data all before uninitialized, so
       we can shorten the on-disk segment size.  */
    *(.sdata*)
   *(.sbss*)
  }
  __ram_data_end = .;
  PROVIDE( __ram_data_end = .);
  _edata  =  .;
  PROVIDE (edata = .);
  . = ALIGN(4);
  __bss_start = .;
  .bss       :
  {
   *(.scommon)
   *(.dynbss)
   *(.bss)
   *(COMMON)
  }
  . = ALIGN(4);
  _end = . ;
  PROVIDE (end = .);

}

_mn10300_interrupt_control = 0x34000100;
_mn10300_interrupt_vectors = 0x20000000;

#ifdef CYG_HAL_STARTUP_RAM
_hal_vsr_table = VSR_BASE;
_hal_virtual_vector_table = VSR_BASE+0x80;
#endif
