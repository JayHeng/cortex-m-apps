/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016 - 2018, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "fsl_common.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* External XTAL (OSC) clock frequency. */
volatile uint32_t g_xtalFreq = 0U;
/* External MCLK in (mclk_in) clock frequency. If not used,
   set this to 0. Otherwise, set it to the exact rate in Hz this pin is
   being driven at.*/
volatile uint32_t g_MclkInFreq = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Clock Selection for IP */
/**
 * brief   Configure the clock selection muxes.
 * param   connection  : Clock to be configured.
 * return  Nothing
 */
void CLOCK_AttachClk(clock_attach_id_t connection)
{
    bool final_descriptor = false;
    uint32_t i;
    volatile uint32_t *pClkSel;

    for (i = 0U; (i < 2U) && (!final_descriptor); i++)
    {
        connection = (clock_attach_id_t)(connection >> (i * 16U)); /*!<  pick up next descriptor */

        if ((connection & 0x80000000U) | ((connection & 0x8000U)))
        {
            pClkSel = CLKCTL_TUPLE_REG(CLKCTL1, connection);
        }
        else
        {
            pClkSel = CLKCTL_TUPLE_REG(CLKCTL0, connection);
        }

        if (connection & 0xfffU)
        {
            *pClkSel = CLKCTL_TUPLE_SEL(connection);
        }
        else
        {
            final_descriptor = true;
        }
    }
}
/* Set IP Clock divider */
/**
 * brief   Setup peripheral clock dividers.
 * param   div_name    : Clock divider name
 * param   divider     : Value to be divided.
 * return  Nothing
 */
void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divider)
{
    volatile uint32_t *pClkDiv;

    if (div_name & 0x80000000U)
    {
        pClkDiv = CLKCTL_TUPLE_REG(CLKCTL1, div_name);
    }
    else
    {
        pClkDiv = CLKCTL_TUPLE_REG(CLKCTL0, div_name);
    }
    /* Reset the divider counter */
    *pClkDiv |= 1U << 29U;

    if (divider == 0U) /*!<  halt */
    {
        *pClkDiv |= 1U << 30U;
    }
    else
    {
        *pClkDiv = (divider - 1U);
    }

    while ((*pClkDiv) & 0x80000000U)
    {
    }
}

/* Get SYSTEM PLL Clk */
/*! brief  Return Frequency of SYSPLL
 *  return Frequency of SYSPLL
 */
uint32_t CLOCK_GetSysPllFreq(void)
{
    uint32_t freq = 0U;
    uint64_t freqTmp;

    switch ((CLKCTL0->SYSPLL0CLKSEL) & CLKCTL0_SYSPLL0CLKSEL_SEL_MASK)
    {
        case CLKCTL0_SYSPLL0CLKSEL_SEL(0):
            freq = CLOCK_GetSFroFreq();
            break;
        case CLKCTL0_SYSPLL0CLKSEL_SEL(1):
            freq = CLOCK_GetXtalInClkFreq();
            break;
        case CLKCTL0_SYSPLL0CLKSEL_SEL(2):
            freq = CLOCK_GetFFroFreq() / 2U;
            break;
        default:
            break;
    }

    if (((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_BYPASS_MASK) == 0U)
    {
        /* PLL output frequency = Fref * (DIV_SELECT + NUM/DENOM). */
        freqTmp = ((uint64_t)freq * ((uint64_t)(CLKCTL0->SYSPLL0NUM))) / ((uint64_t)(CLKCTL0->SYSPLL0DENOM));
        freq *= ((CLKCTL0->SYSPLL0CTL0) & CLKCTL0_SYSPLL0CTL0_MULT_MASK) >> CLKCTL0_SYSPLL0CTL0_MULT_SHIFT;
        freq += (uint32_t)freqTmp;
    }
    return freq;
}
/* Get SYSTEM PLL PFDn Clk */
/*! brief  Get current output frequency of specific System PLL PFD.
 *  param   pfd    : pfd name to get frequency.
 *  return  Frequency of SYSPLL PFD.
 */
uint32_t CLOCK_GetSysPfdFreq(clock_pfd_t pfd)
{
    uint32_t freq = CLOCK_GetSysPllFreq();

    switch (pfd)
    {
        case kCLOCK_Pfd0:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD0_MASK) >> CLKCTL0_SYSPLL0PFD_PFD0_SHIFT);
            break;

        case kCLOCK_Pfd1:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD1_MASK) >> CLKCTL0_SYSPLL0PFD_PFD1_SHIFT);
            break;

        case kCLOCK_Pfd2:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD2_MASK) >> CLKCTL0_SYSPLL0PFD_PFD2_SHIFT);
            break;

        case kCLOCK_Pfd3:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL0->SYSPLL0PFD & CLKCTL0_SYSPLL0PFD_PFD3_MASK) >> CLKCTL0_SYSPLL0PFD_PFD3_SHIFT);
            break;

        default:
            freq = 0U;
            break;
    }

    return freq;
}
/* Get AUDIO PLL Clk */
/*! brief  Return Frequency of AUDIO PLL
 *  return Frequency of AUDIO PLL
 */
