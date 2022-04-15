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
#include "fsl_debug_console.h"
#include "clock_config.h"
/*${header:end}*/
  
/*${function:start}*/
static void print_rt_clocks(void)
{
    PRINTF("Clock roots frequency (MHz):\n");
    for(uint32_t i=0; i<4; i++)
    {
        uint32_t freq = CLOCK_GetRootClockFreq((clock_root_t)i);
        freq /= 1000000;
        char name[5] = {0};
        *(uint32_t *)name = *(uint32_t *)(CCM->CLOCK_ROOT[i].RESERVED_1);
        PRINTF("%4s: %d\n", name, freq);
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

/*${function:end}*/
