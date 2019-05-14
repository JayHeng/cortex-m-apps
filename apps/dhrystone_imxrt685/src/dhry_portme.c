/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "dhry_portme.h"
#include "fsl_ctimer.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CTIMER CTIMER2                  /* Timer 3 */
#define CTIMER_MAT0_OUT kCTIMER_Match_0 /* Match output 0 */
#define CTIMER_CLK_FREQ CLOCK_GetCtimerClkFreq(2)

#define CLOCKS_PER_SEC (16000000)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void ctimer_match0_callback(uint32_t flags);

/* Array of function pointers for callback for each channel */
ctimer_callback_t ctimer_callback_table[] = {
    ctimer_match0_callback, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/*******************************************************************************
 * Variables
 ******************************************************************************/

double secs;

/* Match Configuration for Channel 0 */
static ctimer_match_config_t matchConfig0;

/*******************************************************************************
 * Code
 ******************************************************************************/

volatile uint32_t s_timerHighCounter = 0;
void ctimer_match0_callback(uint32_t flags)
{
    s_timerHighCounter++;
}

void timer_ctimer_init(void)
{
    ctimer_config_t config;

    /* Use 16 MHz clock for the Ctimer2 */
    CLOCK_AttachClk(kSFRO_to_CTIMER2);

    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER, &config);

    /* Configuration 0 */
    matchConfig0.enableCounterReset = true;
    matchConfig0.enableCounterStop = false;
    matchConfig0.matchValue = (uint32_t)~0;
    matchConfig0.outControl = kCTIMER_Output_Toggle;
    matchConfig0.outPinInitState = false;
    matchConfig0.enableInterrupt = true;

    CTIMER_RegisterCallBack(CTIMER, &ctimer_callback_table[0], kCTIMER_SingleCallback);
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
    CTIMER_StartTimer(CTIMER);
}

void start_time(void)
{
    timer_ctimer_init();
    s_timerHighCounter = 0;
}

void end_time(void)
{
    uint64_t retVal = 0;
    uint32_t high;
    uint32_t low;
    do
    {
        high = s_timerHighCounter;
        low = CTIMER_GetTimerCountValue(CTIMER);
    } while (high != s_timerHighCounter);
    retVal = ((uint64_t)high << 32U) + low;
    
    CTIMER_StopTimer(CTIMER);

    secs = retVal / (CLOCKS_PER_SEC * 1.0);
}


