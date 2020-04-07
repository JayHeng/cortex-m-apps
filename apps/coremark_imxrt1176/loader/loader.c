/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "coremark.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_START 0U

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

static void enable_tcm_ecc(void)
{
    // According to ARM Cortex-M7 Process Technical Reference Manual, section 5.7.5
    // The SW needs to enable the RMW bit on the TCM ECC enabled system
    SCB->ITCMCR |= SCB_ITCMCR_RMW_Msk;
    SCB->DTCMCR |= SCB_DTCMCR_RMW_Msk;
    
    asm("NOP");
    asm("NOP");
    asm("NOP");
    
    *(uint32_t *)(FLEXRAM_BASE + 0x108) |= (1u << 5); /* Enable CM7 TCM ECC */
    
    asm("NOP");
    asm("NOP");
    asm("NOP");
}

static void init_itcm_ecc(void)
{
    for (uint32_t i = 0; i < ITCM_SIZE; i += sizeof(uint64_t))
    {
        *(uint64_t *)(ITCM_START + i) = 0;
    }
}

static void init_dtcm_ecc(void)
{
    for (uint32_t i = 0; i < DTCM_SIZE; i += sizeof(uint64_t))
    {
        *(uint64_t *)(DTCM_START + i) = 0;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    enable_tcm_ecc();
    init_itcm_ecc();
    init_dtcm_ecc();

    // Copy image to RAM.
    memcpy((void *)APP_START, app_code, APP_LEN);
    
    uint32_t appStack = *(uint32_t *)(APP_START);
    uint32_t appEntry = *(uint32_t *)(APP_START + 4);

    // Turn off interrupts.
    __disable_irq();

    // Set the VTOR to default.
    SCB->VTOR = APP_START;

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
