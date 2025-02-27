/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "cm7_coremark.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CM7_COREMARK_START 0U

uint32_t s_appStack = 0;
uint32_t s_appEntry = 0;

typedef void (*call_function_t)(void);
call_function_t s_callFunction = 0;

#define ITCM_START   0x00000000
#define ITCM_SIZE    (256*1024U)
#define DTCM_START   0x20000000
#define DTCM_SIZE    (256*1024U)

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

static void enable_cm7_tcm_ecc(void)
{
    // According to ARM Cortex-M7 Process Technical Reference Manual, section 5.7.5
    // The SW needs to enable the RMW bit on the TCM ECC enabled system
    SCB->ITCMCR |= SCB_ITCMCR_RMW_Msk;
    SCB->DTCMCR |= SCB_DTCMCR_RMW_Msk;
    
    asm("NOP");
    asm("NOP");
    asm("NOP");
}

static void enable_flexram_tcm_ecc(void)
{
    FLEXRAM->INT_STAT_EN |= 0x3F18u;

    *(uint32_t *)(FLEXRAM_BASE + 0x108) |= (1u << 5); /* Enable CM7 TCM ECC */
    
    asm("NOP");
    asm("NOP");
    asm("NOP");
}

static void init_flexram_itcm_ecc(void)
{
    for (uint32_t i = 0; i < ITCM_SIZE; i += sizeof(uint64_t))
    {
        *(uint64_t *)(ITCM_START + i) = 0;
    }
}

static void init_flexram_dtcm_ecc(void)
{
    for (uint32_t i = 0; i < DTCM_SIZE; i += sizeof(uint32_t))
    {
        *(uint32_t *)(DTCM_START + i) = 0;
    }
}

static void test_flexram_tcm_ecc_error(void)
{
    volatile uint8_t dat;

    dat = *(uint8_t *)(CM7_COREMARK_START + 0);
    if (dat)
    {
        __NOP();
    }

    for (uint32_t i = 0; i < 256; i++)
    {
        dat = *(uint8_t *)(CM7_COREMARK_START + i);
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
    enable_cm7_tcm_ecc();
    enable_flexram_tcm_ecc();
    //test_flexram_tcm_ecc_error();
    init_flexram_itcm_ecc();
    init_flexram_dtcm_ecc();

    // Copy image to RAM.
    memcpy((void *)CM7_COREMARK_START, cm7_app_code, APP_LEN);
    
    s_appStack = *(uint32_t *)(CM7_COREMARK_START);
    s_appEntry = *(uint32_t *)(CM7_COREMARK_START + 4);

    // Turn off interrupts.
    __disable_irq();

    // Set the VTOR to default.
    SCB->VTOR = CM7_COREMARK_START;

    // Memory barriers for good measure.
    __ISB();
    __DSB();

    // Set main stack pointer and process stack pointer.
    __set_MSP(s_appStack);
    __set_PSP(s_appStack);

    // Jump to app entry point, does not return.
    s_callFunction = (call_function_t)s_appEntry;
    s_callFunction();

    while (1)
    {
    }
}
