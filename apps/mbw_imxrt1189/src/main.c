/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "mbw.h"

#include "fsl_ele_base_api.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

extern int mbw_main(uint32_t testno, uint32_t showavg, uint32_t nr_loops, uint64_t block_size, uint32_t mem_start, uint32_t mem_size);


/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;

/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    g_systickCounter++;

    /*
     *  RT118x ELE requires ping every 24 hours, which is mandatory,
     *  otherwise soc may reset.
     *
     *  note:
     *    1. This is generic rule for all RT118x demos.
     *    2. Most of RT118x demos don't ping ELE every 24 hours, that
     *       is because those demos focus on the function demonstrate only.
     *       It is still MUST to ping ELE every 24 hours if demo run
     *       duration > 24 hours.
     *    3. Below is an example to ping the ELE every 23(but not 24)
     *       hours, in case of any clock inaccuracy.
     */
    if (g_systickCounter >= (23 * 60 * 60 * 1000UL))
    {
        g_systickCounter = 0;
        ELE_BaseAPI_Ping(MU_RT_S3MUA);
    }
}

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

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* Set systick reload value to generate 1ms interrupt */
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        /* 1ms interrupt configure error, stick here */
        while (1)
        {
        }
    }

    print_rt_clocks();
    timer_init();
    
    mbw_main(0, 1, 0, 0x400, 0x20500000, 0x20000);

    while (1)
    {
    }
}
