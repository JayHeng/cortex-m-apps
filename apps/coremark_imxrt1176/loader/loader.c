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

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
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
