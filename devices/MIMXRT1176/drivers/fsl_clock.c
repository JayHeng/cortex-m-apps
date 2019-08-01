/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "fsl_anatop_ai.h"
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* To make full use of CM7 hardware FPU, use double instead of uint64_t in clock driver to
achieve better performance, it is depend on the IDE Floating point settings, if double precision is selected
in IDE, clock_64b_t will switch to double type automatically. only support IAR and MDK here */
#if __FPU_USED

#if ((defined(__ICCARM__)) || (defined(__GNUC__)))

#if (__ARMVFP__ >= __ARMFPV5__) && \
    (__ARM_FP == 0xE) /*0xe implies support for half, single and double precision operations*/
typedef double clock_64b_t;
#else
typedef uint64_t clock_64b_t;
#endif

#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)

#if defined __TARGET_FPU_FPV5_D16
typedef double clock_64b_t;
#else
typedef uint64_t clock_64b_t;
#endif

#else
typedef uint64_t clock_64b_t;
#endif

#else
typedef uint64_t clock_64b_t;
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* External XTAL (OSC) clock frequency. */
volatile uint32_t g_xtalFreq;
/* External RTC XTAL clock frequency. */
volatile uint32_t g_rtcXtalFreq;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* CM7 FBB config */
void CLOCK_ANATOP_Cm7FbbCfg(void)/* ldo_pll enable */
{
    /* anatop_cm7_fbb_config! */;
    CLOCK_ANATOP_InitWbiasCfg(1, 0);
    CLOCK_ANATOP_WbCfg1p8Cfg(0x01EE);
    /* This is to enable PW/NW regulator path to CM7 FBB and disable regulator
     * path to LPSR/SOC RBB. Since RVT does not support FBB.
     */
    CLOCK_ANATOP_WbPwrSwEn1p8(1);
    /* Enable wbias */
    CLOCK_ANATOP_EnableWbias(1);
}

void CLOCK_ANATOP_InitWbiasCfg(uint32_t fbb_on, uint32_t rbb_on)
{
    if(fbb_on==1)
    {
        /* This is to select FBB regulator target voltage as 0.6V for CM7 LVT. */
        /* Set wb_nw_lvl_1p8 and wb_pw_lvl_1p8 in PMU_BIAS_CTRL to 4b’0001! */;
        CLOCK_ANATOP_WbiasPwlvl_1p8( 0x1);
        CLOCK_ANATOP_WbiasNwlvl_1p8( 0x1);
    }
    else if(rbb_on==1)
    {
        CLOCK_ANATOP_WbiasPwlvl_1p8( 0x5);
        CLOCK_ANATOP_WbiasNwlvl_1p8( 0x5);
    }
    else
    {
        CLOCK_ANATOP_WbiasPwlvl_1p8( 0x0);
        CLOCK_ANATOP_WbiasNwlvl_1p8( 0x0);
    }

}

void CLOCK_ANATOP_WbiasPwlvl_1p8(uint32_t pw_lvl_1p8)
{
    uint32_t  temp;
    /* If wb_en_1p8=1, these bits set the vbb_rpw voltage level! */;
    /* ANATOP_RWMS(PMU_BIAS_CTRL, pw_lvl_1p8, PMU_BIAS_CTRL_wb_pw_lvl_1p8_MASK, PMU_BIAS_CTRL_wb_pw_lvl_1p8_SHIFT); */
    temp = ANADIG_PMU->PMU_BIAS_CTRL;
    temp &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_pw_lvl_1p8_MASK;
    temp |= (pw_lvl_1p8<<ANADIG_PMU_PMU_BIAS_CTRL_wb_pw_lvl_1p8_SHIFT)&ANADIG_PMU_PMU_BIAS_CTRL_wb_pw_lvl_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL = temp;
    /* anatop_wbias_pw_lvl_1p8: finished. */;
}

void CLOCK_ANATOP_WbiasNwlvl_1p8(uint32_t nw_lvl_1p8)
{
    uint32_t  temp;
    /* If wb_en_1p8=1, these bits set the vbb_rnw voltage level! */;
    /* ANATOP_RWMS(PMU_BIAS_CTRL, nw_lvl_1p8, PMU_BIAS_CTRL_wb_nw_lvl_1p8_MASK, PMU_BIAS_CTRL_wb_nw_lvl_1p8_SHIFT); */
    temp = ANADIG_PMU->PMU_BIAS_CTRL;
    temp &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_nw_lvl_1p8_MASK;
    temp |= (nw_lvl_1p8<<ANADIG_PMU_PMU_BIAS_CTRL_wb_nw_lvl_1p8_SHIFT)&ANADIG_PMU_PMU_BIAS_CTRL_wb_nw_lvl_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL = temp;
    /* anatop_wbias_nw_lvl_1p8: finished. */;
}

void CLOCK_ANATOP_WbCfg1p8Cfg(uint32_t wb_cfg_1p8)
{
    uint32_t  temp;
    /* anatop_wb_cfg_1p8_cfg. */;
    /* Wb_cfg_1p8<0> = 0, NW tracking regulator is used */
    /* Wb_cfg_1p8<1> = 1, LVT selection */
    /* Wb_cfg_1p8<4:2> = 3’b011, drive strength for M7 FBB */
    /* Wb_cfg_1p8<5:8> = 4’b1111 use osc frequency */
    /* ANATOP_RWMS(PMU_BIAS_CTRanatop_wb_cfg_1p8_cfgL, wb_cfg_1p8, PMU_BIAS_CTRL_wb_cfg_1p8_MASK, PMU_BIAS_CTRL_wb_cfg_1p8_SHIFT); */
    temp = ANADIG_PMU->PMU_BIAS_CTRL;
    temp &= ~ANADIG_PMU_PMU_BIAS_CTRL_wb_cfg_1p8_MASK;
    temp |= (wb_cfg_1p8<<ANADIG_PMU_PMU_BIAS_CTRL_wb_cfg_1p8_SHIFT)&ANADIG_PMU_PMU_BIAS_CTRL_wb_cfg_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL = temp;
}

void CLOCK_ANATOP_WbPwrSwEn1p8(uint32_t wb_pwr_sw_en_1p8)
{
    uint32_t  temp;
    /* Set wb_pwr_sw_en_1p8 in PMU_BIAS_CTRL2! */;
    /* ANATOP_RWMS(PMU_BIAS_CTRL2, wb_pwr_sw_en_1p8, PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_MASK, PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_SHIFT); */
    temp = ANADIG_PMU->PMU_BIAS_CTRL2;
    temp &= ~ANADIG_PMU_PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_MASK;
    temp |= (wb_pwr_sw_en_1p8<<ANADIG_PMU_PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_SHIFT)&ANADIG_PMU_PMU_BIAS_CTRL2_wb_pwr_sw_en_1p8_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL2 = temp;
}

