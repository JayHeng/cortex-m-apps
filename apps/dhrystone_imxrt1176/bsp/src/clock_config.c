/*
 * Copyright 2018-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXXPLL() to configure corresponding PLL clock.
 *
 * 2. Call CLOCK_InitXXXpfd() to configure corresponding PLL pfd clock.
 *
 * 3. Call CLOCK_SetMux() to configure corresponding clock source for target clock out.
 *
 * 4. Call CLOCK_SetDiv() to configure corresponding clock divider for target clock out.
 *
 * 5. Call CLOCK_SetXtalFreq() to set XTAL frequency based on board settings.
 *
 */

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v5.0
processor: MIMXRT1062xxxxA
package_id: MIMXRT1062DVL6A
mcu_data: ksdk2_0
processor_version: 0.0.0
board: MIMXRT1060-EVK
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

#include "clock_config.h"
#include "fsl_iomuxc.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: AHB_CLK_ROOT.outFreq, value: 600 MHz}
- {id: CAN_CLK_ROOT.outFreq, value: 40 MHz}
- {id: CKIL_SYNC_CLK_ROOT.outFreq, value: 32.768 kHz}
- {id: CLK_1M.outFreq, value: 1 MHz}
- {id: CLK_24M.outFreq, value: 24 MHz}
- {id: CSI_CLK_ROOT.outFreq, value: 12 MHz}
- {id: ENET1_TX_CLK.outFreq, value: 2.4 MHz}
- {id: ENET2_125M_CLK.outFreq, value: 1.2 MHz}
- {id: ENET2_TX_CLK.outFreq, value: 1.2 MHz}
- {id: ENET_125M_CLK.outFreq, value: 2.4 MHz}
- {id: ENET_25M_REF_CLK.outFreq, value: 1.2 MHz}
- {id: FLEXIO1_CLK_ROOT.outFreq, value: 30 MHz}
- {id: FLEXIO2_CLK_ROOT.outFreq, value: 30 MHz}
- {id: FLEXSPI2_CLK_ROOT.outFreq, value: 1440/11 MHz}
- {id: FLEXSPI_CLK_ROOT.outFreq, value: 1440/11 MHz}
- {id: GPT1_ipg_clk_highfreq.outFreq, value: 75 MHz}
- {id: GPT2_ipg_clk_highfreq.outFreq, value: 75 MHz}
- {id: IPG_CLK_ROOT.outFreq, value: 150 MHz}
- {id: LCDIF_CLK_ROOT.outFreq, value: 67.5/7 MHz}
- {id: LPI2C_CLK_ROOT.outFreq, value: 60 MHz}
- {id: LPSPI_CLK_ROOT.outFreq, value: 105.6 MHz}
- {id: LVDS1_CLK.outFreq, value: 1.2 GHz}
- {id: MQS_MCLK.outFreq, value: 1080/17 MHz}
- {id: PERCLK_CLK_ROOT.outFreq, value: 75 MHz}
- {id: PLL7_MAIN_CLK.outFreq, value: 24 MHz}
- {id: SAI1_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK1.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK2.outFreq, value: 1080/17 MHz}
- {id: SAI1_MCLK3.outFreq, value: 30 MHz}
- {id: SAI2_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI2_MCLK1.outFreq, value: 1080/17 MHz}
- {id: SAI2_MCLK3.outFreq, value: 30 MHz}
- {id: SAI3_CLK_ROOT.outFreq, value: 1080/17 MHz}
- {id: SAI3_MCLK1.outFreq, value: 1080/17 MHz}
- {id: SAI3_MCLK3.outFreq, value: 30 MHz}
- {id: SEMC_CLK_ROOT.outFreq, value: 75 MHz}
- {id: SPDIF0_CLK_ROOT.outFreq, value: 30 MHz}
- {id: TRACE_CLK_ROOT.outFreq, value: 352/3 MHz}
- {id: UART_CLK_ROOT.outFreq, value: 80 MHz}
- {id: USDHC1_CLK_ROOT.outFreq, value: 198 MHz}
- {id: USDHC2_CLK_ROOT.outFreq, value: 198 MHz}
settings:
- {id: CCM.AHB_PODF.scale, value: '1', locked: true}
- {id: CCM.ARM_PODF.scale, value: '2', locked: true}
- {id: CCM.FLEXSPI2_PODF.scale, value: '2', locked: true}
- {id: CCM.FLEXSPI2_SEL.sel, value: CCM_ANALOG.PLL3_PFD0_CLK}
- {id: CCM.FLEXSPI_PODF.scale, value: '2', locked: true}
- {id: CCM.FLEXSPI_SEL.sel, value: CCM_ANALOG.PLL3_PFD0_CLK}
- {id: CCM.LPSPI_PODF.scale, value: '5', locked: true}
- {id: CCM.PERCLK_PODF.scale, value: '2', locked: true}
- {id: CCM.SEMC_PODF.scale, value: '8'}
- {id: CCM.TRACE_PODF.scale, value: '3', locked: true}
- {id: CCM_ANALOG.PLL1_BYPASS.sel, value: CCM_ANALOG.PLL1}
- {id: CCM_ANALOG.PLL1_PREDIV.scale, value: '1', locked: true}
- {id: CCM_ANALOG.PLL1_VDIV.scale, value: '50', locked: true}
- {id: CCM_ANALOG.PLL2.denom, value: '1', locked: true}
- {id: CCM_ANALOG.PLL2.num, value: '0', locked: true}
- {id: CCM_ANALOG.PLL2_BYPASS.sel, value: CCM_ANALOG.PLL2_OUT_CLK}
- {id: CCM_ANALOG.PLL2_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD0}
- {id: CCM_ANALOG.PLL2_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD1}
- {id: CCM_ANALOG.PLL2_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD2}
- {id: CCM_ANALOG.PLL2_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL2_PFD3}
- {id: CCM_ANALOG.PLL3_BYPASS.sel, value: CCM_ANALOG.PLL3}
- {id: CCM_ANALOG.PLL3_PFD0_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD0}
- {id: CCM_ANALOG.PLL3_PFD0_DIV.scale, value: '33', locked: true}
- {id: CCM_ANALOG.PLL3_PFD0_MUL.scale, value: '18', locked: true}
- {id: CCM_ANALOG.PLL3_PFD1_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD1}
- {id: CCM_ANALOG.PLL3_PFD2_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD2}
- {id: CCM_ANALOG.PLL3_PFD3_BYPASS.sel, value: CCM_ANALOG.PLL3_PFD3}
- {id: CCM_ANALOG.PLL4.denom, value: '50'}
- {id: CCM_ANALOG.PLL4.div, value: '47'}
- {id: CCM_ANALOG.PLL5.denom, value: '1'}
- {id: CCM_ANALOG.PLL5.div, value: '40'}
- {id: CCM_ANALOG.PLL5.num, value: '0'}
- {id: CCM_ANALOG_PLL_ENET_POWERDOWN_CFG, value: 'Yes'}
- {id: CCM_ANALOG_PLL_USB1_POWER_CFG, value: 'Yes'}
sources:
- {id: XTALOSC24M.OSC.outFreq, value: 24 MHz, enabled: true}
- {id: XTALOSC24M.RTC_OSC.outFreq, value: 32.768 kHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

#define DCDC_TARGET_VOLTAGE_1V 1000
#ifndef DCDC_TARGET_VOLTAGE
#define DCDC_TARGET_VOLTAGE DCDC_TARGET_VOLTAGE_1V
#endif

/*
 * TODO:
 *   replace the following two dcdc function with SDK DCDC driver when it's
 * ready
 */
/*******************************************************************************
 * Code for getting current DCDC voltage setting
 ******************************************************************************/
uint32_t dcdc_get_target_voltage()
{
    uint32_t temp = DCDC->DCDC_CTRL1;
    temp          = (temp & DCDC_DCDC_CTRL1_DCDC_VDD1P0CTRL_TRG_MASK) >> DCDC_DCDC_CTRL1_DCDC_VDD1P0CTRL_TRG_SHIFT;
    return (temp * 25 + 600);
}

/*******************************************************************************
 * Code for setting DCDC to target voltage
 ******************************************************************************/
void dcdc_trim_target_1p0(uint32_t target_voltage)
{
    uint8_t trim_value;
    uint32_t temp;

    trim_value = (target_voltage - 600) / 25;
    temp       = DCDC->DCDC_CTRL1;
    temp &= ~DCDC_DCDC_CTRL1_DCDC_VDD1P0CTRL_TRG_MASK;
    temp |= DCDC_DCDC_CTRL1_DCDC_VDD1P0CTRL_TRG(trim_value);
    DCDC->DCDC_CTRL1 = temp;
}

/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    clock_root_config_t rootCfg = {0};

#if !defined(SKIP_DCDC_ADJUSTMENT) || (!SKIP_DCDC_ADJUSTMENT)
    dcdc_trim_target_1p0(DCDC_TARGET_VOLTAGE);
#endif

#if defined(BYPASS_LDO_LPSR) && BYPASS_LDO_LPSR
    CLOCK_ANATOP_LdoLpsrAnaBypassOn();
    CLOCK_ANATOP_LdoLpsrDigBypassOn();
#endif

#if __CORTEX_M == 7
    /* ARM PLL 996 MHz. */
    const clock_arm_pll_config_t armPllConfig = {
        .postDivider = kCLOCK_PllPostDiv2, .loopDivider = 133, /* 116: 696M
                                                                * 133: 798M
                                                                * 150: 900M
                                                                * 166: 996M
                                                                */
    };
#endif

    /* SYS PLL2 528MHz. */
    const clock_sys_pll_config_t sysPllConfig = {
        .loopDivider = 1,
        /* Using 24Mhz OSC */
        .mfn = 0,
        .mfi = 22,
    };

#if __CORTEX_M == 4
    const clock_sys_pll3_config_t sysPll3Config = {
        .divSelect = 3,
    };
#endif
    /* PLL LDO shall be enabled first before enable PLLs */
    CLOCK_EnableOsc24M();
    CLOCK_EnablePllLdo();

#if __CORTEX_M == 7
    rootCfg.mux = 0;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_M7, &rootCfg);
    CLOCK_SetRootClock(kCLOCK_Root_M7_Systick, &rootCfg);

    /* ARMPll: 996M */
    CLOCK_InitArmPll(&armPllConfig);
    /* Configure M7 */
    rootCfg.mux = 4;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_M7, &rootCfg);

    /* Configure M7 Systick running at 100K */
    rootCfg.mux = 0;
    rootCfg.div = 239;
    CLOCK_SetRootClock(kCLOCK_Root_M7_Systick, &rootCfg);
