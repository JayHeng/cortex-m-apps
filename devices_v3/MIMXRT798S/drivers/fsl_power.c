/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_common.h"
#include "fsl_power.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.power"
#endif
#define MEGA (1000000U)

#if defined(PMC0)
#define PMC      PMC0
#define SLEEPCON SLEEPCON0
#elif defined(PMC1)
#define PMC      PMC1
#define SLEEPCON SLEEPCON1
#else
#error "Unsupported header file"
#endif

#define PMC_PDRCFG_REG(x)         (*((volatile uint32_t *)((uint32_t)(&(PMC->PDRUNCFG0)) + ((x - 1U) << 2U))))
#define SLEEPCON_PDRCFGSET_REG(x) (*((volatile uint32_t *)((uint32_t)(&(SLEEPCON->RUNCFG_SET)) + ((x) << 2U))))
#define SLEEPCON_PDRCFGCLR_REG(x) (*((volatile uint32_t *)((uint32_t)(&(SLEEPCON->RUNCFG_CLR)) + ((x) << 2U))))

#define PMC_REG(off) (*((volatile uint32_t *)(void *)PMC + (off) / 4U))

/* Get AFBB bits mask from body bias domain _body_bias_domain */
#define POWER_AFBB_BITS_MASK(x) (((x)&0x20000000u) << 2U | (((x)&0x05400000u) << 1U))

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief API to enable PDRUNCFG bit in the SLEEPCON and PMC. Note that enabling the bit powers down the peripheral
 *
 * @param en    peripheral for which to enable the PDRUNCFG bit
 * @return none
 */
void POWER_EnablePD(pd_bit_t en)
{
    assert(en != kPDRUNCFG_ForceUnsigned);

    if (GET_PD_REG_FROM_BITS(en) != 0U) /* PMC PDRUNCFG */
    {
        PMC_PDRCFG_REG(GET_PD_REG_FROM_BITS(en)) |= (1UL << (((uint32_t)en) & 0xFFU));
    }
    else /* SLEEPCON_RUNCFG */
    {
        SLEEPCON_PDRCFGSET_REG(GET_PD_REG_FROM_BITS(en)) = (1UL << (((uint32_t)en) & 0xFFU));
    }
}

/*!
 * @brief API to disable PDRUNCFG bit in the Sysctl0. Note that disabling the bit powers up the peripheral
 *
 * @param en    peripheral for which to disable the PDRUNCFG bit
 * @return none
 */
void POWER_DisablePD(pd_bit_t en)
{
    assert(en != kPDRUNCFG_ForceUnsigned);

    if (GET_PD_REG_FROM_BITS(en) != 0U) /* PMC PDRUNCFG */
    {
        PMC_PDRCFG_REG(GET_PD_REG_FROM_BITS(en)) &= ~(1UL << (((uint32_t)en) & 0xFFU));
    }
    else /* SLEEPCON_RUNCFG */
    {
        SLEEPCON_PDRCFGCLR_REG(GET_PD_REG_FROM_BITS(en)) = (1UL << (((uint32_t)en) & 0xFFU));
    }
}

/**
 * @brief  API to apply updated PMC PDRUNCFG bits in the Sysctl0.
 */
void POWER_ApplyPD(void)
{
    /* Cannot set APPLYCFG when BUSY is 1 */
    while ((PMC->STATUS & PMC_STATUS_BUSY_MASK) != 0U)
    {
    }
    PMC->CTRL |= PMC_CTRL_APPLYCFG_MASK;
    /* Wait all PMC finite state machines finished. */
    while ((PMC->STATUS & PMC_STATUS_BUSY_MASK) != 0U)
    {
    }
}

void POWER_SetPmicMode(uint32_t mode, pmic_mode_reg_t reg)
{
    __disable_irq();

    PMC_REG((uint32_t)reg) = (PMC_REG(reg) & ~PMC_PDSLEEPCFG0_PMICMODE_MASK) | (mode << PMC_PDSLEEPCFG0_PMICMODE_SHIFT);

    __enable_irq();
}

uint32_t POWER_GetPmicMode(pmic_mode_reg_t reg)
{
    uint32_t mode = (uint32_t)reg;

    return ((PMC_REG(mode) & PMC_PDSLEEPCFG0_PMICMODE_MASK) >> PMC_PDSLEEPCFG0_PMICMODE_SHIFT);
}

