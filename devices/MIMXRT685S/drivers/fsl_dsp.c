/*
 * Copyright (c) 2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dsp.h"
#include "fsl_reset.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Initializing DSP core.
 *
 * Power up DSP TCM
 * Enable DSP clock
 * Reset DSP peripheral
 */
void DSP_Init(void)
{
    SYSCTL0->PDRUNCFG1_CLR = 0x0F000000;
    PMC->MEMSEQCTRL = 0x00000000;
    PMC->CTRL |= 0x00000001;
    while ((PMC->STATUS & PMC_STATUS_ACTIVEFSM_MASK) != 0)
    {
    }

    CLOCK_EnableClock(kCLOCK_Dsp);
    RESET_PeripheralReset(kDSP_RST_SHIFT_RSTn);
}

/*!
 * @brief Copy DSP image to destination address.
 *
 * Copy DSP image from source address to destination address with given size.
 *
 * @param dspCopyImage Structure contains information for DSP copy image to destination address.
 */
void DSP_CopyImage(dsp_copy_image_t *dspCopyImage)
{
    assert(dspCopyImage != NULL);
    assert(dspCopyImage->srcAddr != NULL);
    assert(dspCopyImage->destAddr != NULL);

    uint32_t * srcAddr = dspCopyImage->srcAddr;
    uint32_t * destAddr = dspCopyImage->destAddr;
    uint32_t size = dspCopyImage->size;

    while (size > 0)
    {
        *destAddr++ = *srcAddr++;
        size -= 4;
    }
}