void CLOCK_ANATOP_EnableWbias(uint32_t enable)
{
    uint32_t  temp;
    assert(enable <= 1);
    /* ANATOP_RWMS(PMU_BIAS_CTRL2, wb_en, PMU_BIAS_CTRL2_wb_en_MASK, PMU_BIAS_CTRL2_wb_en_SHIFT); */
    temp = ANADIG_PMU->PMU_BIAS_CTRL2;
    temp &= ~ANADIG_PMU_PMU_BIAS_CTRL2_wb_en_MASK;
    temp |= (enable << ANADIG_PMU_PMU_BIAS_CTRL2_wb_en_SHIFT) & ANADIG_PMU_PMU_BIAS_CTRL2_wb_en_MASK;
    ANADIG_PMU->PMU_BIAS_CTRL2 = temp;
}

void CLOCK_EnablePllLdo()
{
    ANADIG_MISC->VDDSOC_AI_CTRL = 0;
    ANADIG_MISC->VDDSOC_AI_WDATA = 0x0105;
    ANADIG_PMU->PMU_LDO_PLL |= 0x10000;
    SDK_DelayAtLeastUs(100);
    ANADIG_PMU->PMU_POWER_DETECT_CTRL = 0x100;
    SDK_DelayAtLeastUs(1);
    ANADIG_PMU->PMU_REF_CTRL |= 0x10;
}

#define ARM_PLL_DIV_SEL_MIN 104
#define ARM_PLL_DIV_SEL_MAX 208

void CLOCK_InitArmPll(const clock_arm_pll_config_t *config)
{
    assert((config->loopDivider <= ARM_PLL_DIV_SEL_MAX) &&
           (config->loopDivider >= ARM_PLL_DIV_SEL_MIN));

    uint32_t reg = ANADIG_PLL->PLL_ARM_CTRL & (~ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_STABLE_MASK);

    if (reg & (ANADIG_PLL_PLL_ARM_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_ARM_CTRL_ENABLE_CLK_MASK))
    {
        /* Power down the PLL. */
        reg &= ~(ANADIG_PLL_PLL_ARM_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_ARM_CTRL_ENABLE_CLK_MASK);
        reg |= ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_GATE_MASK;
        ANADIG_PLL->PLL_ARM_CTRL = reg;
    }

    /* Set the configuration. */
    reg &= ~(ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT_MASK | ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL_MASK);
    reg |= (ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT(config->loopDivider) | ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL(config->postDivider))
            | ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_GATE_MASK | ANADIG_PLL_PLL_ARM_CTRL_POWERUP_MASK;
    ANADIG_PLL->PLL_ARM_CTRL = reg;
    __DSB();
    __ISB();
    SDK_DelayAtLeastUs(30);

    /* Wait for the PLL stable, */
    while (0U == (ANADIG_PLL->PLL_ARM_CTRL & ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_STABLE_MASK))
    {
    }

    /* Enable and ungate the clock. */
    reg |= ANADIG_PLL_PLL_ARM_CTRL_ENABLE_CLK_MASK;
    reg &= ~ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_GATE_MASK;
    ANADIG_PLL->PLL_ARM_CTRL = reg;
}

void CLOCK_DeinitArmPll(void)
{
    uint32_t reg = ANADIG_PLL->PLL_ARM_CTRL & (~ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_STABLE_MASK);

    reg &= ~(ANADIG_PLL_PLL_ARM_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_ARM_CTRL_ENABLE_CLK_MASK);
    reg |= ANADIG_PLL_PLL_ARM_CTRL_PLL_ARM_GATE_MASK;
    ANADIG_PLL->PLL_ARM_CTRL = reg;
}

#define SYS_PLL2_DIV_SEL_MIN 54
#define SYS_PLL2_DIV_SEL_MAX 108
/* 528PLL */
void CLOCK_InitSysPll2(const clock_sys_pll_config_t *config)
{
    uint32_t reg;

    /* Gate all PFDs */
    ANADIG_PLL->PLL_528_PFD |= ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD1_DIV1_CLKGATE(1)
                            | ANADIG_PLL_PLL_528_PFD_PFD2_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD3_DIV1_CLKGATE(1);

    reg = ANADIG_PLL->PLL_528_CTRL;
    if (reg & (ANADIG_PLL_PLL_528_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_528_CTRL_ENABLE_CLK_MASK))
    {
        /* Power down the PLL. */
        reg &= ~(ANADIG_PLL_PLL_528_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_528_CTRL_ENABLE_CLK_MASK);
        reg |= ANADIG_PLL_PLL_528_CTRL_PLL_528_GATE_MASK;
        ANADIG_PLL->PLL_528_CTRL = reg;
    }

    /* Config PLL */
    ANADIG_PLL->PLL_528_MFD = 0x0FFFFFFFUL;
    ANADIG_PLL->PLL_528_MFN = config->mfn;
    ANADIG_PLL->PLL_528_MFI = config->mfi;

    /* REG_EN = 1, GATE = 1, DIV_SEL = 0, POWERUP = 0 */
    reg = ANADIG_PLL_PLL_528_CTRL_PLL_REG_EN(1) | ANADIG_PLL_PLL_528_CTRL_PLL_528_GATE(1);
    ANADIG_PLL->PLL_528_CTRL = reg;
    /* Wait until LDO is stable */
    SDK_DelayAtLeastUs(30);

    /* REG_EN = 1, GATE = 1, DIV_SEL = 0, POWERUP = 1, HOLDRING_OFF = 1 */
    reg |= ANADIG_PLL_PLL_528_CTRL_POWERUP(1) | ANADIG_PLL_PLL_528_CTRL_HOLD_RING_OFF_MASK;
    ANADIG_PLL->PLL_528_CTRL = reg;
    SDK_DelayAtLeastUs(250);

    /* REG_EN = 1, GATE = 1, DIV_SEL = 0, POWERUP = 1, HOLDRING_OFF = 0 */
    reg &= ~ANADIG_PLL_PLL_528_CTRL_HOLD_RING_OFF_MASK;
    ANADIG_PLL->PLL_528_CTRL = reg;
    /* Wait for PLL stable */
    while(ANADIG_PLL_PLL_528_CTRL_PLL_528_STABLE_MASK !=
            (ANADIG_PLL->PLL_528_CTRL & ANADIG_PLL_PLL_528_CTRL_PLL_528_STABLE_MASK))
    {
    }

    /* REG_EN = 1, GATE = 1, DIV_SEL = 0, POWERUP = 1, HOLDRING_OFF = 0, CLK = 1*/
    reg |= ANADIG_PLL_PLL_528_CTRL_ENABLE_CLK_MASK;
    ANADIG_PLL->PLL_528_CTRL = reg;

    /* REG_EN = 1, GATE = 0, DIV_SEL = 0, POWERUP = 1, HOLDRING_OFF = 0, CLK = 1*/
    reg &= ~ANADIG_PLL_PLL_528_CTRL_PLL_528_GATE_MASK;
    ANADIG_PLL->PLL_528_CTRL = reg;
    ANADIG_PLL->PLL_528_PFD &= ~(ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD1_DIV1_CLKGATE(1)
                            | ANADIG_PLL_PLL_528_PFD_PFD2_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD3_DIV1_CLKGATE(1));

}