#endif

#if __CORTEX_M == 4
    rootCfg.mux = 0;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_M4, &rootCfg);
    CLOCK_SetRootClock(kCLOCK_Root_Bus_Lpsr, &rootCfg);

    /* SysPll3Pfd3: 480M */
    CLOCK_InitSysPll3(&sysPll3Config);
    CLOCK_InitPfd(kCLOCK_Pll_SysPll3, kCLOCK_Pfd3, 18);

    /* Configure M4 using SysPll3Pfd3 divided by 1 */
    rootCfg.mux = 4;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_M4, &rootCfg);

    /* SysPll3 */
    rootCfg.mux = 5;
    rootCfg.div = 3;
    CLOCK_SetRootClock(kCLOCK_Root_Bus_Lpsr, &rootCfg);
#endif
    CLOCK_InitSysPll2(&sysPllConfig);

#if DEBUG_CONSOLE_UART_INDEX == 1
    /* Configure Lpuart1 using Osc48MDiv2 */
    rootCfg.mux = 0;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart1, &rootCfg);
#else
    /* Configure Lpuart2 using Osc48MDiv2 */
    rootCfg.mux = 0;
    rootCfg.div = 0;
    CLOCK_SetRootClock(kCLOCK_Root_Lpuart2, &rootCfg);
#endif

    CLOCK_EnableOscRc400M();
    /* Configure Semc using OscRc400M divided by 2 */
    rootCfg.mux = 2;
    rootCfg.div = 1;
    CLOCK_SetRootClock(kCLOCK_Root_Semc, &rootCfg);

#if __CORTEX_M == 7
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M7);
#else
    SystemCoreClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M4);
#endif
}
