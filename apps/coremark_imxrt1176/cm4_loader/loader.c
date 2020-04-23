/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "cm4_coremark.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CM4_COREMARK_START 0x1FFE0000U

#define ITCM_START   0x1FFE0000
#define ITCM_SIZE    (128*1024U)
#define DTCM_START   0x20000000
#define DTCM_SIZE    (128*1024U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

void HardFault_Handler(void)
{
    while (1);
}

static void enable_mecc(void)
{
    // This only works on A0 silicon
    // For B0 silicon, this register is readonly
    OCOTP->HW_OCOTP_FUSE004 |= 0x4;
    __ISB();
    __DSB();
}

static void enable_lmem_tcm_ecc(void)
{
    // Check eFuse 0x840[2] - MECC_ENABLE bit
    while (!(OCOTP->HW_OCOTP_FUSE004 & 0x4));

    // MCM->LMPECR[9,1] - Enable TCRAM ECC 1-bit/Multi-bit IRQ
    *(uint32_t *)0xE0080480 |= 0x303;

    // MCM->LMDR0[3] - Enable TCRAML ECC
    *(uint32_t *)0xE0080400 |= 0x0B;        /* Enable CM4 TCRAM_L ECC */
    // MCM->LMDR1[3] - Enable TCRAMU ECC
    *(uint32_t *)0xE0080404 |= 0x0B;        /* Enable CM4 TCRAM_U ECC */
}

static void init_lmem_itcm_ecc(void)
{
    for (uint32_t i = 0; i < ITCM_SIZE; i += sizeof(uint32_t))
    {
        *(uint32_t *)(ITCM_START + i) = 0;
    }
}

static void init_lmem_dtcm_ecc(void)
{
    for (uint32_t i = 0; i < DTCM_SIZE; i += sizeof(uint32_t))
    {
        *(uint32_t *)(DTCM_START + i) = 0;
    }
}

static void test_lmem_tcm_ecc_error(void)
{
    volatile uint8_t dat;

    dat = *(uint8_t *)(CM4_COREMARK_START + 0);
    if (dat)
    {
        __NOP();
    }

    for (uint32_t i = 0; i < 256; i++)
    {
        dat = *(uint8_t *)(CM4_COREMARK_START + i);
        if (dat)
        {
            __NOP();
        }
    }

    while (1);
}

/*!
 * @brief Main function
 */
int main(void)
{
    enable_mecc();
    enable_lmem_tcm_ecc();
    //test_lmem_tcm_ecc_error();
    init_lmem_itcm_ecc();
    init_lmem_dtcm_ecc();

    // Copy image to RAM.
    memcpy((void *)CM4_COREMARK_START, cm4_app_code, APP_LEN);
    
    uint32_t appStack = *(uint32_t *)(CM4_COREMARK_START);
    uint32_t appEntry = *(uint32_t *)(CM4_COREMARK_START + 4);

    // Turn off interrupts.
    __disable_irq();

    // Set the VTOR to default.
    SCB->VTOR = CM4_COREMARK_START;

    // Memory barriers for good measure.
    __ISB();
    __DSB();

    // Set main stack pointer and process stack pointer.
    __set_MSP(appStack);
    __set_PSP(appStack);

    // Jump to app entry point, does not return.
    void (*entry)(void) = (void (*)(void))appEntry;
    entry();

    while (1)
    {
    }
}
