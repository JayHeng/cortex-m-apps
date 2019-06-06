/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "board.h"
#include "clock_config.h"
/*${header:end}*/
  
/*${function:start}*/
void BOARD_InitHardware(void)
{
    uint32_t freq;
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    freq =  CLOCK_GetFreqFromObs(CCM_OBS_PLL_ARM_OUT) / 1000000;
    //DbgConsole_Printf("\r\nARM PLL %d MHz", freq * 4);
    freq =  CLOCK_GetFreqFromObs(CCM_OBS_M7_CLK_ROOT) / 1000000;
    //DbgConsole_Printf("\r\nCM7 %d MHz", freq * 4);

    //DbgConsole_Printf("\r\nOD is ");
#if OD
    //DbgConsole_Printf("Enabled\r\n");
#else
    //DbgConsole_Printf("Disabled\r\n");
#endif
    //DbgConsole_Printf("M7_ROOT_CTRL 0x%x\r\n", CCM->CLOCK_ROOT[0].CONTROL);
}

#ifdef COREMARK_USING_SYSTICK && COREMARK_USING_SYSTICK
uint32_t COREMARK_GetTimerClockFreq(void)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_M7_Systick);
}
#endif
/*${function:end}*/