void CLOCK_DeinitSysPll2(void)
{
    ANADIG_PLL->PLL_528_PFD |= ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD1_DIV1_CLKGATE(1)
                            | ANADIG_PLL_PLL_528_PFD_PFD2_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_528_PFD_PFD3_DIV1_CLKGATE(1);

    ANADIG_PLL->PLL_528_CTRL |= ANADIG_PLL_PLL_528_CTRL_PLL_528_GATE_MASK;
    ANADIG_PLL->PLL_528_CTRL &= ~(ANADIG_PLL_PLL_528_CTRL_ENABLE_CLK_MASK | ANADIG_PLL_PLL_528_CTRL_POWERUP_MASK | ANADIG_PLL_PLL_528_CTRL_PLL_REG_EN_MASK);
}

#define PFD_FRAC_MIN 12
#define PFD_FRAC_MAX 35
void CLOCK_InitPfd(clock_pll_t pll, clock_pfd_t pfd, uint8_t frac)
{
    volatile uint32_t *pfdCtrl, *pfdUpdate, stable;

    assert(frac <= PFD_FRAC_MAX && frac >= PFD_FRAC_MIN);

    switch (pll)
    {
        case kCLOCK_Pll_SysPll2:
            pfdCtrl = &ANADIG_PLL->PLL_528_PFD;
            break;
        case kCLOCK_Pll_SysPll3:
            pfdCtrl = &ANADIG_PLL->PLL_480_PFD;
            pfdUpdate = &ANADIG_PLL->PLL_480_UPDATE;
            break;
        default:
            assert(false);
            break;
    }
    stable = *pfdCtrl & (ANADIG_PLL_PLL_528_PFD_PFD0_STABLE_MASK << (8 * (uint32_t) pfd));
    *pfdCtrl |= (ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE_MASK << (8 * (uint32_t) pfd));

    /* all pfds support to be updated on-the-fly after corresponding PLL is stable */
    *pfdCtrl &= ~(ANADIG_PLL_PLL_528_PFD_PFD0_FRAC_MASK << (8 * (uint32_t) pfd));
    *pfdCtrl |= (ANADIG_PLL_PLL_528_PFD_PFD0_FRAC(frac) << (8 * (uint32_t) pfd));

    *pfdUpdate ^= (ANADIG_PLL_PLL_528_UPDATE_PFD0_UPDATE_MASK << (uint32_t) pfd);
    *pfdCtrl &= ~(ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE_MASK << (8 * (uint32_t) pfd));
    /* Wait for stablizing */
    while (stable == (*pfdCtrl & (ANADIG_PLL_PLL_528_PFD_PFD0_STABLE_MASK << (8 * (uint32_t) pfd))))
    {
    }
}

#ifdef PLLPFD_FREQ_NOT_FROM_OBS
uint32_t CLOCK_GetPfdFreq(clock_pll_t pll, clock_pfd_t pfd)
{
    uint32_t pllFreq, frac;

    switch (pll)
    {
        case kCLOCK_Pll_SysPll2:
            frac = (ANADIG_PLL->PLL_528_PFD & (ANADIG_PLL_PLL_528_PFD_PFD0_FRAC_MASK << (8 * (uint32_t)pfd)));
            pllFreq = SYS_PLL2_FREQ;
            break;
        case kCLOCK_Pll_SysPll3:
            frac = (ANADIG_PLL->PLL_480_PFD & (ANADIG_PLL_PLL_480_PFD_PFD0_FRAC_MASK << (8 * (uint32_t)pfd)));
            pllFreq = SYS_PLL3_FREQ;
            break;
        default:
            assert(false);
            break;
    }

    frac = frac >> (8 * (uint32_t)pfd);
    assert((frac >= PFD_FRAC_MIN) && (frac <= PFD_FRAC_MAX));
    return pllFreq / frac * 18;
}
#else
uint32_t CLOCK_GetPfdFreq(clock_pll_t pll, clock_pfd_t pfd)
{
    uint32_t freq;
    switch (pll)
    {
        case kCLOCK_Pll_SysPll2:
            /* PLL_528_PFD0 OBS index starts from 234 */
            freq = CLOCK_GetFreqFromObs(pfd + 234, 2);
            break;
        case kCLOCK_Pll_SysPll3:
            /* PLL_480_PFD0 OBS index starts from 241 */
            freq = CLOCK_GetFreqFromObs(pfd + 241, 2);
            break;
        default:
            assert(false);
    }
    return freq;
}
#endif

