/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
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
volatile uint32_t g_xtalFreq = 24000000U;
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

/* 1GPLL */
void CLOCK_InitSysPll1(const clock_sys_pll_config_t *config)
{
}

void CLOCK_DeinitSysPll1(void)
{
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

void CLOCK_InitPfd(clock_pll_t pll, clock_pfd_t pfd, uint8_t frac)
{
    volatile uint32_t *pfdCtrl, reg;

    switch (pll)
    {
        case kCLOCK_Pll_SysPll2:
            pfdCtrl = &ANADIG_PLL->PLL_528_PFD;
            break;
        case kCLOCK_Pll_SysPll3:
            pfdCtrl = &ANADIG_PLL->PLL_480_PFD;
            break;
        default:
            assert(false);
            break;
    }

    /* all pfds support to be updated on-the-fly after corresponding PLL is stable */
    *pfdCtrl &= ~(ANADIG_PLL_PLL_528_PFD_PFD0_FRAC_MASK << (8 * (uint32_t) pfd));
    *pfdCtrl |= (ANADIG_PLL_PLL_528_PFD_PFD0_FRAC(frac) << (8 * (uint32_t) pfd));
    reg = *pfdCtrl;
    /* Wait for stablizing */
    while (!(reg & (ANADIG_PLL_PLL_528_PFD_PFD0_STABLE_MASK << (8 * (uint32_t) pfd))))
    {
    }
    *pfdCtrl |= (ANADIG_PLL_PLL_528_PFD_PFD0_DIV1_CLKGATE_MASK << (8 * (uint32_t) pfd));
}

#define PFD_FRAC_MIN 12
#define PFD_FRAC_MAX 35 
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
    return pllFreq * 18 / frac;
}

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

void CLOCK_InitAudioPll(const clock_audio_pll_config_t *config)
{
}

void CLOCK_DeinitAudioPll(void)
{
}

void CLOCK_InitVideoPll(const clock_video_pll_config_t *config)
{
}

void CLOCK_DeinitVideoPll(void)
{
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
    uint32_t divSelect, postDiv, mfi, mfn, mfd;

    switch(pll)
    {
        case kCLOCK_Pll_ArmPll:
            divSelect = (ANADIG_PLL->PLL_ARM_CTRL & ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT_MASK) >> ANADIG_PLL_PLL_ARM_CTRL_DIV_SELECT_SHIFT;
            postDiv = (ANADIG_PLL->PLL_ARM_CTRL & ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL_MASK) >> ANADIG_PLL_PLL_ARM_CTRL_POST_DIV_SEL_SHIFT;
            postDiv = (1 << (postDiv + 1));
            freq = g_xtalFreq / (2 * postDiv);
            freq *= divSelect;
            break;
        case kCLOCK_Pll_SysPll1:
            break;
        case kCLOCK_Pll_SysPll2:
            mfi = (ANADIG_PLL->PLL_528_MFI & ANADIG_PLL_PLL_528_MFI_MFI_MASK) >> ANADIG_PLL_PLL_528_MFI_MFI_SHIFT;
            mfn = (ANADIG_PLL->PLL_528_MFN & ANADIG_PLL_PLL_528_MFN_MFN_MASK) >> ANADIG_PLL_PLL_528_MFN_MFN_SHIFT;
            mfd = (ANADIG_PLL->PLL_528_MFD & ANADIG_PLL_PLL_528_MFD_MFD_MASK) >> ANADIG_PLL_PLL_528_MFD_MFD_SHIFT;
            freq = g_xtalFreq * (mfi + mfn/mfd); 
            break;
        case kCLOCK_Pll_SysPll3:
            divSelect = (ANADIG_PLL->PLL_480_CTRL & ANADIG_PLL_PLL_480_CTRL_DIV_SELECT_MASK) >> ANADIG_PLL_PLL_480_CTRL_DIV_SELECT_SHIFT;
            freq = g_xtalFreq * divSelect;
            break;
        case kCLOCK_Pll_AudioPll:
            break;
        case kCLOCK_Pll_VideoPll:
            break;
        default:
            assert(false);
            break;
    }
    assert(freq);
    return freq;
}

uint32_t CLOCK_GetSourceFreq(clock_source_t name)
{
    uint32_t freq = 0;
    switch (name)
    {
        case kCLOCK_Source_OscRc16M:
        case kCLOCK_Source_OscRc48M:
        case kCLOCK_Source_OscRc48MDiv2:
            freq = 24000000;
            break;
        case kCLOCK_Source_OscRc400M:
        case kCLOCK_Source_Osc24M:
            freq = 24000000;
            break;
        case kCLOCK_Source_Osc24MOut:
            freq = 24000000;
            break;
        case kCLOCK_Source_ArmPllOut:
        case kCLOCK_Source_ArmPll:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_ArmPll);
            break;
        case kCLOCK_Source_SysPll2:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll2);
            break;
        case kCLOCK_Source_SysPll2Out:
            break;
        case kCLOCK_Source_SysPll2Pfd0:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd0);
            break;
        case kCLOCK_Source_SysPll2Pfd1:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd1);
            break;
        case kCLOCK_Source_SysPll2Pfd2:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd2);
            break;
        case kCLOCK_Source_SysPll2Pfd3:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll2, kCLOCK_Pfd3);
            break;
        case kCLOCK_Source_SysPll3:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll3);
            break;
        case kCLOCK_Source_SysPll3Out:
            break;
        case kCLOCK_Source_SysPll3Pfd0:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd0);
            break;
        case kCLOCK_Source_SysPll3Pfd1:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd1);
            break;
        case kCLOCK_Source_SysPll3Pfd2:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd2);
            break;
        case kCLOCK_Source_SysPll3Pfd3:
            freq = CLOCK_GetPfdFreq(kCLOCK_Pll_SysPll3, kCLOCK_Pfd3);
            break;
        case kCLOCK_Source_SysPll1:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1);
            break;
        case kCLOCK_Source_SysPll1Out:
            break;
        case kCLOCK_Source_SysPll1Div2:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1) / 2;
            break;
        case kCLOCK_Source_SysPll1Div5:
            freq = CLOCK_GetPllFreq(kCLOCK_Pll_SysPll1) / 5;
            break;
        case kCLOCK_Source_AudioPll:
        case kCLOCK_Source_AudioPllOut:
        case kCLOCK_Source_VideoPll:
        case kCLOCK_Source_VideoPllOut:
            break;
        default:
            assert(false);
            break;
    }
    assert(freq);
    return freq;
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

    return CCM_OBS->OBSERVE[obsIndex].FREQUENCY_CURRENT;
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