uint32_t CLOCK_GetAudioPllFreq(void)
{
    uint32_t freq = 0U;
    uint64_t freqTmp;

    switch ((CLKCTL1->AUDIOPLL0CLKSEL) & CLKCTL1_AUDIOPLL0CLKSEL_SEL_MASK)
    {
        case CLKCTL1_AUDIOPLL0CLKSEL_SEL(0):
            freq = CLOCK_GetSFroFreq();
            break;
        case CLKCTL1_AUDIOPLL0CLKSEL_SEL(1):
            freq = CLOCK_GetXtalInClkFreq();
            break;
        case CLKCTL1_AUDIOPLL0CLKSEL_SEL(2):
            freq = CLOCK_GetFFroFreq() / 2U;
            break;
        default:
            break;
    }

    if (((CLKCTL1->AUDIOPLL0CTL0) & CLKCTL1_AUDIOPLL0CTL0_BYPASS_MASK) == 0)
    {
        /* PLL output frequency = Fref * (DIV_SELECT + NUM/DENOM). */
        freqTmp = ((uint64_t)freq * ((uint64_t)(CLKCTL1->AUDIOPLL0NUM))) / ((uint64_t)(CLKCTL1->AUDIOPLL0DENOM));
        freq *= ((CLKCTL1->AUDIOPLL0CTL0) & CLKCTL1_AUDIOPLL0CTL0_MULT_MASK) >> CLKCTL1_AUDIOPLL0CTL0_MULT_SHIFT;
        freq += (uint32_t)freqTmp;
    }
    return freq;
}
/* Get AUDIO PLL PFDn Clk */
/*! brief  Get current output frequency of specific Audio PLL PFD.
 *  param   pfd    : pfd name to get frequency.
 *  return  Frequency of AUDIO PLL PFD.
 */
uint32_t CLOCK_GetAudioPfdFreq(clock_pfd_t pfd)
{
    uint32_t freq = CLOCK_GetAudioPllFreq();

    switch (pfd)
    {
        case kCLOCK_Pfd0:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL1->AUDIOPLL0PFD & CLKCTL1_AUDIOPLL0PFD_PFD0_MASK) >> CLKCTL1_AUDIOPLL0PFD_PFD0_SHIFT);
            break;

        case kCLOCK_Pfd1:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL1->AUDIOPLL0PFD & CLKCTL1_AUDIOPLL0PFD_PFD1_MASK) >> CLKCTL1_AUDIOPLL0PFD_PFD1_SHIFT);
            break;

        case kCLOCK_Pfd2:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL1->AUDIOPLL0PFD & CLKCTL1_AUDIOPLL0PFD_PFD2_MASK) >> CLKCTL1_AUDIOPLL0PFD_PFD2_SHIFT);
            break;

        case kCLOCK_Pfd3:
            freq = (uint64_t)freq * 18 /
                   ((CLKCTL1->AUDIOPLL0PFD & CLKCTL1_AUDIOPLL0PFD_PFD3_MASK) >> CLKCTL1_AUDIOPLL0PFD_PFD3_SHIFT);
            break;

        default:
            freq = 0U;
            break;
    }

    return freq;
}
/* Get MAIN Clk */
/*! brief  Return Frequency of main clk
 *  return Frequency of main clk
 */
uint32_t CLOCK_GetMainClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL0->MAINCLKSELB) & CLKCTL0_MAINCLKSELB_SEL_MASK)
    {
        case CLKCTL0_MAINCLKSELB_SEL(0):
            switch ((CLKCTL0->MAINCLKSELA) & CLKCTL0_MAINCLKSELA_SEL_MASK)
            {
                case CLKCTL0_MAINCLKSELA_SEL(0):
                    freq = CLOCK_GetFFroFreq() / 4U;
                    break;
                case CLKCTL0_MAINCLKSELA_SEL(1):
                    freq = CLOCK_GetXtalInClkFreq();
                    break;
                case CLKCTL0_MAINCLKSELA_SEL(2):
                    freq = CLOCK_GetLpOscFreq();
                    break;
                case CLKCTL0_MAINCLKSELA_SEL(3):
                    freq = CLOCK_GetFFroFreq();
                    break;
                default:
                    break;
            }
            break;

        case CLKCTL0_MAINCLKSELB_SEL(1):
            freq = CLOCK_GetSFroFreq();
            break;

        case CLKCTL0_MAINCLKSELB_SEL(2):
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case CLKCTL0_MAINCLKSELB_SEL(3):
            freq = CLOCK_GetOsc32KFreq();
            break;

        default:
            break;
    }

    return freq;
}
/* Get DSP MAIN Clk */
/*! brief  Return Frequency of DSP main clk
 *  return Frequency of DSP main clk
 */