/**
 * @brief   Clears the PMC event flags state.
 * @param   statusMask : A bitmask of event flags that are to be cleared.
 */
void POWER_ClearEventFlags(uint32_t statusMask)
{
    PMC->FLAGS = statusMask;
}

/**
 * @brief   Get the PMC event flags state.
 * @return  PMC FLAGS register value
 */
uint32_t POWER_GetEventFlags(void)
{
    return PMC->FLAGS;
}

/**
 * @brief   Enable the PMC reset requests.
 * @param   resetMask : A bitmask of of resets to enable.
 */
void POWER_EnableResets(uint32_t resetMask)
{
    PMC->CTRL |= resetMask;
}

/**
 * @brief   Disable the PMC reset requests.
 * @param   resetMask : A bitmask of of resets to disable.
 */
void POWER_DisableResets(uint32_t resetMask)
{
    PMC->CTRL &= ~resetMask;
}

/**
 * @brief   Enable the PMC interrupt requests.
 * @param   interruptMask : A bitmask of of interrupts to enable.
 */
void POWER_EnableInterrupts(uint32_t interruptMask)
{
    PMC->INTRCTRL |= interruptMask;
}

void POWER_EnableDMAHWWake(uint32_t sources)
{
    SLEEPCON->HW_WAKE_SET = sources;
}

void POWER_DisableDMAHWWake(uint32_t sources)
{
    SLEEPCON->HW_WAKE_CLR = sources;
}

/**
 * @brief   Disable the PMC interrupt requests.
 * @param   interruptMask : A bitmask of of interrupts to disable.
 */
void POWER_DisableInterrupts(uint32_t interruptMask)
{
    PMC->INTRCTRL &= ~interruptMask;
}

void EnableDeepSleepIRQ(IRQn_Type interrupt)
{
    uint32_t intNumber = (uint32_t)interrupt;

#if defined(PMC0)
    if (intNumber >= 128U)
    {
        /* enable interrupt wake up in the WAKEUPEN4 register */
        SLEEPCON->WAKEUPEN4_SET = 1UL << (intNumber - 128U);
    }
    else if (intNumber >= 96U)
    {
        SLEEPCON->WAKEUPEN3_SET = 1UL << (intNumber - 96U);
    }
    else if (intNumber >= 64U)
#else  /* For sense domain */
    if (intNumber >= 64U)
#endif /* PMC0 */
    {
        SLEEPCON->WAKEUPEN2_SET = 1UL << (intNumber - 64U);
    }
    else if (intNumber >= 32U)
    {
        SLEEPCON->WAKEUPEN1_SET = 1UL << (intNumber - 32U);
    }
    else
    {
        SLEEPCON->WAKEUPEN0_SET = 1UL << intNumber;
    }
    /* also enable interrupt at NVIC */
    (void)EnableIRQ(interrupt);
}

void DisableDeepSleepIRQ(IRQn_Type interrupt)
{
    uint32_t intNumber = (uint32_t)interrupt;

    /* also disable interrupt at NVIC */
    (void)DisableIRQ(interrupt);

#if defined(PMC0)
    if (intNumber >= 128U)
    {
        /* enable interrupt wake up in the WAKEUPEN4 register */
        SLEEPCON->WAKEUPEN4_CLR = 1UL << (intNumber - 128U);
    }
    else if (intNumber >= 96U)
    {
        SLEEPCON->WAKEUPEN3_CLR = 1UL << (intNumber - 96U);
    }
    else if (intNumber >= 64U)
#else
    if (intNumber >= 64U)
#endif /* PMC0 */
    {
        SLEEPCON->WAKEUPEN2_CLR = 1UL << (intNumber - 64U);
    }
    else if (intNumber >= 32U)
    {
        SLEEPCON->WAKEUPEN1_CLR = 1UL << (intNumber - 32U);
    }
    else
    {
        SLEEPCON->WAKEUPEN0_CLR = 1UL << intNumber;
    }
}

void POWER_EnableLPRequestMask(uint32_t mask)
{
    SLEEPCON->SHARED_MASK0_SET |= mask;
}

