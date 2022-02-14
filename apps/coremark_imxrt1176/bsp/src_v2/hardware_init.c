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
#include "core_portme.h"
/*${header:end}*/
  
/*${function:start}*/
static void print_rt_clocks(void)
{
    ee_printf("Clock roots frequency (MHz):\n");
    for(uint32_t i=0; i<4; i++)
    {
        float freq = CLOCK_GetRootClockFreq((clock_root_t)i);
        freq /= 1000000;
        char name[5] = {0};
        *(uint32_t *)name = *(uint32_t *)(CCM->CLOCK_ROOT[i].RESERVED_1);
        ee_printf("%4s: %6.2f\n", name, freq);
    }
}

void BOARD_InitHardware(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    print_rt_clocks();
}

#ifdef COREMARK_USING_SYSTICK && COREMARK_USING_SYSTICK
uint32_t COREMARK_GetTimerClockFreq(void)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_M7_Systick);
}
#endif
/*${function:end}*/