uint32_t CLOCK_GetDspMainClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL1->DSPCPUCLKSELB) & CLKCTL1_DSPCPUCLKSELB_SEL_MASK)
    {
        case 0U:
            if (CLKCTL1->DSPCPUCLKSELA == 0U)
            {
                freq = CLOCK_GetSFroFreq();
            }
            else if (CLKCTL1->DSPCPUCLKSELA == 1U)
            {
                freq = CLOCK_GetXtalInClkFreq();
            }
            else if (CLKCTL1->DSPCPUCLKSELA == 2U)
            {
                freq = CLOCK_GetLpOscFreq();
            }
            else if (CLKCTL1->DSPCPUCLKSELA == 3U)
            {
                freq = CLOCK_GetFFroFreq();
            }
            else
            {
            }

            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 2U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd1);
            break;

        case 3U:
            freq = CLOCK_GetOsc32KFreq();
            break;

        default:
            break;
    }

    return freq;
}
/* Get ADC Clk */
/*! brief  Return Frequency of Adc Clock
 *  return Frequency of Adc Clock.
 */
uint32_t CLOCK_GetAdcClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL0->ADC0FCLKSEL1) & CLKCTL0_ADC0FCLKSEL1_SEL_MASK)
    {
        case 0U:
            if (CLKCTL0->ADC0FCLKSEL0 == 0U)
            {
                freq = CLOCK_GetSFroFreq();
            }
            else if (CLKCTL0->ADC0FCLKSEL0 == 1U)
            {
                freq = CLOCK_GetXtalInClkFreq();
            }
            else if (CLKCTL0->ADC0FCLKSEL0 == 2U)
            {
                freq = CLOCK_GetLpOscFreq();
            }
            else if (CLKCTL0->ADC0FCLKSEL0 == 3U)
            {
                freq = CLOCK_GetFFroFreq();
            }
            else
            {
            }

            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 3U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 5U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        default:
            break;
    }

    return freq / ((CLKCTL0->ADC0FCLKDIV & 0xffU) + 1U);
}
/* Get CLOCK OUT Clk */
/*! brief  Return Frequency of ClockOut
 *  return Frequency of ClockOut
 */
uint32_t CLOCK_GetClockOutClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL1->CLKOUTSEL1) & CLKCTL1_CLKOUTSEL1_SEL_MASK)
    {
        case 0U:
            if (CLKCTL1->CLKOUTSEL0 == 0U)
            {
                freq = CLOCK_GetSFroFreq();
            }
            else if (CLKCTL1->CLKOUTSEL0 == 1U)
            {
                freq = CLOCK_GetXtalInClkFreq();
            }
            else if (CLKCTL1->CLKOUTSEL0 == 2U)
            {
                freq = CLOCK_GetLpOscFreq();
            }
            else if (CLKCTL1->CLKOUTSEL0 == 3U)
            {
                freq = CLOCK_GetFFroFreq();
            }
            else if (CLKCTL1->CLKOUTSEL0 == 4U)
            {
                freq = CLOCK_GetMainClkFreq();
            }
            else if (CLKCTL1->CLKOUTSEL0 == 6U)
            {
                freq = CLOCK_GetDspMainClkFreq();
            }
            else
            {
            }

            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 2U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 3U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd1);
            break;

        case 4U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        case 5U:
            freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
            break;

        case 6U:
            freq = CLOCK_GetOsc32KFreq();
            break;

        default:
            break;
    }

    return freq / ((CLKCTL1->CLKOUTDIV & 0xffU) + 1U);
}
/* Get FRG Clk */
/*! brief  Return Input frequency for the Fractional baud rate generator
 *  return Input Frequency for FRG
 */
