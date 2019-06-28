/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "dhry_portme.h"
#include "clock_config.h"
#include "app.h"
#include "fsl_pit.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#define CLOCKS_PER_SEC (24000000)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

double secs;

/*******************************************************************************
 * Code
 ******************************************************************************/

volatile uint32_t s_timerHighCounter = 0;

void PIT_IRQ_HANDLER(void)
{
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    s_timerHighCounter++;
}

void timer_pit_init(void)
{
    /* Structure of initialize PIT */
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    /* Init pit module */
    PIT_Init(PIT, &pitConfig);
    /* Set max timer period for channel 0 */
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, (uint32_t)~0);
    /* Enable timer interrupts for channel 0 */
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    /* Enable at the NVIC */
    EnableIRQ(PIT_IRQ_ID);
    /* Start channel 0 */
    PIT_StartTimer(PIT, kPIT_Chnl_0);
}

void start_time(void)
{
    timer_pit_init();
    s_timerHighCounter = 0;
}

void end_time(void)
{
    uint64_t retVal;
    uint32_t high;
    uint32_t low;
    do
    {
        high = s_timerHighCounter;
        low = ~PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_0);
    } while (high != s_timerHighCounter);
    retVal = ((uint64_t)high << 32U) + low;

    secs = retVal / (CLOCKS_PER_SEC * 1.0);
}