#define SYS_PLL3_DIV_SEL_MIN 54
#define SYS_PLL3_DIV_SEL_MAX 108
/* 480PLL */
void CLOCK_InitSysPll3(const clock_sys_pll3_config_t *config)
{
    uint32_t reg;

    /* Gate all PFDs */
    ANADIG_PLL->PLL_480_PFD |= ANADIG_PLL_PLL_480_PFD_PFD0_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_480_PFD_PFD1_DIV1_CLKGATE(1)
                            | ANADIG_PLL_PLL_480_PFD_PFD2_DIV1_CLKGATE(1) | ANADIG_PLL_PLL_480_PFD_PFD3_DIV1_CLKGATE(1);
    /*
     * 1. configure PLL registres
     * 2. Enable internal LDO
     * 3. Wati LDO stable
     * 4. Power up PLL, assert hold_ring_off (only needed for avpll)
     * 5. At half lock time, de-asserted hold_ring_off (only needed for avpll)
     * 6. Wait PLL lock
     * 7. Enable clock output, release pfd_gate
     */
    reg = ANADIG_PLL_PLL_480_CTRL_DIV_SELECT(config->divSelect) | ANADIG_PLL_PLL_480_CTRL_PLL_REG_EN(1)
        | ANADIG_PLL_PLL_480_CTRL_PLL_480_GATE(1);
    ANADIG_PLL->PLL_480_CTRL = reg;
    SDK_DelayAtLeastUs(30);

    reg |= ANADIG_PLL_PLL_480_CTRL_POWERUP(1) | ANADIG_PLL_PLL_480_CTRL_HOLD_RING_OFF_MASK;
    ANADIG_PLL->PLL_480_CTRL = reg;
    SDK_DelayAtLeastUs(30);

    reg &= ~ANADIG_PLL_PLL_480_CTRL_HOLD_RING_OFF_MASK;
    ANADIG_PLL->PLL_480_CTRL = reg;

    /* Wait for PLL stable */
    while(ANADIG_PLL_PLL_480_CTRL_PLL_480_STABLE_MASK !=
            (ANADIG_PLL->PLL_480_CTRL & ANADIG_PLL_PLL_480_CTRL_PLL_480_STABLE_MASK))
    {
    }

    reg |= ANADIG_PLL_PLL_480_CTRL_ENABLE_CLK(1) | ANADIG_PLL_PLL_480_CTRL_PLL_480_DIV2(1);
    ANADIG_PLL->PLL_480_CTRL = reg;

    reg &= ~ANADIG_PLL_PLL_480_CTRL_PLL_480_GATE_MASK;
    ANADIG_PLL->PLL_480_CTRL = reg;
}

void CLOCK_DeinitSysPll3(void)
{
}

void ANATOP_PllSetPower(anatop_ai_itf_t itf, bool enable)
{
    ANATOP_AI_Write(itf, enable ? 0x04 : 0x08, 0x4000 | (enable ? 0x2000 : 0));
}

void ANATOP_PllBypass(anatop_ai_itf_t itf, bool bypass)
{
    ANATOP_AI_Write(itf, bypass ? 0x04 : 0x08, 0x10000);
}

void ANATOP_PllEnablePllReg(anatop_ai_itf_t itf, bool enable)
{
    ANATOP_AI_Write(itf, enable ? 0x04 : 0x08, 0x400000);
}

void ANATOP_PllHoldRingOff(anatop_ai_itf_t itf, bool off)
{
    ANATOP_AI_Write(itf, off ? 0x04 : 0x08, 0x2000);
}

void ANATOP_PllToggleHoldRingOff(anatop_ai_itf_t itf, uint32_t delay_us)
{
    ANATOP_PllHoldRingOff(itf, true);
    SDK_DelayAtLeastUs(delay_us);
    ANATOP_PllHoldRingOff(itf, false);

}

void ANATOP_PllEnableClk(anatop_ai_itf_t itf, bool enable)
{
    ANATOP_AI_Write(itf, enable ? 0x04 : 0x08, 0x8000);
}

void ANATOP_PllConfigure(anatop_ai_itf_t itf, uint8_t div,
                        uint32_t numer, uint8_t post_div, uint32_t denom)
{
    if (itf != kAI_Itf_1g)
    {
        ANATOP_PllSetPower(itf, false);
    }
    ANATOP_AI_Write(itf, 0x30, denom);
    ANATOP_AI_Write(itf, 0x20, numer);
    ANATOP_AI_WriteWithMaskShift(itf, 0x00, div, 0x7f, 0);
    if (itf != kAI_Itf_1g)
    {
        ANATOP_AI_WriteWithMaskShift(itf, 0x00, post_div, 0xE000000, 25);
    }
    ANATOP_PllEnablePllReg(itf, true);
    SDK_DelayAtLeastUs(100);
    ANATOP_PllSetPower(itf, true);
}

void ANATOP_AudioPllGate(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_AUDIO_CTRL &= ~ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_GATE_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_AUDIO_CTRL |= ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_GATE_MASK;
    }
}

void ANATOP_AudioPllSwEnClk(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_AUDIO_CTRL &= ~ANADIG_PLL_PLL_AUDIO_CTRL_ENABLE_CLK_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_AUDIO_CTRL |= ANADIG_PLL_PLL_AUDIO_CTRL_ENABLE_CLK_MASK;
    }
}

status_t CLOCK_InitAudioPllWithFreq(uint32_t freqInMhz)
{
    clock_audio_pll_config_t config = {0};
    assert(g_xtalFreq);
    uint32_t refFreq = (g_xtalFreq / 1000000) * 54; /* MHz */

/*
 * AUDIO_PLL  (20.3125MHZ--- 1300MHZ configureable )
 * freq = osc_freq * (loopDivider + numerator / (2^28 - 1) ) / 2^postDivider
 *   - loopDivider:  27---54
 *   - postDivider: 0---5
 *   - numerator is a signed number, 30 bit,  numer[29] is the sign bit, such as +1--->0x00000001; -1---> 0x20000001
 * such as: div_sel = 27, numer = 0x026EEEEF, post_div =0, fref = 24.0MHZ, output_fre =24.0*(27 + 40824559/(2^28 - 1))/2^0 = 651.65M
 * such as: div_sel = 33, numer = 0x0F0AAAAA, post_div =0, fref = 19.2MHZ, output_fre =19.2*(33 + 252357290/(2^28 - 1))/2^0= 651.65M
 */

    config.denominator = 0x0FFFFFFF;

    if (freqInMhz >= refFreq)
    {
        config.postDivider = 0;
        config.loopDivider = 54;
        config.numerator = 0;
    }
    else if (freqInMhz >= (refFreq >> 0))
    {
        config.postDivider = 0;
        config.loopDivider = freqInMhz / 24;
        config.numerator = (config.denominator / 24) * (freqInMhz % 24);
    }
    else if (freqInMhz >= (refFreq >> 1))
    {
        config.postDivider = 1;
        config.loopDivider = freqInMhz / 12;
        config.numerator = (config.denominator / 12) * (freqInMhz % 12);
    }
    else if (freqInMhz >= (refFreq >> 2))
    {
        config.postDivider = 2;
        config.loopDivider = freqInMhz / 6;
        config.numerator = (config.denominator / 6) * (freqInMhz % 6);
    }
    else if (freqInMhz >= (refFreq >> 3))
    {
        config.postDivider = 3;
        config.loopDivider = freqInMhz / 3;
        config.numerator = (config.denominator / 3) * (freqInMhz % 3);
    }
    else if (freqInMhz >= (refFreq >> 4))
    {
        config.postDivider = 4;
        config.loopDivider = freqInMhz * 2 / 3;
        config.numerator = (config.denominator * 2 / 3) * (freqInMhz * 2 % 3);
    }
    else if (freqInMhz >= (refFreq >> 5))
    {
        config.postDivider = 5;
        config.loopDivider = freqInMhz * 4 / 3;
        config.numerator = (config.denominator * 4 / 3) * (freqInMhz * 4 % 3);
    }
    else
    {
        return kStatus_Fail;
    }

    CLOCK_InitAudioPll(&config);
    return kStatus_Success;
}

