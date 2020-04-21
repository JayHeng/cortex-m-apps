/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "cm4_loader.h"
#include "fsl_soc_src.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CM4_LOADER_START 0x20240000U

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

static void boot_cm4_app(void)
{
    IOMUXC_LPSR_GPR->GPR0 = IOMUXC_LPSR_GPR_GPR0_CM4_INIT_VTOR_LOW(CM4_LOADER_START);
    IOMUXC_LPSR_GPR->GPR1 = IOMUXC_LPSR_GPR_GPR1_CM4_INIT_VTOR_HIGH(CM4_LOADER_START >> 16);

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
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)CM4_LOADER_START, APP_LEN);
#endif

    // Copy image to RAM.
    memcpy((void *)CM4_LOADER_START, cm4_app_code, APP_LEN);

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)CM4_LOADER_START, APP_LEN);
#endif
    
    boot_cm4_app();

    while (1)
    {
    }
}
