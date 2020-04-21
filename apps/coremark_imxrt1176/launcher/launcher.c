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
#include "fsl_soc_src.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_START 0x20200000U

#define CM4_OCRAM_START   0x20200000
#define CM4_OCRAM_SIZE    (256*1024U)

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

static void enable_cm4_tcm_ecc(void)
{
    // MCM->LMDR0[3] - Enable TCRAML ECC
    *(uint32_t *)0xE0080400 |= 0x0B;        /* Enable CM4 TCRAM_L ECC */
    // MCM->LMDR1[3] - Enable TCRAMU ECC
    *(uint32_t *)0xE0080404 |= 0x0B;        /* Enable CM4 TCRAM_U ECC */
}

static void init_cm4_tcm_ecc(void)
{
    for (uint32_t i = 0; i < CM4_OCRAM_SIZE; i += sizeof(uint32_t))
    {
        *(uint32_t *)(CM4_OCRAM_START + i) = 0;
    }
}

static void boot_cm4_app(void)
{
    IOMUXC_LPSR_GPR->GPR0 = IOMUXC_LPSR_GPR_GPR0_CM4_INIT_VTOR_LOW(APP_START);
    IOMUXC_LPSR_GPR->GPR1 = IOMUXC_LPSR_GPR_GPR1_CM4_INIT_VTOR_HIGH(APP_START >> 16);

    /* If CM4 is already running (released by debugger), then reset the CM4.
       If CM4 is not running, release it. */
    if ((SRC->SCR & SRC_SCR_BT_RELEASE_M4_MASK) != 0)
    {
        SRC_AssertSliceSoftwareReset(SRC, kSRC_M4CoreSlice);
        while (kSRC_SliceResetInProcess == SRC_GetSliceResetState(SRC, kSRC_M4CoreSlice))
        {
        }
    }
    else
    {
        SRC_ReleaseCoreReset(SRC, kSRC_CM4Core);
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    enable_cm4_tcm_ecc();
    init_cm4_tcm_ecc();

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)APP_START, APP_LEN);
#endif

    // Copy image to RAM.
    memcpy((void *)APP_START, cm4_app_code, APP_LEN);

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)APP_START, APP_LEN);
#endif
    
    boot_cm4_app();

    while (1)
    {
    }
}
