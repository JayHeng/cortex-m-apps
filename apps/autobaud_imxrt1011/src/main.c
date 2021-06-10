/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "autobaud.h"
#include "microseconds.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifndef FREQ_392MHz
#define FREQ_392MHz (392UL * 1000 * 1000)
#endif

enum
{
    kMaxIpgClock = 144000000UL,
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void uart_pinmux_config(pinmux_type_t pinmux);

/*******************************************************************************
 * Code
 ******************************************************************************/

//! @brief Get PIT clock value
uint32_t get_pit_clock(void)
{
    // Get PIT clock source
    uint32_t ahbBusDivider = ((CCM->CBCDR & CCM_CBCDR_IPG_PODF_MASK) >> CCM_CBCDR_IPG_PODF_SHIFT) + 1;
    uint32_t periphDivider = ((CCM->CSCMR1 & CCM_CSCMR1_PERCLK_PODF_MASK) >> CCM_CSCMR1_PERCLK_PODF_SHIFT) + 1;
    return SystemCoreClock / ahbBusDivider / periphDivider;
}

void clock_setup(void)
{
    uint32_t clock_divider = 1;
    uint32_t core_clock = 0;

    CLOCK_SetXtal0Freq(CPU_XTAL_CLK_HZ);
    core_clock = FREQ_392MHz / clock_divider;

    // If clock is not configured, configure clock first, otherwise, just update SystemCoreClock
    {
        CCM_ANALOG->PLL_SYS_CLR = CCM_ANALOG_PLL_SYS_POWERDOWN_MASK;
        while ((CCM_ANALOG->PLL_SYS & CCM_ANALOG_PLL_SYS_LOCK_MASK) == 0)
        {
        }
        CCM_ANALOG->PLL_SYS_SET = CCM_ANALOG_PLL_SYS_BYPASS_MASK;

        /*
         * PLL_SYS / PLL 2 / PLL 528
         * PFD0 = 396MHz
         * PFD1 = 396MHz
         * PFD2 = 500MHz
         * PFD3 = 396MHz
         */
        CCM_ANALOG->PFD_528 =
            (CCM_ANALOG->PFD_528 & (~(CCM_ANALOG_PFD_528_PFD0_FRAC_MASK | CCM_ANALOG_PFD_528_PFD1_FRAC_MASK |
                                      CCM_ANALOG_PFD_528_PFD2_FRAC_MASK | CCM_ANALOG_PFD_528_PFD3_FRAC_MASK))) |
            CCM_ANALOG_PFD_528_PFD0_FRAC(24) | CCM_ANALOG_PFD_528_PFD1_FRAC(24) | CCM_ANALOG_PFD_528_PFD2_FRAC(19) |
            CCM_ANALOG_PFD_528_PFD3_FRAC(24);

        CCM_ANALOG->PLL_USB1 =
            CCM_ANALOG_PLL_USB1_DIV_SELECT(0) | CCM_ANALOG_PLL_USB1_POWER(1) | CCM_ANALOG_PLL_USB1_ENABLE(1);
        while ((CCM_ANALOG->PLL_USB1 & CCM_ANALOG_PLL_USB1_LOCK_MASK) == 0)
        {
        }
        CCM_ANALOG->PLL_USB1_SET = CCM_ANALOG_PLL_USB1_BYPASS_MASK;

        /*
         * PLL_USB / PLL 3 / PLL 480
         * PFD0 = 247MHz
         * PFD1 = 247MHz
         * PFD2 = 332MHz
         * PFD3 = 392MHz
         */
        CCM_ANALOG->PFD_480 =
            (CCM_ANALOG->PFD_480 & (~(CCM_ANALOG_PFD_480_PFD0_FRAC_MASK | CCM_ANALOG_PFD_480_PFD1_FRAC_MASK |
                                      CCM_ANALOG_PFD_480_PFD2_FRAC_MASK | CCM_ANALOG_PFD_480_PFD3_FRAC_MASK))) |
            CCM_ANALOG_PFD_480_PFD0_FRAC(35) | CCM_ANALOG_PFD_480_PFD1_FRAC(35) | CCM_ANALOG_PFD_480_PFD2_FRAC(26) |
            CCM_ANALOG_PFD_480_PFD3_FRAC(22);

        uint32_t ipg_divider = (core_clock + kMaxIpgClock - 1) / kMaxIpgClock;

        CCM->CBCDR =
            (CCM->CBCDR & (~(CCM_CBCDR_PERIPH_CLK_SEL_MASK | CCM_CBCDR_AHB_PODF_MASK | CCM_CBCDR_IPG_PODF_MASK))) |
            CCM_CBCDR_PERIPH_CLK_SEL(0) | CCM_CBCDR_AHB_PODF(clock_divider - 1) | CCM_CBCDR_IPG_PODF(ipg_divider - 1);

        // LPUART clock configuration, peripheral clock 40MHz
        CCM->CSCDR1 = (CCM->CSCDR1 & (~(CCM_CSCDR1_UART_CLK_SEL_MASK | CCM_CSCDR1_UART_CLK_PODF_MASK))) |
                      CCM_CSCDR1_UART_CLK_PODF(1);

        // Configure the clock source for core modules
        CCM->CBCMR = ((CCM->CBCMR & ~(CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK)) | (CCM_CBCMR_PRE_PERIPH_CLK_SEL(1)));

        // Finally enable PLLs
        CCM_ANALOG->PLL_SYS &= ~CCM_ANALOG_PLL_SYS_BYPASS_MASK;
        CCM_ANALOG->PLL_USB1 &= ~CCM_ANALOG_PLL_USB1_BYPASS_MASK;
    }

    SystemCoreClockUpdate();
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    //BOARD_InitPins();
    //BOARD_BootClockRUN();
    //SystemCoreClockUpdate();
    clock_setup();

    // PIT clock gate control ON
    CLOCK_EnableClock(kCLOCK_Pit);
    microseconds_init();
    //microseconds_delay(5000000);
    uart_pinmux_config(kPinmuxType_PollForActivity);
    autobaud_init();

    uint32_t baudrate;
    while (autobaud_get_rate(&baudrate) != kStatus_Success);
    autobaud_deinit();
    uart_pinmux_config(kPinmuxType_Peripheral);

    //BOARD_InitDebugConsole();
    uint32_t uartClkSrcFreq = BOARD_DebugConsoleSrcFreq();
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, baudrate, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);

    PRINTF("Autobaud test success\r\n");

    while (1)
    {
    }
}