uint32_t CLOCK_GetFRGClock(uint32_t id)
{
    uint32_t freq = 0U;
    uint32_t frgPllDiv = 1U;
    uint32_t clkSel = 0U;
    uint32_t frgDiv = 0U;
    uint32_t frgMul = 0U;

    if (id <= 5)
    {
        clkSel = CLKCTL1->FLEXCOMM[id].FRGCLKSEL & CLKCTL1_FLEXCOMM_FRGCLKSEL_SEL_MASK;
        frgMul =
            ((CLKCTL1->FLEXCOMM[id].FRGCTL) & CLKCTL1_FLEXCOMM_FRGCTL_MULT_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_MULT_SHIFT;
        frgDiv =
            ((CLKCTL1->FLEXCOMM[id].FRGCTL) & CLKCTL1_FLEXCOMM_FRGCTL_DIV_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_DIV_SHIFT;
    }
    else if (id == 14)
    {
        clkSel = CLKCTL1->FRG14CLKSEL & CLKCTL1_FRG14CLKSEL_SEL_MASK;
        frgMul = ((CLKCTL1->FRG14CTL) & CLKCTL1_FLEXCOMM_FRGCTL_MULT_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_MULT_SHIFT;
        frgDiv = ((CLKCTL1->FRG14CTL) & CLKCTL1_FLEXCOMM_FRGCTL_DIV_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_DIV_SHIFT;
    }
    else
    {
        clkSel = CLKCTL1->FRG15CLKSEL & CLKCTL1_FRG14CLKSEL_SEL_MASK;
        frgMul = ((CLKCTL1->FRG15CTL) & CLKCTL1_FLEXCOMM_FRGCTL_MULT_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_MULT_SHIFT;
        frgDiv = ((CLKCTL1->FRG15CTL) & CLKCTL1_FLEXCOMM_FRGCTL_DIV_MASK) >> CLKCTL1_FLEXCOMM_FRGCTL_DIV_SHIFT;
    }

    switch (clkSel)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            frgPllDiv = (CLKCTL1->FRGPLLCLKDIV & 0xffU) + 1U;
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0) / frgPllDiv;
            break;

        case 2U:
            freq = CLOCK_GetSFroFreq();
            break;

        case 3U:
            freq = CLOCK_GetFFroFreq();
            break;

        default:
            break;
    }

    return ((uint64_t)freq * (frgDiv + 1)) / (frgMul + frgDiv + 1);
}
/* Get FLEXCOMM Clk */
/*! brief  Return Frequency of Flexcomm functional Clock
 *  param   id    : flexcomm index to get frequency.
 *  return Frequency of Flexcomm functional Clock
 */
uint32_t CLOCK_GetFlexCommClkFreq(uint32_t id)
{
    uint32_t freq = 0U;
    uint32_t clkSel = 0U;

    if (id <= 5)
    {
        clkSel = CLKCTL1->FLEXCOMM[id].FCFCLKSEL;
    }
    else if (id == 14)
    {
        clkSel = CLKCTL1->FC14FCLKSEL;
    }
    else
    {
        clkSel = CLKCTL1->FC15FCLKSEL;
    }

    switch (clkSel)
    {
        case 0U:
            freq = CLOCK_GetSFroFreq();
            break;

        case 1U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 2U:
            freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
            break;

        case 3U:
            freq = CLOCK_GetMclkInClkFreq();
            break;

        case 4U:
            freq = CLOCK_GetFRGClock(id);
            break;

        default:
            break;
    }

    return freq;
}
/* Get CTIMER Clk */
/*! brief  Return Frequency of Ctimer Clock
 *  param   id    : ctimer index to get frequency.
 *  return Frequency of Ctimer Clock
 */
uint32_t CLOCK_GetCtimerClkFreq(uint32_t id)
{
    uint32_t freq = 0U;

    switch ((CLKCTL1->CT32BITFCLKSEL[id]) & CLKCTL1_CT32BITFCLKSEL_SEL_MASK)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetSFroFreq();
            break;

        case 2U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 3U:
            freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
            break;

        case 4U:
            freq = CLOCK_GetMclkInClkFreq();
            break;

        case 5U:
            freq = CLOCK_GetLpOscFreq();
            break;

        default:
            break;
    }

    return freq;
}
/* Get QSPI Clk */
/*! brief  Return Frequency of QSPI Clock
 *  return Frequency of Qspi.
 */
uint32_t CLOCK_GetQspiClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL0->OSPIFFCLKSEL) & CLKCTL0_OSPIFFCLKSEL_SEL_MASK)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 2U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 3U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 4U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        default:
            break;
    }

    return freq / ((CLKCTL0->OSPIFCLKDIV & 0xffU) + 1U);
}

/* Get SCT Clk */
/*! brief  Return Frequency of sct
 *  return Frequency of sct clk
 */
uint32_t CLOCK_GetSctClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL0->SCTFCLKSEL) & CLKCTL0_SCTFCLKSEL_SEL_MASK)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 2U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 3U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 4U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        case 5U:
            freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
            break;

        default:
            break;
    }

    return freq / ((CLKCTL0->SCTFCLKDIV & 0xffU) + 1U);
}
/*! brief  Return Frequency of mclk_in
 *  return Frequency of mclk clk
 *  Note The pin need associated with MCLK function.
 */
uint32_t CLOCK_GetMclkInClkFreq(void)
{
    uint32_t freq = 0U;

    if ((SYSCTL1->MCLKPINDIR & SYSCTL1_MCLKPINDIR_MCLKPINDIR_MASK) == 0U)
    {
        freq = g_MclkInFreq;
    }
    else 
    {
        freq = CLOCK_GetMclkClkFreq();
    }

    return freq;
}
/*! brief  Return Frequency of mclk Out
 *  return Frequency of mclk Out clk
 */
uint32_t CLOCK_GetMclkClkFreq(void)
{
    uint32_t freq = 0U;

    if (CLKCTL1->AUDIOMCLKSEL == 0U)
    {
        freq = CLOCK_GetFFroFreq();
    }
    else if (CLKCTL1->AUDIOMCLKSEL == 1U)
    {
        freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
    }
    else
    {
    }

    return freq / ((CLKCTL1->AUDIOMCLKDIV & 0xffU) + 1U);
}

