/*
** ###################################################################
**     Processors:          MIMXRT798SGAWAR_ezhv
**                          MIMXRT798SGFOA_ezhv
**
**     Compiler:            LLVM C Compiler
**     Reference manual:    iMXRT700RM Rev.1, 08/2022
**     Version:             rev. 1.0, 2023-11-14
**     Build:               b240227
**
**     Abstract:
**         Linker file for the LLVM C Compiler
**
**     Copyright 2024 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/

/* Entry Point */
ENTRY(Reset_Handler)
OUTPUT_ARCH( "riscv" )

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x800;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x800;

/* Specify the memory areas, the  */
MEMORY
{
  m_text                (RX)  : ORIGIN = 0x24100000, LENGTH = 0x00008000
  m_data                (RW)  : ORIGIN = 0x24108000, LENGTH = 0x00007F00
  m_data_shared         (RW)  : ORIGIN = 0x2410FF00, LENGTH = 0x00000100
}

/* Define output sections */
SECTIONS
{
  .vectors : ALIGN(4)
  {
     PROVIDE(__VECTOR_TABLE = .);
     KEEP(*(.vectors))

     /* Event Manager Vector. */
    . = ALIGN(4);
     KEEP(*(.em_vector0))
    . = ALIGN(4);
     KEEP(*(.em_vector1))
    . = ALIGN(4);
     KEEP(*(.em_vector2))
    . = ALIGN(4);
     KEEP(*(.em_vector3))
  } > m_text

  .text :
  {
    . = ALIGN(4);
    __user_vector = .;
    KEEP(*(user_vectors))
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.srodata .srodata.*)
    *(.eh_frame)
    *(.init)
    *(.fini)
  } > m_text

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_text

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_text

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > m_text

 .ctors :
  {
    __CTOR_LIST__ = .;
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END__ = .;
  } > m_text

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > m_text

  . = ALIGN(4);
  __etext = .;              /* define a global symbol at end of code */
  __global_pointer$ = .;    /* define a global symbol at end of code */

  .data : AT(__etext)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;     /* create a global symbol at data start */
    *(.data)                /* .data sections */
    *(.data*)               /* .data* sections */
    *(.sdata .sdata.*)
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > m_data

  _edata = .;

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(.sbss)
    *(.sbss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > m_data

  /* End of uninitalized data segement */
  _end = .;
  PROVIDE(end = .);

  /* Heap is managed internally by newlib and begins after _end, see
   * libgloss/riscv/sys_sbrk.c */
  /* The stack pointer is not even initialized in libgloss/riscv/crt0.S, so
   * I am assume it starts at 0 and decrements from there, sigh... */
 .heap :
 {
     . = ALIGN(8);
     __heap_start = .;
     . += HEAP_SIZE;
     __heap_end = .;
     _heap_end = __heap_end;
  } > m_data

 .stack :
 {
   . = ALIGN(8);
    __StackLimit = .;
   . += STACK_SIZE;
     __StackTop = .;
 } > m_data
}