void CLOCK_InitAudioPll(const clock_audio_pll_config_t *config)
{
    uint32_t reg;
    bool pllStable = false;

    reg = ANADIG_PLL->PLL_AUDIO_CTRL;
    if ((reg & (ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_STABLE_MASK | ANADIG_PLL_PLL_AUDIO_CTRL_ENABLE_CLK_MASK)) 
            == (ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_STABLE_MASK | ANADIG_PLL_PLL_AUDIO_CTRL_ENABLE_CLK_MASK))
    {
        pllStable = true;
    }
    /* bypass pll */
    ANATOP_PllBypass(kAI_Itf_Audio, true);

    if (pllStable)
    {
        /* sw enable clock */
        ANATOP_AudioPllSwEnClk(false);
        /* gate clock */
        ANATOP_AudioPllGate(true);
        ANATOP_PllEnableClk(kAI_Itf_Audio, false);
        ANATOP_PllSetPower(kAI_Itf_Audio, false);
        ANATOP_PllEnablePllReg(kAI_Itf_Audio, false);
    }

    ANATOP_AudioPllSwEnClk(true);
    /* configure pll */
    ANATOP_PllConfigure(kAI_Itf_Audio, config->loopDivider, config->numerator,
                        config->postDivider, config->denominator);

    /* toggle hold ring off */
    ANATOP_PllToggleHoldRingOff(kAI_Itf_Audio, 225);

    /* wait pll stable */
    do
    {
        reg = ANADIG_PLL->PLL_AUDIO_CTRL;
    } while ((reg & ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_STABLE_MASK)
            != ANADIG_PLL_PLL_AUDIO_CTRL_PLL_AUDIO_STABLE_MASK); /* wait for PLL locked */

    /* enabled clock */
    ANATOP_PllEnableClk(kAI_Itf_Audio, true);

    /* ungate clock */
    ANATOP_AudioPllGate(false);

    ANATOP_PllBypass(kAI_Itf_Audio, false);
}

void CLOCK_DeinitAudioPll(void)
{
    ANATOP_AudioPllSwEnClk(false);
    ANATOP_AudioPllGate(true);
    ANATOP_PllEnableClk(kAI_Itf_Audio, false);
    ANATOP_PllSetPower(kAI_Itf_Audio, false);
    ANATOP_PllEnablePllReg(kAI_Itf_Audio, false);
}

void ANATOP_VideoPllGate(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_VIDEO_CTRL &= ~ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_GATE_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_VIDEO_CTRL |= ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_GATE_MASK;
    }
}

void ANATOP_VideoPllSwEnClk(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_VIDEO_CTRL &= ~ANADIG_PLL_PLL_VIDEO_CTRL_ENABLE_CLK_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_VIDEO_CTRL |= ANADIG_PLL_PLL_VIDEO_CTRL_ENABLE_CLK_MASK;
    }
}

status_t CLOCK_InitVideoPllWithFreq(uint32_t freqInMhz)
{
    clock_video_pll_config_t config = {0};
    assert(g_xtalFreq);
    uint32_t refFreq = (g_xtalFreq / 1000000) * 54; /* MHz */

/*
 * VIDEO_PLL  (20.3125MHZ--- 1300MHZ configureable )
 * freqInMhz = osc_freq * (loopDivider + numerator / (2^28 - 1) ) / 2^postDivider
 *   - loopDivider:  27---54
 *   - postDivider: 0---5
 *   - numerator is a signed number, 30 bit,  numer[29] is the sign bit, such as +1--->0x00000001; -1---> 0x20000001
 * such as: div_sel = 27, numer = 0x026EEEEF, post_div =0, fref = 24.0MHZ, output_fre =24.0*(27 + 40824559/(2^28 - 1))/2^0 = 651.65M
 * such as: div_sel = 33, numer = 0x0F0AAAAA, post_div =0, fref = 19.2MHZ, output_fre =19.2*(33 + 252357290/(2^28 - 1))/2^0= 651.65M
 */

    config.denominator = 0x0FFFFFFF;

    if (freqInMhz >= refFreq)
    {
        config.postDivider = 0;
        config.loopDivider = 54;
        config.numerator = 0;
    }
    else if (freqInMhz >= (refFreq >> 0))
    {
        config.postDivider = 0;
        config.loopDivider = freqInMhz / 24;
        config.numerator = (config.denominator / 24) * (freqInMhz % 24);
    }
    else if (freqInMhz >= (refFreq >> 1))
    {
        config.postDivider = 1;
        config.loopDivider = freqInMhz / 12;
        config.numerator = (config.denominator / 12) * (freqInMhz % 12);
    }
    else if (freqInMhz >= (refFreq >> 2))
    {
        config.postDivider = 2;
        config.loopDivider = freqInMhz / 6;
        config.numerator = (config.denominator / 6) * (freqInMhz % 6);
    }
    else if (freqInMhz >= (refFreq >> 3))
    {
        config.postDivider = 3;
        config.loopDivider = freqInMhz / 3;
        config.numerator = (config.denominator / 3) * (freqInMhz % 3);
    }
    else if (freqInMhz >= (refFreq >> 4))
    {
        config.postDivider = 4;
        config.loopDivider = freqInMhz * 2 / 3;
        config.numerator = (config.denominator * 2 / 3) * (freqInMhz * 2 % 3);
    }
    else if (freqInMhz >= (refFreq >> 5))
    {
        config.postDivider = 5;
        config.loopDivider = freqInMhz * 4 / 3;
        config.numerator = (config.denominator * 4 / 3) * (freqInMhz * 4 % 3);
    }
    else
    {
        return kStatus_Fail;
    }

    CLOCK_InitVideoPll(&config);
    return kStatus_Success;
}