uint32_t CLOCK_GetWdtOscFreq(void)
{
    if (CLKCTL0->WDT0FCLKSEL == 0U)
    {
        return CLOCK_GetLpOscFreq();
    }

    return CLOCK_GetMainClkFreq();
}

/*! brief  Return Frequency of systick clk
 *  return Frequency of systick clk
 */
uint32_t CLOCK_GetSystickClkFreq(void)
{
    uint32_t freq = 0U;

    switch (CLKCTL0->SYSTICKFCLKSEL)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq() / ((CLKCTL0->SYSTICKFCLKDIV & 0xffU) + 1U);
            break;

        case 1U:
            freq = CLOCK_GetLpOscFreq();
            break;

        case 2U:
            freq = CLOCK_GetOsc32KFreq();
            break;

        case 3U:
            freq = CLOCK_GetSFroFreq();
            break;

        default:
            break;
    }

    return freq;
}

/*! brief  Return Frequency of SDIO clk
 *  param sel clock source select
 *  param divider clock divider
 *  return Frequency of SDIO clk
 */
uint32_t CLOCK_GetSdioClkFreq(uint32_t sel, uint32_t divider)
{
    uint32_t freq = 0U;

    switch (sel)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd0);
            break;

        case 2U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 3U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 4U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        default:
            break;
    }

    return freq / ((divider & 0xffU) + 1U);
}

uint32_t CLOCK_GetI3cClkFreq(uint32_t sel, uint32_t divider)
{
    uint32_t freq = 0U;

    switch (sel)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetFFroFreq();
            break;

        default:
            break;
    }

    return freq / ((divider & 0xffU) + 1U);
}

/*! brief  Return Frequency of USB clk
 *  return Frequency of USB clk
 */
uint32_t CLOCK_GetUsbClkFreq(void)
{
    uint32_t freq = 0U;

    if (CLKCTL0->USBHSFCLKSEL == 0U)
    {
        freq = CLOCK_GetXtalInClkFreq();
    }
    else if (CLKCTL0->USBHSFCLKSEL == 1U)
    {
        freq = CLOCK_GetMainClkFreq();
    }
    else
    {
    }

    return freq / ((CLKCTL0->USBHSFCLKDIV & 0xffU) + 1U);
}

/*! brief  Return Frequency of DMIC clk
 *  return Frequency of DMIC clk
 */
uint32_t CLOCK_GetDmicClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL1->DMIC0FCLKSEL) & CLKCTL1_DMIC0FCLKSEL_SEL_MASK)
    {
        case 0U:
            freq = CLOCK_GetSFroFreq();
            break;

        case 1U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 2U:
            freq = CLOCK_GetAudioPfdFreq(kCLOCK_Pfd0) / ((CLKCTL1->AUDIOPLLCLKDIV & 0xffU) + 1U);
            break;

        case 3U:
            freq = CLOCK_GetMclkInClkFreq();
            break;

        case 4U:
            freq = CLOCK_GetLpOscFreq();
            break;

        case 5U:
            freq = CLOCK_GetWakeClk32KFreq();
            break;

        case 6U:
            freq = CLOCK_GetMainClkFreq();
            break;

        default:
            break;
    }

    return freq / ((CLKCTL1->DMIC0FCLKDIV & 0xffU) + 1U);
}

/*! brief  Return Frequency of ACMP clk
 *  return Frequency of ACMP clk
 */
uint32_t CLOCK_GetAcmpClkFreq(void)
{
    uint32_t freq = 0U;

    switch ((CLKCTL1->ACMP0FCLKSEL) & CLKCTL1_ACMP0FCLKSEL_SEL_MASK)
    {
        case 0U:
            freq = CLOCK_GetMainClkFreq();
            break;

        case 1U:
            freq = CLOCK_GetSFroFreq();
            break;

        case 2U:
            freq = CLOCK_GetFFroFreq();
            break;

        case 3U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd2);
            break;

        case 4U:
            freq = CLOCK_GetSysPfdFreq(kCLOCK_Pfd3);
            break;

        default:
            break;
    }

    return freq / ((CLKCTL1->ACMP0FCLKDIV & 0xffU) + 1U);
}

/* Get IP Clk */
/*! brief  Return Frequency of selected clock
 *  return Frequency of selected clock
 */
