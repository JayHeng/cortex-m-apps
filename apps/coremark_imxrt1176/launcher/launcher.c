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

}

static void init_cm4_tcm_ecc(void)
{

}

/*!
 * @brief Main function
 */
int main(void)
{
    enable_cm4_tcm_ecc();
    init_cm4_tcm_ecc();

    // Copy image to RAM.
    memcpy((void *)APP_START, cm4_app_code, APP_LEN);
    
    IOMUXC_LPSR_GPR->GPR0 = 0x0000;
    IOMUXC_LPSR_GPR->GPR1 = 0x1FFE;

    SRC->SCR |= SRC_SCR_BT_RELEASE_M4_MASK;

    while (1)
    {
    }
}
