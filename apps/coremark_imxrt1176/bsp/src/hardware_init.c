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
        *(uint32_t *)name = CCM->CLOCK_ROOT[i].NAME;
        ee_printf("%4s: %6.2f\n", name, freq);
    }
}

static void enable_fbb(void)
{
    //== FBB lvl config 0->0.5 1->0.6 бнбн8->1.3
    ANADIG_PMU->PMU_BIAS_CTRL &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_pw_lvl_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL |= ANADIG_PMU_PMU_BIAS_CTRL_wb_pw_lvl_1p8(1);
    ANADIG_PMU->PMU_BIAS_CTRL &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_nw_lvl_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL |= ANADIG_PMU_PMU_BIAS_CTRL_wb_nw_lvl_1p8(1);     
    //==anatop_wb_cfg_1p8_cfg(0x01EE);
    ANADIG_PMU->PMU_BIAS_CTRL &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_cfg_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL |= ANADIG_PMU_PMU_BIAS_CTRL_wb_cfg_1p8(0x01EE);
    //==anatop_wb_pwr_sw_en_1p8(1);
    ANADIG_PMU->PMU_BIAS_CTRL2 &= ~ANADIG_PMU_PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL2 |= ANADIG_PMU_PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8(0x1);
    //==anatop_enable_wbias();
    ANADIG_PMU->PMU_BIAS_CTRL2 |= ANADIG_PMU_PMU_BIAS_CTRL2_wb_en_MASK;
}

void BOARD_InitHardware(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    print_rt_clocks();
    //enable_fbb();
}

#ifdef COREMARK_USING_SYSTICK && COREMARK_USING_SYSTICK
uint32_t COREMARK_GetTimerClockFreq(void)
{
    return CLOCK_GetRootClockFreq(kCLOCK_Root_M7_Systick);
}
#endif
/*${function:end}*/