uint32_t CLOCK_GetFreq(clock_name_t clockName)
{
    uint32_t freq = 0U;

    switch (clockName)
    {
        case kCLOCK_CoreSysClk:
        case kCLOCK_BusClk:
            freq = CLOCK_GetMainClkFreq() / ((CLKCTL0->SYSCPUAHBCLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_Mclk:
            freq = CLOCK_GetMclkClkFreq();
            break;
        case kCLOCK_ClockOut:
            freq = CLOCK_GetClockOutClkFreq();
            break;
        case kCLOCK_AdcClk:
            freq = CLOCK_GetAdcClkFreq();
            break;
        case kCLOCK_QspiClk:
            freq = CLOCK_GetQspiClkFreq();
            break;
        case kCLOCK_SctClk:
            freq = CLOCK_GetSctClkFreq();
            break;
        case kCLOCK_WdtOsc:
            freq = CLOCK_GetWdtOscFreq();
            break;
        case kCLOCK_SystickClk:
            freq = CLOCK_GetSystickClkFreq();
            break;
        case kCLOCK_Sdio0Clk:
            freq = CLOCK_GetSdioClkFreq(CLKCTL0->SDIO0FCLKSEL, CLKCTL0->SDIO0FCLKDIV);
            break;
        case kCLOCK_I3cClk:
            freq = CLOCK_GetI3cClkFreq(CLKCTL1->I3C0FCLKSEL, CLKCTL1->I3C0FCLKDIV);
            break;
        case kCLOCK_Usb:
            freq = CLOCK_GetUsbClkFreq();
            break;
        case kCLOCK_Dmic:
            freq = CLOCK_GetDmicClkFreq();
            break;
        case kCLOCK_DspCpuClk:
            freq = CLOCK_GetDspMainClkFreq() / ((CLKCTL1->DSPCPUCLKDIV & 0xffU) + 1U);
            break;
        case kCLOCK_AcmpClk:
            freq = CLOCK_GetAcmpClkFreq();
            break;
        case kCLOCK_Flexcomm0:
            freq = CLOCK_GetFlexCommClkFreq(0U);
            break;
        case kCLOCK_Flexcomm1:
            freq = CLOCK_GetFlexCommClkFreq(1U);
            break;
        case kCLOCK_Flexcomm2:
            freq = CLOCK_GetFlexCommClkFreq(2U);
            break;
        case kCLOCK_Flexcomm3:
            freq = CLOCK_GetFlexCommClkFreq(3U);
            break;
        case kCLOCK_Flexcomm4:
            freq = CLOCK_GetFlexCommClkFreq(4U);
            break;
        case kCLOCK_Flexcomm5:
            freq = CLOCK_GetFlexCommClkFreq(5U);
            break;
        case kCLOCK_Flexcomm14:
            freq = CLOCK_GetFlexCommClkFreq(14U);
            break;
        case kCLOCK_Flexcomm15:
            freq = CLOCK_GetFlexCommClkFreq(15U);
            break;
        default:
            break;
    }

    return freq;
}

/* Set FRG Clk */
/*! brief  Set output of the Fractional baud rate generator
 * param   config    : Configuration to set to FRGn clock.
 */
void CLOCK_SetFRGClock(const clock_frg_clk_config_t *config)
{
    uint32_t i = config->num;

    if (i <= 5)
    {
        CLKCTL1->FLEXCOMM[i].FRGCLKSEL = config->sfg_clock_src;
        CLKCTL1->FLEXCOMM[i].FRGCTL =
            (CLKCTL1_FLEXCOMM_FRGCTL_MULT(config->mult) | CLKCTL1_FLEXCOMM_FRGCTL_DIV(config->divider));
    }
    else if (i == 14)
    {
        CLKCTL1->FRG14CLKSEL = config->sfg_clock_src;
        CLKCTL1->FRG14CTL = (CLKCTL1_FLEXCOMM_FRGCTL_MULT(config->mult) | CLKCTL1_FLEXCOMM_FRGCTL_DIV(config->divider));
    }
    else
    {
        CLKCTL1->FRG15CLKSEL = config->sfg_clock_src;
        CLKCTL1->FRG15CTL = (CLKCTL1_FLEXCOMM_FRGCTL_MULT(config->mult) | CLKCTL1_FLEXCOMM_FRGCTL_DIV(config->divider));
    }
}
/* Initialize the SYSTEM PLL Clk */
/*! brief  Initialize the System PLL.
 *  param  config    : Configuration to set to PLL.
 */
void CLOCK_InitSysPll(const clock_sys_pll_config_t *config)
{
    uint32_t i;

    /* Power down SYSPLL before change fractional settings */
    SYSCTL0->PDRUNCFG0_SET = SYSCTL0_PDRUNCFG0_SYSPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_SYSPLLANA_PD_MASK;

    CLKCTL0->SYSPLL0CLKSEL = config->sys_pll_src;
    CLKCTL0->SYSPLL0NUM = config->numerator;
    CLKCTL0->SYSPLL0DENOM = config->denominator;
    switch (config->sys_pll_mult)
    {
        case kCLOCK_SysPllMult16:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(16);
            break;
        case kCLOCK_SysPllMult17:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(17);
            break;
        case kCLOCK_SysPllMult20:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(20);
            break;
        case kCLOCK_SysPllMult22:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(22);
            break;
        case kCLOCK_SysPllMult27:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(27);
            break;
        case kCLOCK_SysPllMult33:
            CLKCTL0->SYSPLL0CTL0 =
                (CLKCTL0->SYSPLL0CTL0 & ~CLKCTL0_SYSPLL0CTL0_MULT_MASK) | CLKCTL0_SYSPLL0CTL0_MULT(33);
            break;
        default:
            break;
    }
    /* Clear System PLL reset*/
    CLKCTL0->SYSPLL0CTL0 &= ~CLKCTL0_SYSPLL0CTL0_RESET_MASK;
    /* Power up SYSPLL*/
    SYSCTL0->PDRUNCFG0_CLR = SYSCTL0_PDRUNCFG0_SYSPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_SYSPLLANA_PD_MASK;

    for (i = 0; i < (25 * (CLKCTL0->SYSPLL0LOCKTIMEDIV2 & CLKCTL0_SYSPLL0LOCKTIMEDIV2_LOCKTIMEDIV2_MASK)); i++)
    {
        __NOP();
    }
}
/* Initialize the System PLL PFD */
/*! brief Initialize the System PLL PFD.
 *  param pfd    : Which PFD clock to enable.
 *  param divider    : The PFD divider value.
 *  note It is recommended that PFD settings are kept between 12-35.
 */
void CLOCK_InitSysPfd(clock_pfd_t pfd, uint8_t divider)
{
    uint32_t pfdIndex = (uint32_t)pfd;
    uint32_t syspfd;

    syspfd = CLKCTL0->SYSPLL0PFD &
             ~((CLKCTL0_SYSPLL0PFD_PFD0_CLKGATE_MASK | CLKCTL0_SYSPLL0PFD_PFD0_MASK) << (8 * pfdIndex));

    /* Disable the clock output first. */
    CLKCTL0->SYSPLL0PFD = syspfd | (CLKCTL0_SYSPLL0PFD_PFD0_CLKGATE_MASK << (8 * pfdIndex));

    /* Set the new value and enable output. */
    CLKCTL0->SYSPLL0PFD = syspfd | (CLKCTL0_SYSPLL0PFD_PFD0(divider) << (8 * pfdIndex));
    /* Wait for output becomes stable. */
    while ((CLKCTL0->SYSPLL0PFD & (CLKCTL0_SYSPLL0PFD_PFD0_CLKRDY_MASK << (8 * pfdIndex))) == 0)
    {
    }
    /* Clear ready status flag. */
    CLKCTL0->SYSPLL0PFD |= (CLKCTL0_SYSPLL0PFD_PFD0_CLKRDY_MASK << (8 * pfdIndex));
}
/* Initialize the Audio PLL Clk */
/*! brief  Initialize the audio PLL.
 *  param  config    : Configuration to set to PLL.
 */
void CLOCK_InitAudioPll(const clock_audio_pll_config_t *config)
{
    uint32_t i;

    /* Power down Audio PLL before change fractional settings */
    SYSCTL0->PDRUNCFG0_SET = SYSCTL0_PDRUNCFG0_AUDPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_AUDPLLANA_PD_MASK;

    CLKCTL1->AUDIOPLL0CLKSEL = config->audio_pll_src;
    CLKCTL1->AUDIOPLL0NUM = config->numerator;
    CLKCTL1->AUDIOPLL0DENOM = config->denominator;
    CLKCTL1->AUDIOPLL0CTL0 =
        (CLKCTL1->AUDIOPLL0CTL0 & ~CLKCTL1_AUDIOPLL0CTL0_MULT_MASK) | CLKCTL1_AUDIOPLL0CTL0_MULT(config->mult);
    /* Clear Audio PLL reset*/
    CLKCTL1->AUDIOPLL0CTL0 &= ~CLKCTL1_AUDIOPLL0CTL0_RESET_MASK;
    /* Power up Audio PLL*/
    SYSCTL0->PDRUNCFG0_CLR = SYSCTL0_PDRUNCFG0_AUDPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_AUDPLLANA_PD_MASK;

    for (i = 0; i < (25 * (CLKCTL1->AUDIOPLL0LOCKTIMEDIV2 & CLKCTL1_AUDIOPLL0LOCKTIMEDIV2_LOCKTIMEDIV2_MASK)); i++)
    {
        __NOP();
    }
}
/* Initialize the Audio PLL PFD */
/*! brief Initialize the audio PLL PFD.
 *  param pfd    : Which PFD clock to enable.
 *  param divider    : The PFD divider value.
 *  note It is recommended that PFD settings are kept between 12-35.
 */
void CLOCK_InitAudioPfd(clock_pfd_t pfd, uint8_t divider)
{
    uint32_t pfdIndex = (uint32_t)pfd;
    uint32_t syspfd;

    syspfd = CLKCTL1->AUDIOPLL0PFD &
             ~((CLKCTL1_AUDIOPLL0PFD_PFD0_CLKGATE_MASK | CLKCTL1_AUDIOPLL0PFD_PFD0_MASK) << (8 * pfdIndex));

    /* Disable the clock output first. */
    CLKCTL1->AUDIOPLL0PFD = syspfd | (CLKCTL1_AUDIOPLL0PFD_PFD0_CLKGATE_MASK << (8 * pfdIndex));

    /* Set the new value and enable output. */
    CLKCTL1->AUDIOPLL0PFD = syspfd | (CLKCTL1_AUDIOPLL0PFD_PFD0(divider) << (8 * pfdIndex));
    /* Wait for output becomes stable. */
    while ((CLKCTL1->AUDIOPLL0PFD & (CLKCTL1_AUDIOPLL0PFD_PFD0_CLKRDY_MASK << (8 * pfdIndex))) == 0)
    {
    }
    /* Clear ready status flag. */
    CLKCTL1->AUDIOPLL0PFD |= (CLKCTL1_AUDIOPLL0PFD_PFD0_CLKRDY_MASK << (8 * pfdIndex));
}
/*! @brief Enable USB HS device PLL clock.
 *
 * This function enables USB HS device PLL clock.
 */
void CLOCK_EnableUsbhsDeviceClock(void)
{
    SYSCTL0->PDRUNCFG0_CLR = SYSCTL0_PDRUNCFG0_SYSXTAL_PD_MASK;
    CLOCK_EnableClock(kCLOCK_UsbhsPhy);
    CLKCTL0->PFCDIV[1] = 0x03;
    CLKCTL0->SYSOSCCTL0 = CLKCTL0_SYSOSCCTL0_LP_ENABLE(1);
    CLKCTL0->SYSOSCBYPASS = CLKCTL0_SYSOSCBYPASS_SEL(0);
    CLKCTL0->USBHSFCLKSEL = 0;
    CLKCTL0->USBHSFCLKDIV = 0;

    /* Enable usbhs device and ram clock */
    CLOCK_EnableClock(kCLOCK_UsbhsDevice);
    CLOCK_EnableClock(kCLOCK_UsbhsSram);

    RESET_PeripheralReset(kUSBHS_PHY_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_DEVICE_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_HOST_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_SRAM_RST_SHIFT_RSTn);
}

/*! @brief Enable USB HS host PLL clock.
 *
 * This function enables USB HS host PLL clock.
 */
void CLOCK_EnableUsbhsHostClock(void)
{
    SYSCTL0->PDRUNCFG0_CLR = SYSCTL0_PDRUNCFG0_SYSXTAL_PD_MASK;
    CLOCK_EnableClock(kCLOCK_UsbhsPhy);
    CLKCTL0->PFCDIV[1] = 0x03;
    CLKCTL0->SYSOSCCTL0 = CLKCTL0_SYSOSCCTL0_LP_ENABLE(1);
    CLKCTL0->SYSOSCBYPASS = CLKCTL0_SYSOSCBYPASS_SEL(0);
    CLKCTL0->USBHSFCLKSEL = 0;
    CLKCTL0->USBHSFCLKDIV = 0;

    /* Enable usbhs device and ram clock */
    CLOCK_EnableClock(kCLOCK_UsbhsHost);
    CLOCK_EnableClock(kCLOCK_UsbhsSram);

    RESET_PeripheralReset(kUSBHS_PHY_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_DEVICE_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_HOST_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kUSBHS_SRAM_RST_SHIFT_RSTn);
}
/*! @brief Enable USB HS PHY PLL clock.
 *
 * This function enables USB HS PHY PLL clock.
 */
void CLOCK_EnableUsbhsPhyClock(void)
{
    USBPHY->CTRL_CLR = USBPHY_CTRL_SFTRST_MASK;

    SYSCTL0->STARTEN1 |= SYSCTL0_STARTEN1_USB0_IRQ(1) | SYSCTL0_STARTEN1_USB0_NEEDCLK(1);
    uint32_t delay = 100000;
    while (delay--)
    {
        __NOP();
    }

    USBPHY->PLL_SIC_SET = (USBPHY_PLL_SIC_PLL_POWER(1) | USBPHY_PLL_SIC_PLL_REG_ENABLE_MASK);
    USBPHY->PLL_SIC = (USBPHY->PLL_SIC & ~(USBPHY_PLL_SIC_PLL_DIV_SEL_MASK)) | USBPHY_PLL_SIC_PLL_DIV_SEL(3);
    USBPHY->PLL_SIC_CLR = USBPHY_PLL_SIC_PLL_BYPASS_MASK;
    USBPHY->PLL_SIC_SET = (USBPHY_PLL_SIC_PLL_EN_USB_CLKS_MASK);

    USBPHY->CTRL_CLR = USBPHY_CTRL_CLR_CLKGATE_MASK;
    USBPHY->PWD_SET = 0x0;

    while (!(USBPHY->PLL_SIC & USBPHY_PLL_SIC_PLL_LOCK_MASK))
    {
    }
}