void CLOCK_InitVideoPll(const clock_video_pll_config_t *config)
{
    uint32_t reg;
    bool pllStable = false;

    reg = ANADIG_PLL->PLL_VIDEO_CTRL;
    if ((reg & (ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_STABLE_MASK | ANADIG_PLL_PLL_VIDEO_CTRL_ENABLE_CLK_MASK)) 
            == (ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_STABLE_MASK | ANADIG_PLL_PLL_VIDEO_CTRL_ENABLE_CLK_MASK))
    {
        pllStable = true;
    }
    /* bypass pll */
    ANATOP_PllBypass(kAI_Itf_Video, true);

    if (pllStable)
    {
        /* sw enable clock */
        ANATOP_VideoPllSwEnClk(false);
        /* gate clock */
        ANATOP_VideoPllGate(true);
        ANATOP_PllEnableClk(kAI_Itf_Video, false);
        ANATOP_PllSetPower(kAI_Itf_Video, false);
        ANATOP_PllEnablePllReg(kAI_Itf_Video, false);
    }

    ANATOP_VideoPllSwEnClk(true);
    /* configure pll */
    ANATOP_PllConfigure(kAI_Itf_Video, config->loopDivider, config->numerator,
                        config->postDivider, config->denominator);

    /* toggle hold ring off */
    ANATOP_PllToggleHoldRingOff(kAI_Itf_Video, 225);

    /* wait pll stable */
    do
    {
        reg = ANADIG_PLL->PLL_VIDEO_CTRL;
    } while ((reg & ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_STABLE_MASK)
            != ANADIG_PLL_PLL_VIDEO_CTRL_PLL_VIDEO_STABLE_MASK); /* wait for PLL locked */

    /* enabled clock */
    ANATOP_PllEnableClk(kAI_Itf_Video, true);

    /* ungate clock */
    ANATOP_VideoPllGate(false);

    ANATOP_PllBypass(kAI_Itf_Video, false);
}

void CLOCK_DeinitVideoPll(void)
{
    ANATOP_VideoPllSwEnClk(false);
    ANATOP_VideoPllGate(true);
    ANATOP_PllEnableClk(kAI_Itf_Video, false);
    ANATOP_PllSetPower(kAI_Itf_Video, false);
    ANATOP_PllEnablePllReg(kAI_Itf_Video, false);
}

void ANATOP_SysPll1Gate(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_1G_CTRL &= ~ANADIG_PLL_PLL_1G_CTRL_PLL_1G_GATE_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_1G_CTRL |= ANADIG_PLL_PLL_1G_CTRL_PLL_1G_GATE_MASK;
    }
}

void ANATOP_SysPll1Div2En(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_1G_CTRL &= ~ANADIG_PLL_PLL_1G_CTRL_pll_1g_div2_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_1G_CTRL |= ANADIG_PLL_PLL_1G_CTRL_pll_1g_div2_MASK;
    }
}

void ANATOP_SysPll1Div5En(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_1G_CTRL &= ~ANADIG_PLL_PLL_1G_CTRL_pll_1g_div5_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_1G_CTRL |= ANADIG_PLL_PLL_1G_CTRL_pll_1g_div5_MASK;
    }
}

void ANATOP_SysPll1SwEnClk(bool enable)
{
    if (!enable)
    {
        ANADIG_PLL->PLL_1G_CTRL &= ~ANADIG_PLL_PLL_1G_CTRL_ENABLE_CLK_MASK;
    }
    else
    {
        ANADIG_PLL->PLL_1G_CTRL |= ANADIG_PLL_PLL_1G_CTRL_ENABLE_CLK_MASK;
    }
}

void ANATOP_SysPll1WaitStable(void)
{
    uint32_t reg;
    do
    {
        reg = ANADIG_PLL->PLL_1G_CTRL;
    } while ((reg & ANADIG_PLL_PLL_1G_CTRL_PLL_1G_STABLE_MASK)
            != ANADIG_PLL_PLL_1G_CTRL_PLL_1G_STABLE_MASK); /* wait for PLL locked */
}

/* 1GPLL */
void CLOCK_InitSysPll1(const clock_sys_pll1_config_t *config)
{
    uint32_t div, numerator, denominator;

    /* bypass pll */
    ANATOP_PllBypass(kAI_Itf_1g, true);

    /* sw enable clock */
    ANATOP_SysPll1SwEnClk(true);

    denominator = 0x0FFFFFFF;
    switch (g_xtalFreq / 100000)
    {
        case 240:
            div = 41;
            numerator = 178956970;
            break;
        case 192:
            div = 52;
            numerator = 22369621;
            break;
        default:
            assert(false);
            return;
    }

    /* configure pll */
    ANATOP_PllConfigure(kAI_Itf_1g, div, numerator, 0, denominator);

    /* toggle hold ring off */
    ANATOP_PllToggleHoldRingOff(kAI_Itf_1g, 225);

    /* wait pll stable */
    ANATOP_SysPll1WaitStable();

    /* enabled clock */
    ANATOP_PllEnableClk(kAI_Itf_1g, true);

    /* ungate clock */
    ANATOP_SysPll1Gate(false);

    ANATOP_SysPll1Div2En(config->pllDiv2En);
    ANATOP_SysPll1Div5En(config->pllDiv5En);

    /* bypass pll */
    ANATOP_PllBypass(kAI_Itf_1g, false);
}

void CLOCK_DeinitSysPll1(void)
{
    ANATOP_SysPll1SwEnClk(false);
    ANATOP_SysPll1Div2En(false);
    ANATOP_SysPll1Div5En(false);
    ANATOP_SysPll1Gate(true);
    ANATOP_PllEnableClk(kAI_Itf_1g, false);
    ANATOP_PllSetPower(kAI_Itf_1g, false);
    ANATOP_PllEnablePllReg(kAI_Itf_1g, false);
}

void CLOCK_EnableOsc24M(void)
{
    if (!(ANADIG_OSC->OSC_24M_CTRL & ANADIG_OSC_OSC_24M_CTRL_osc_en_MASK))
    {
        ANADIG_OSC->OSC_24M_CTRL = ANADIG_OSC_OSC_24M_CTRL_osc_en_MASK
                            | ANADIG_OSC_OSC_24M_CTRL_lp_en_MASK;
        while (ANADIG_OSC_OSC_24M_CTRL_osc_24m_stable_MASK !=
                (ANADIG_OSC->OSC_24M_CTRL & ANADIG_OSC_OSC_24M_CTRL_osc_24m_stable_MASK))
        {
        }
    }
}