void POWER_DisableLPRequestMask(uint32_t mask)
{
    SLEEPCON->SHARED_MASK0_CLR |= mask;
}

void POWER_EnterSleep(void)
{
    uint32_t pmsk;
    pmsk = __get_PRIMASK();
    __disable_irq();
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
    __set_PRIMASK(pmsk);
}

void POWER_EnableAutoWake(uint16_t ticks)
{
    PMC->WAKEUP = (PMC->WAKEUP & 0x3F000000U) | PMC_WAKEUP_WAKETIME(ticks); /* Don't clear the [WKSRCF] W1C bits! */
    PMC->INTRCTRL |= PMC_INTRCTRL_AUTOWKIE_MASK;
    EnableDeepSleepIRQ(PMC_IRQn);
}

void POWER_SetRunBodyBiasMode(uint32_t mask)
{
    PMC->PDRUNCFG0 |= (PMC->PDRUNCFG0 & ~mask) | mask;
}

void POWER_EnableRunAFBB(uint32_t mask)
{
    /* clear AFBBxxx_PD, set RBBxxx_PD. No AFBBSRAM1 bit. */
    PMC->PDRUNCFG0 &= ~POWER_AFBB_BITS_MASK(mask);
    PMC->PDRUNCFG0 |= mask;
}

void POWER_EnableRunRBB(uint32_t mask)
{
    PMC->PDRUNCFG0 &= ~mask; /* Clear RBB* bits, set AFBB* bits */
    PMC->PDRUNCFG0 |= POWER_AFBB_BITS_MASK(mask);
}

void POWER_EnableSleepRBB(uint32_t mask)
{
    /* Set AFBB bits, clear RBB bits. */
    PMC->PDSLEEPCFG0 &= ~mask;
    PMC->PDSLEEPCFG0 |= POWER_AFBB_BITS_MASK(mask);
}

void POWER_EnableSleepNBB(uint32_t mask)
{
    /* Set AFBB bits and RBB bits. */
    PMC->PDSLEEPCFG0 |= mask | POWER_AFBB_BITS_MASK(mask);
}

status_t POWER_ConfigRegulatorSetpoints(power_regulator_t regulator,
                                        const power_regulator_voltage_t *volt,
                                        const power_lvd_voltage_t *lvd)
{
    status_t ret = kStatus_Success;

#if defined(PMC0)
    if (regulator == kRegulator_DCDC) /* DCDC with 2 setpoints. */
    {
        if ((volt->DCDC.vsel1 < volt->DCDC.vsel0) || (lvd->VDDN.lvl1 < lvd->VDDN.lvl0) ||
            ((volt->value & 0xFF00) != 0U) || ((lvd->value & 0xFF00) != 0U))
        {
            ret = kStatus_InvalidArgument;
        }
        else
        {
            PMC->DCDCVSEL    = volt->value & (PMC_DCDCVSEL_VSEL0_MASK | PMC_DCDCVSEL_VSEL1_MASK);
            PMC->LVDVDDNCTRL = lvd->value & (PMC_LVDVDDNCTRL_LVL0_MASK | PMC_LVDVDDNCTRL_LVL1_MASK);
        }
    }
#else
    if (regulator != kRegulator_Vdd1LDO) /* Only VDD1 LDO configurable from Sense side. */
    {
        ret = kStatus_InvalidArgument;
    }
#endif   /* PMC0, only compute domain support DCDC, LDO VDD2 control. */
    else /* LDO with 4 setpoints. */
    {
        if ((volt->LDO.vsel3 < volt->LDO.vsel2) || (volt->LDO.vsel2 < volt->LDO.vsel1) ||
            (volt->LDO.vsel1 < volt->LDO.vsel0) || (lvd->VDD12.lvl3 < lvd->VDD12.lvl2) ||
            (lvd->VDD12.lvl2 < lvd->VDD12.lvl1) || (lvd->VDD12.lvl1 < lvd->VDD12.lvl0))
        {
            ret = kStatus_InvalidArgument;
        }
        else
        {
            PMC_REG(0x24U + 4U * ((uint32_t)regulator - 1U)) = volt->value; /* LDOVDDxVSEL */
            PMC_REG(0x30U + 4U * ((uint32_t)regulator - 1U)) = lvd->value;  /* LVDVDDxCTRL */
        }
    }

    return ret;
}

