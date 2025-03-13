/*
                  Linker File for Renesas FSP
*/

INCLUDE memory_regions.ld

/* If a flat (secure) project has DEFINED RAM_NS_BUFFER_LENGTH, then emit IDAU symbols to allocate non-secure RAM. */
__RESERVE_NS_RAM = !DEFINED(PROJECT_NONSECURE) && DEFINED(RAM_NS_BUFFER_LENGTH) && (OPTION_SETTING_S_LENGTH != 0);

APP_ROM_START  = ITCM_START;
APP_ROM_LENGTH = ITCM_LENGTH;
APP_RAM_START  = DTCM_START;
APP_RAM_LENGTH = DTCM_LENGTH;

/* Define memory regions. */
MEMORY
{
  FLASH (rx)                      : ORIGIN = APP_ROM_START, LENGTH = APP_ROM_LENGTH
  RAM (rwx)                       : ORIGIN = APP_RAM_START, LENGTH = APP_RAM_LENGTH
}

/* Library configurations */
GROUP(libgcc.a libc.a libm.a)

ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        __tz_FLASH_S = ABSOLUTE(APP_ROM_START);
        __ROM_Start = .;

        /* Even though the vector table is not 256 entries (1KB) long, we still allocate that much
         * space because ROM registers are at address 0x400 and there is very little space
         * in between. */
        KEEP(*(.fixed_vectors*))
        KEEP(*(.application_vectors*))
        __Vectors_End = .;

        *(.text*)

        KEEP(*(.version))
        KEEP(*(.init))
        KEEP(*(.fini))

        *(.rodata*)

        __ROM_End = .;
    } > FLASH = 0xFF

    __Vectors_Size = __Vectors_End - __Vectors;

    __exidx_start = .;
    /DISCARD/ :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
    __exidx_end = .;

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
    /*
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG (__data_end__ - __data_start__)
        LONG (__etext2)
        LONG (__data2_start__)
        LONG (__data2_end__ - __data2_start__)
        __copy_table_end__ = .;
    } > FLASH
    */

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    /*
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        LONG (__bss2_start__)
        LONG (__bss2_end__ - __bss2_start__)
        __zero_table_end__ = .;
    } > FLASH
    */

    __etext = .;

    __tz_RAM_S = ORIGIN(RAM);

    /* Initialized data section. */
    .data :
    {
        __data_start__ = .;
        . = ALIGN(4);

        __Code_In_RAM_Start = .;

        KEEP(*(.code_in_ram*))
        __Code_In_RAM_End = .;

        *(vtable)
        /* Don't use *(.data*) because it will place data meant for .data_flash in this section. */
        *(.data.*)
        *(.data)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(4);

        /* All data end */
        __data_end__ = .;

    } > RAM AT > FLASH

    /* TrustZone Secure Gateway Stubs Section */

    /* Store location counter for SPI non-retentive sections. */
    sgstubs_pre_location = .;

    /* Determine the secure gateway stubs address either by the provided linker variable or the next 1024-byte block. */
    SGSTUBS_LOC = (DEFINED(PROJECT_SECURE) && DEFINED(FLASH_NSC_START)) ? ABSOLUTE(FLASH_NSC_START) : ALIGN(1024);
    .gnu.sgstubs SGSTUBS_LOC : ALIGN(1024)
    {
        __FLASH_NSC_START = DEFINED(FLASH_NSC_START) ? ABSOLUTE(FLASH_NSC_START) : __RESERVE_NS_RAM ? ABSOLUTE(APP_ROM_START + APP_ROM_LENGTH) : ALIGN(1024);
        _start_sg = .;
        *(.gnu.sgstubs*)
        . = ALIGN(32);
        _end_sg = .;
    } > FLASH

    __tz_FLASH_N = DEFINED(FLASH_NS_START) ? ABSOLUTE(FLASH_NS_START) : __RESERVE_NS_RAM ? ABSOLUTE(APP_ROM_START + APP_ROM_LENGTH) : APP_ROM_LENGTH < 32768 ? APP_ROM_LENGTH : ALIGN(32768);
    FLASH_NS_IMAGE_START = DEFINED(FLASH_NS_IMAGE_START) ? FLASH_NS_IMAGE_START : __tz_FLASH_N;

    .noinit (NOLOAD):
    {
        . = ALIGN(4);
        __noinit_start = .;
        KEEP(*(.noinit*))
        . = ALIGN(8);
        /* Place the FreeRTOS heap here so that the __HeapLimit calculation does not include the freertos heap. */
        KEEP(*(.heap.*))
        __noinit_end = .;
    } > RAM

    . = .;
    __nocache_pre_location = .;
    .nocache ALIGN(32) (NOLOAD):
    {
        __nocache_start = .;

        KEEP(*(.nocache))

        . = ALIGN(32);
        __nocache_end = .;
    } > RAM
    . = (SIZEOF(.nocache) > 0) ? __nocache_end : __nocache_pre_location;

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;
        /* Place the STD heap here. */
        KEEP(*(.heap))
        __HeapLimit = .;
    } > RAM

    /* Stacks are stored in this section. */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(8);
        __StackLimit = .;
        /* Main stack */
        KEEP(*(.stack))
        __StackTop = .;
        /* Thread stacks */
        KEEP(*(.stack*))
        __StackTopAll = .;
    } > RAM

    PROVIDE(__stack = __StackTopAll);

    /* This symbol represents the end of user allocated RAM. The RAM after this symbol can be used
       at run time for things such as ThreadX memory pool allocations. */
    __RAM_segment_used_end__ = ALIGN(__StackTopAll , 4);
}