uint32_t CLOCK_GetPllFreq(clock_pll_t pll)
{
    uint32_t freq = 0;
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
    uint32_t divSelect, postDiv, mfi, mfn, mfd;
#endif

    if (!g_xtalFreq)
    {
        g_xtalFreq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_24M_OUT);
    }

    switch(pll)
    {
        case kCLOCK_Pll_ArmPll:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
            divSelect = (ANADIG_PLL->PLL_ARM_CTRL & ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT_MASK) >> ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT_SHIFT;
            postDiv = (ANADIG_PLL->PLL_ARM_CTRL & ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL_MASK) >> ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL_SHIFT;
            postDiv = (1 << (postDiv + 1));
            freq = g_xtalFreq / (2 * postDiv);
            freq *= divSelect;
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_ARM_OUT);
#endif
            break;
        case kCLOCK_Pll_SysPll1:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_1G_OUT);
#endif
            break;
        case kCLOCK_Pll_SysPll2:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
            mfi = (ANADIG_PLL->PLL_528_MFI & ANADIG_PLL_PLL_528_MFI_MFI_MASK) >> ANADIG_PLL_PLL_528_MFI_MFI_SHIFT;
            mfn = (ANADIG_PLL->PLL_528_MFN & ANADIG_PLL_PLL_528_MFN_MFN_MASK) >> ANADIG_PLL_PLL_528_MFN_MFN_SHIFT;
            mfd = (ANADIG_PLL->PLL_528_MFD & ANADIG_PLL_PLL_528_MFD_MFD_MASK) >> ANADIG_PLL_PLL_528_MFD_MFD_SHIFT;
            freq = g_xtalFreq * (mfi + mfn/mfd);
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_528_OUT);
#endif
            break;
        case kCLOCK_Pll_SysPll3:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
            freq = 480000000;
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_480_OUT);
#endif
            break;
        case kCLOCK_Pll_AudioPll:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_AUDIO_OUT);
#endif
            break;
        case kCLOCK_Pll_VideoPll:
#ifdef PLLPFD_FREQ_NOT_FROM_OBS
#else
            freq = CLOCK_GetFreqFromObs(CCM_OBS_PLL_VIDEO_OUT);
#endif
            break;
        default:
            assert(false);
            break;
    }
    assert(freq);
    return freq;
}

uint32_t CLOCK_GetFreq(clock_name_t name)
{
    uint32_t freq = 0;
    switch (name)
    {
        case kCLOCK_OscRc16M:
            freq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_RC_16M);
            break;
        case kCLOCK_OscRc48M:
            freq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_RC_48M);
            break;
        case kCLOCK_OscRc48MDiv2:
            freq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_RC_48M_DIV2);
            break;
        case kCLOCK_OscRc400M:
            freq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_RC_400M);
            break;
        case kCLOCK_Osc24MOut:
        case kCLOCK_Osc24M:
            freq = CLOCK_GetFreqFromObs(CCM_OBS_OSC_24M_OUT);
            break;
        case kCLOCK_ArmPllOut:
        case kCLOCK_ArmPll:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_ArmPll);
            break;
        case kCLOCK_SysPll2:
        case kCLOCK_SysPll2Out:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll2);
            break;
        case kCLOCK_SysPll2Pfd0:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd0);
            break;
        case kCLOCK_SysPll2Pfd1:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd1);
            break;
        case kCLOCK_SysPll2Pfd2:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd2);
            break;
        case kCLOCK_SysPll2Pfd3:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd3);
            break;
        case kCLOCK_SysPll3Out:    
        case kCLOCK_SysPll3:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll3);
            break;
        case kCLOCK_SysPll3Pfd0:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd0);
            break;
        case kCLOCK_SysPll3Pfd1:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd1);
            break;
        case kCLOCK_SysPll3Pfd2:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd2);
            break;
        case kCLOCK_SysPll3Pfd3:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd3);
            break;
        case kCLOCK_SysPll1:
        case kCLOCK_SysPll1Out:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1);
            break;
        case kCLOCK_SysPll1Div2:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1) / 2;
            break;
        case kCLOCK_SysPll1Div5:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1) / 5;
            break;
        case kCLOCK_AudioPll:
        case kCLOCK_AudioPllOut:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_AudioPll);
            break;
        case kCLOCK_VideoPll:
        case kCLOCK_VideoPllOut:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_VideoPll);
            assert(false);
            break;
        default:
            assert(false);
            break;
    }
    assert(freq);
    return freq;
}

void CLOCK_SetGroupConfig(clock_group_t group, const clock_group_config_t *config)
{
    assert(group < kCLOCK_Group_Last);

    CCM->CLOCK_GROUP[group].CONTROL = ((config->clockOff ? CCM_CLOCK_GROUP_CONTROL_OFF_MASK: 0U)
                                           |  (config->resetDiv << CCM_CLOCK_GROUP_CONTROL_RSTDIV_SHIFT)
                                           |  (config->div0 << CCM_CLOCK_GROUP_CONTROL_DIV0_SHIFT)
                                           |  (config->div1 << CCM_CLOCK_GROUP_CONTROL_DIV1_SHIFT)
                                           |  (config->div2 << CCM_CLOCK_GROUP_CONTROL_DIV2_SHIFT)
                                           |  (config->div3 << CCM_CLOCK_GROUP_CONTROL_DIV3_SHIFT));

    CCM->CLOCK_GROUP[group].CONTROL_EXTEND = ((config->div4  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV4_SHIFT)
                                           |  (config->div5  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV5_SHIFT)
                                           |  (config->div6  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV6_SHIFT)
                                           |  (config->div7  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV7_SHIFT)
                                           |  (config->div8  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV8_SHIFT)
                                           |  (config->div9  << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV9_SHIFT)
                                           |  (config->div10 << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV10_SHIFT)
                                           |  (config->div11 << CCM_CLOCK_GROUP_CONTROL_EXTEND_DIV11_SHIFT));
}