void POWER_SelectRunSetpoint(power_regulator_t regulator, uint32_t setpoint)
{
    if (regulator == kRegulator_DCDC)
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_DCDC_VSEL_MASK) | PMC_PDRUNCFG0_DCDC_VSEL(setpoint);
    }
    else if (regulator == kRegulator_Vdd2LDO)
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_LDO2_VSEL_MASK) | PMC_PDRUNCFG0_LDO2_VSEL(setpoint);
    }
    else
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_LDO1_VSEL_MASK) | PMC_PDRUNCFG0_LDO1_VSEL(setpoint);
    }
}

void POWER_SelectSleepSetpoint(power_regulator_t regulator, uint32_t setpoint)
{
    if (regulator == kRegulator_DCDC)
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_DCDC_VSEL_MASK) | PMC_PDSLEEPCFG0_DCDC_VSEL(setpoint);
    }
    else if (regulator == kRegulator_Vdd2LDO)
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_LDO2_VSEL_MASK) | PMC_PDSLEEPCFG0_LDO2_VSEL(setpoint);
    }
    else
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_LDO1_VSEL_MASK) | PMC_PDSLEEPCFG0_LDO1_VSEL(setpoint);
    }
}

void POWER_SetRunRegulatorMode(power_regulator_t regulator, uint32_t mode)
{
    if (regulator == kRegulator_DCDC)
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_DCDC_LP_MASK) | PMC_PDRUNCFG0_DCDC_LP(mode);
    }
    else if (regulator == kRegulator_Vdd2LDO)
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_LDO2_MODE_MASK) | PMC_PDRUNCFG0_LDO2_MODE(mode);
    }
    else
    {
        PMC->PDRUNCFG0 &= (~PMC_PDRUNCFG0_LDO1_MODE_MASK) | PMC_PDRUNCFG0_LDO1_MODE(mode);
    }
}

void POWER_SetSleepRegulatorMode(power_regulator_t regulator, uint32_t mode)
{
    if (regulator == kRegulator_DCDC)
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_DCDC_LP_MASK) | PMC_PDSLEEPCFG0_DCDC_LP(mode);
    }
    else if (regulator == kRegulator_Vdd2LDO)
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_LDO2_MODE_MASK) | PMC_PDSLEEPCFG0_LDO2_MODE(mode);
    }
    else
    {
        PMC->PDSLEEPCFG0 &= (~PMC_PDSLEEPCFG0_LDO1_MODE_MASK) | PMC_PDSLEEPCFG0_LDO1_MODE(mode);
    }
}

#if defined(PMC0)
void POWER_ConfigRBBVolt(const power_rbb_voltage_t *config)
{
    PMC->BBCTRL = (*((const uint32_t *)(const void *)config)) &
                  (PMC_BBCTRL_RBB1LVL_MASK | PMC_BBCTRL_RBB2LVL_MASK | PMC_BBCTRL_RBBNLVL_MASK);
}

void POWER_DisableRegulators(uint32_t mask)
{
    PMC->POWERCFG |= mask & 0xFF80U; /* Ignore FDSR related bits. */
}

void POWER_EnableSleepRegulators(uint32_t mask)
{
    PMC->POWERCFG |= mask & 0x7FU; /* Ignore all mode control bits. */
}

void POWER_DisableSleepRegulators(uint32_t mask)
{
    PMC->POWERCFG &= ~(mask & 0x7FU); /* Ignore all mode control bits. */
}

void POWER_SetPMICModeDelay(uint8_t value)
{
    PMC->POWERCFG &= ~PMC_POWERCFG_MODEDLY_MASK;
    PMC->POWERCFG |= PMC_POWERCFG_MODEDLY(value);
}

void POWER_SetPORVoltage(const power_por_voltage_t *porVolt)
{
    PMC->PORCTRL = (*((const uint32_t *)(const void *)porVolt));
}
#endif

/* Get power lib version */
uint32_t POWER_GetLibVersion(void)
{
    return (uint32_t)FSL_POWER_DRIVER_VERSION;
}