void CLOCK_TrimOscRc400M(void)
{
    uint32_t trimRegValue = ANADIG_OSC->OSC_OTP_TRIM_VALUE_200M;
    uint32_t trimEnable = (trimRegValue & (1u << 9)) ? 1 : 0;
    uint32_t trimBypass = (trimRegValue & (1u << 8)) ? 1 : 0;
    uint32_t trimValue = trimRegValue & 0xFF;

    if (trimEnable)
    {
        ANADIG_MISC->VDDLPSR_AI400M_CTRL = 0x20;
        ANADIG_MISC->VDDLPSR_AI400M_WDATA = (trimBypass << 10) | (trimValue << 24);
        ANADIG_MISC->VDDLPSR_AI400M_CTRL |= 0x100;
        SDK_DelayAtLeastUs(1);
        ANADIG_MISC->VDDLPSR_AI400M_CTRL &= ~0x100;
    }
}

void CLOCK_EnableOscRc400M(void)
{
    ANADIG_OSC->OSC_200M_CTRL1 &= ~0x1;
    ANADIG_OSC->OSC_200M_CTRL2 |= 0x1;
}

uint32_t CLOCK_GetFreqFromObs(uint32_t obsSigIndex, uint32_t obsIndex)
{
    CCM_OBS->OBSERVE[obsIndex].CONTROL  = CCM_OBS_OBSERVE_CONTROL_OFF_MASK; /* turn off detect */
    CCM_OBS->OBSERVE[obsIndex].CONTROL_SET  = CCM_OBS_OBSERVE_CONTROL_RESET_MASK; /* reset slice */
    CCM_OBS->OBSERVE[obsIndex].CONTROL_CLR  = CCM_OBS_OBSERVE_CONTROL_RAW_MASK; /* select raw obsSigIndex */
    CCM_OBS->OBSERVE[obsIndex].CONTROL &= ~CCM_OBS_OBSERVE_CONTROL_SELECT_MASK; /* Select observed obsSigIndex */
    CCM_OBS->OBSERVE[obsIndex].CONTROL |= CCM_OBS_OBSERVE_CONTROL_SELECT(obsSigIndex)
                                    | CCM_OBS_OBSERVE_CONTROL_DIVIDE(CCM_OBS_DIV); /* turn on detection */
    CCM_OBS->OBSERVE[obsIndex].CONTROL_CLR = CCM_OBS_OBSERVE_CONTROL_RESET_MASK
                                    | CCM_OBS_OBSERVE_CONTROL_OFF_MASK; /* unreset and turn on detect */

    while(CCM_OBS->OBSERVE[obsIndex].FREQUENCY_CURRENT == 0) {}

    return (CCM_OBS->OBSERVE[obsIndex].FREQUENCY_CURRENT * (CCM_OBS_DIV + 1));
}

void CLOCK_ANATOP_TrimLdoLpsrDig(uint32_t target_voltage)
{
    uint8_t trim_value;
    uint32_t temp;

    trim_value = (target_voltage - 628)/20;
    temp  = ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG;
    temp &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_trim_MASK;
    temp |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_trim(trim_value);
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG = temp;
}

void CLOCK_ANATOP_LdoLpsrAnaBypassOn()
{
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_lp_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_track_mode_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* set BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_bypass_mode_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_disable_MASK;
}

void CLOCK_ANATOP_LdoLpsrAnaBypassOff()
{
    /* Enable LDO and HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~(ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_disable_MASK |
            ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_lp_en_MASK);
    SDK_DelayAtLeastUs(1000);
    /* Clear BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_bypass_mode_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_track_mode_en_MASK;
}

void CLOCK_ANATOP_LdoLpsrDigBypassOn()
{
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_hp_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_tracking_mode_MASK;
    SDK_DelayAtLeastUs(1000);
    /* set BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_bypass_mode_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_en_MASK;
}

void CLOCK_ANATOP_LdoLpsrDigBypassOff()
{
    /* Enable LDO and HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= (ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_en_MASK |
            ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_hp_en_MASK);
    SDK_DelayAtLeastUs(1000);
    /* Clear BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_bypass_mode_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_tracking_mode_MASK;
}

void CLOCK_ANATOP_BothLdoLpsrBypassOn()
{
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_lp_en_MASK;
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_hp_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_track_mode_en_MASK;
    /* tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_tracking_mode_MASK;
    SDK_DelayAtLeastUs(1000);
    /* set BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_bypass_mode_en_MASK;
    /* set BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_bypass_mode_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_disable_MASK;
    /* Disable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_en_MASK;
}

void CLOCK_ANATOP_BothLdoLpsrBypassOff()
{
    /* Enable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_disable_MASK;
    /* Enable LDO */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_en_MASK;
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_reg_lp_en_MASK;
    /* HP mode */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG |= ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_reg_hp_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Clear BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_bypass_mode_MASK;
    /* Clear BYPASS */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_bypass_mode_en_MASK;
    SDK_DelayAtLeastUs(1000);
    /* Disable tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_DIG &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_DIG_tracking_mode_MASK;
    /* Disable tracking */
    ANADIG_LDO_SNVS->PMU_LDO_LPSR_ANA &= ~ANADIG_LDO_SNVS_PMU_LDO_LPSR_ANA_track_mode_en_MASK;
}

/*!
 * brief Use DWT to delay at least for some time.
 * Please note that, this API will calculate the microsecond period with the maximum devices
 * supported CPU frequency, so this API will only delay for at least the given microseconds, if precise
 * delay count was needed, please implement a new timer count to achieve this function.
 *
 * param delay_us  Delay time in unit of microsecond.
 */
__attribute__((weak)) void SDK_DelayAtLeastUs(uint32_t delay_us)
{
    assert(0U != delay_us);
    uint64_t count  = 0U;
    uint32_t period = SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY / 1000000;

    /* Make sure the DWT trace fucntion is enabled. */
    if (CoreDebug_DEMCR_TRCENA_Msk != (CoreDebug_DEMCR_TRCENA_Msk & CoreDebug->DEMCR))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }

    /* CYCCNT not supported on this device. */
    assert(DWT_CTRL_NOCYCCNT_Msk != (DWT->CTRL & DWT_CTRL_NOCYCCNT_Msk));

    /* If CYCCENT has already been enabled, read directly, otherwise, need enable it. */
    if (DWT_CTRL_CYCCNTENA_Msk != (DWT_CTRL_CYCCNTENA_Msk & DWT->CTRL))
    {
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    /* Calculate the count ticks. */
    count = DWT->CYCCNT;
    count += (uint64_t)period * delay_us;

    if (count > 0xFFFFFFFFUL)
    {
        count -= 0xFFFFFFFFUL;
        /* wait for cyccnt overflow. */
        while (count < DWT->CYCCNT)
        {
        }
    }

    /* Wait for cyccnt reach count value. */
    while (count > DWT->CYCCNT)
    {
    }
}
