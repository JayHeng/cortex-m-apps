/*
 * Copyright (c) 2018, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_POWER_H_
#define _FSL_POWER_H_

#include "fsl_common.h"

/** @defgroup power
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief power driver version 2.0.0. */
#define FSL_POWER_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

#define MAKE_PD_BITS(reg, slot) (((reg) << 8) | (slot))
#define SYSCTL0_PDRCFGSET_REG(x) (*((volatile uint32_t *)((uint32_t)(&(SYSCTL0->PDRUNCFG0_SET)) + ((x) << 2U))))
#define SYSCTL0_PDRCFGCLR_REG(x) (*((volatile uint32_t *)((uint32_t)(&(SYSCTL0->PDRUNCFG0_CLR)) + ((x) << 2U))))
#define PDRCFG0 0x0U
#define PDRCFG1 0x1U
#define PDRCFG2 0x2U
#define PDRCFG3 0x3U

typedef enum pd_bits
{
    kPDRUNCFG_PMC_MODE0 = MAKE_PD_BITS(PDRCFG0, 1U),
    kPDRUNCFG_PMC_MODE1 = MAKE_PD_BITS(PDRCFG0, 2U),
    kPDRUNCFG_LP_VDD_COREREG = MAKE_PD_BITS(PDRCFG0, 4U),
    kPDRUNCFG_LP_PMCREF = MAKE_PD_BITS(PDRCFG0, 6U),
    kPDRUNCFG_PD_HVD1V8 = MAKE_PD_BITS(PDRCFG0, 7U),
    kPDRUNCFG_LP_PORCORE = MAKE_PD_BITS(PDRCFG0, 8U),
    kPDRUNCFG_LP_LVDCORE = MAKE_PD_BITS(PDRCFG0, 9U),
    kPDRUNCFG_PD_HVDCORE = MAKE_PD_BITS(PDRCFG0, 10U),
    kPDRUNCFG_PD_RBB = MAKE_PD_BITS(PDRCFG0, 11U),
    kPDRUNCFG_PD_FBB = MAKE_PD_BITS(PDRCFG0, 12U),
    kPDRUNCFG_PD_SYSXTAL = MAKE_PD_BITS(PDRCFG0, 13U),
    kPDRUNCFG_PD_LPOSC = MAKE_PD_BITS(PDRCFG0, 14U),
    kPDRUNCFG_PD_SFRO = MAKE_PD_BITS(PDRCFG0, 15U),
    kPDRUNCFG_PD_FFRO = MAKE_PD_BITS(PDRCFG0, 16U),
    kPDRUNCFG_PD_SYSPLL_LDO = MAKE_PD_BITS(PDRCFG0, 17U),
    kPDRUNCFG_PD_SYSPLL_ANA = MAKE_PD_BITS(PDRCFG0, 18U),
    kPDRUNCFG_PD_AUDPLL_LDO = MAKE_PD_BITS(PDRCFG0, 19U),
    kPDRUNCFG_PD_AUDPLL_ANA = MAKE_PD_BITS(PDRCFG0, 20U),
    kPDRUNCFG_PD_ADC = MAKE_PD_BITS(PDRCFG0, 21U),
    kPDRUNCFG_LP_ADC = MAKE_PD_BITS(PDRCFG0, 22U),
    kPDRUNCFG_PD_ADC_TEMPSNS = MAKE_PD_BITS(PDRCFG0, 23U),
    kPDRUNCFG_PD_PMU_TEMPSNS = MAKE_PD_BITS(PDRCFG0, 24U),
    kPDRUNCFG_PD_ACMP = MAKE_PD_BITS(PDRCFG0, 25U),
    kPDRUNCFG_PD_USB = MAKE_PD_BITS(PDRCFG0, 26U),
    kPDRUNCFG_LP_HSPAD_VDET = MAKE_PD_BITS(PDRCFG0, 28U),
    kPDRUNCFG_PD_HSPAD_REF = MAKE_PD_BITS(PDRCFG0, 29U),
    kPDRUNCFG_EN_WAKE_DELAY = MAKE_PD_BITS(PDRCFG0, 30U),
    kPDRUNCFG_EN_AUTO_WKUP = MAKE_PD_BITS(PDRCFG0, 31U),

    kPDRUNCFG_APD_PQ_SRAM = MAKE_PD_BITS(PDRCFG1, 0U),
    kPDRUNCFG_PPD_PQ_SRAM = MAKE_PD_BITS(PDRCFG1, 1U),
    kPDRUNCFG_APD_QSPI_SRAM = MAKE_PD_BITS(PDRCFG1, 2U),
    kPDRUNCFG_PPD_QSPI_SRAM = MAKE_PD_BITS(PDRCFG1, 3U),
    kPDRUNCFG_APD_USBHS_SRAM = MAKE_PD_BITS(PDRCFG1, 4U),
    kPDRUNCFG_PPD_USBHS_SRAM = MAKE_PD_BITS(PDRCFG1, 5U),
    kPDRUNCFG_APD_USDHC0_SRAM = MAKE_PD_BITS(PDRCFG1, 6U),
    kPDRUNCFG_PPD_USDHC0_SRAM = MAKE_PD_BITS(PDRCFG1, 7U),
    kPDRUNCFG_APD_USDHC1_SRAM = MAKE_PD_BITS(PDRCFG1, 8U),
    kPDRUNCFG_PPD_USDHC1_SRAM = MAKE_PD_BITS(PDRCFG1, 9U),
    kPDRUNCFG_APD_CASPER_SRAM = MAKE_PD_BITS(PDRCFG1, 10U),
    kPDRUNCFG_PPD_CASPER_SRAM = MAKE_PD_BITS(PDRCFG1, 11U),
    kPDRUNCFG_APD_DSP_CACHE_REGF = MAKE_PD_BITS(PDRCFG1, 24U),
    kPDRUNCFG_PPD_DSP_CACHE_REGF = MAKE_PD_BITS(PDRCFG1, 25U),
    kPDRUNCFG_APD_DSP_TCM_REGF = MAKE_PD_BITS(PDRCFG1, 26U),
    kPDRUNCFG_PPD_DSP_TCM_REGF = MAKE_PD_BITS(PDRCFG1, 27U),
    kPDRUNCFG_PD_ROM = MAKE_PD_BITS(PDRCFG1, 28U),
    kPDRUNCFG_IGNORE_OK_SIG = MAKE_PD_BITS(PDRCFG1, 30U),
    kPDRUNCFG_SRAM_SLEEP = MAKE_PD_BITS(PDRCFG1, 31U),

    kPDRUNCFG_APD_SRAM_IF0 = MAKE_PD_BITS(PDRCFG2, 0U),
    kPDRUNCFG_APD_SRAM_IF1 = MAKE_PD_BITS(PDRCFG2, 1U),
    kPDRUNCFG_APD_SRAM_IF2 = MAKE_PD_BITS(PDRCFG2, 2U),
    kPDRUNCFG_APD_SRAM_IF3 = MAKE_PD_BITS(PDRCFG2, 3U),
    kPDRUNCFG_APD_SRAM_IF4 = MAKE_PD_BITS(PDRCFG2, 4U),
    kPDRUNCFG_APD_SRAM_IF5 = MAKE_PD_BITS(PDRCFG2, 5U),
    kPDRUNCFG_APD_SRAM_IF6 = MAKE_PD_BITS(PDRCFG2, 6U),
    kPDRUNCFG_APD_SRAM_IF7 = MAKE_PD_BITS(PDRCFG2, 7U),
    kPDRUNCFG_APD_SRAM_IF8 = MAKE_PD_BITS(PDRCFG2, 8U),
    kPDRUNCFG_APD_SRAM_IF9 = MAKE_PD_BITS(PDRCFG2, 9U),
    kPDRUNCFG_APD_SRAM_IF10 = MAKE_PD_BITS(PDRCFG2, 10U),
    kPDRUNCFG_APD_SRAM_IF11 = MAKE_PD_BITS(PDRCFG2, 11U),
    kPDRUNCFG_APD_SRAM_IF12 = MAKE_PD_BITS(PDRCFG2, 12U),
    kPDRUNCFG_APD_SRAM_IF13 = MAKE_PD_BITS(PDRCFG2, 13U),
    kPDRUNCFG_APD_SRAM_IF14 = MAKE_PD_BITS(PDRCFG2, 14U),
    kPDRUNCFG_APD_SRAM_IF15 = MAKE_PD_BITS(PDRCFG2, 15U),
    kPDRUNCFG_APD_SRAM_IF16 = MAKE_PD_BITS(PDRCFG2, 16U),
    kPDRUNCFG_APD_SRAM_IF17 = MAKE_PD_BITS(PDRCFG2, 17U),
    kPDRUNCFG_APD_SRAM_IF18 = MAKE_PD_BITS(PDRCFG2, 18U),
    kPDRUNCFG_APD_SRAM_IF19 = MAKE_PD_BITS(PDRCFG2, 19U),
    kPDRUNCFG_APD_SRAM_IF20 = MAKE_PD_BITS(PDRCFG2, 20U),
    kPDRUNCFG_APD_SRAM_IF21 = MAKE_PD_BITS(PDRCFG2, 21U),
    kPDRUNCFG_APD_SRAM_IF22 = MAKE_PD_BITS(PDRCFG2, 22U),
    kPDRUNCFG_APD_SRAM_IF23 = MAKE_PD_BITS(PDRCFG2, 23U),
    kPDRUNCFG_APD_SRAM_IF24 = MAKE_PD_BITS(PDRCFG2, 24U),
    kPDRUNCFG_APD_SRAM_IF25 = MAKE_PD_BITS(PDRCFG2, 25U),
    kPDRUNCFG_APD_SRAM_IF26 = MAKE_PD_BITS(PDRCFG2, 26U),
    kPDRUNCFG_APD_SRAM_IF27 = MAKE_PD_BITS(PDRCFG2, 27U),
    kPDRUNCFG_APD_SRAM_IF28 = MAKE_PD_BITS(PDRCFG2, 28U),
    kPDRUNCFG_APD_SRAM_IF29 = MAKE_PD_BITS(PDRCFG2, 29U),
    kPDRUNCFG_APD_SRAM_IF30 = MAKE_PD_BITS(PDRCFG2, 30U),
    kPDRUNCFG_APD_SRAM_IF31 = MAKE_PD_BITS(PDRCFG2, 31U),

    kPDRUNCFG_PPD_SRAM_IF0 = MAKE_PD_BITS(PDRCFG3, 0U),
    kPDRUNCFG_PPD_SRAM_IF1 = MAKE_PD_BITS(PDRCFG3, 1U),
    kPDRUNCFG_PPD_SRAM_IF2 = MAKE_PD_BITS(PDRCFG3, 2U),
    kPDRUNCFG_PPD_SRAM_IF3 = MAKE_PD_BITS(PDRCFG3, 3U),
    kPDRUNCFG_PPD_SRAM_IF4 = MAKE_PD_BITS(PDRCFG3, 4U),
    kPDRUNCFG_PPD_SRAM_IF5 = MAKE_PD_BITS(PDRCFG3, 5U),
    kPDRUNCFG_PPD_SRAM_IF6 = MAKE_PD_BITS(PDRCFG3, 6U),
    kPDRUNCFG_PPD_SRAM_IF7 = MAKE_PD_BITS(PDRCFG3, 7U),
    kPDRUNCFG_PPD_SRAM_IF8 = MAKE_PD_BITS(PDRCFG3, 8U),
    kPDRUNCFG_PPD_SRAM_IF9 = MAKE_PD_BITS(PDRCFG3, 9U),
    kPDRUNCFG_PPD_SRAM_IF10 = MAKE_PD_BITS(PDRCFG3, 10U),
    kPDRUNCFG_PPD_SRAM_IF11 = MAKE_PD_BITS(PDRCFG3, 11U),
    kPDRUNCFG_PPD_SRAM_IF12 = MAKE_PD_BITS(PDRCFG3, 12U),
    kPDRUNCFG_PPD_SRAM_IF13 = MAKE_PD_BITS(PDRCFG3, 13U),
    kPDRUNCFG_PPD_SRAM_IF14 = MAKE_PD_BITS(PDRCFG3, 14U),
    kPDRUNCFG_PPD_SRAM_IF15 = MAKE_PD_BITS(PDRCFG3, 15U),
    kPDRUNCFG_PPD_SRAM_IF16 = MAKE_PD_BITS(PDRCFG3, 16U),
    kPDRUNCFG_PPD_SRAM_IF17 = MAKE_PD_BITS(PDRCFG3, 17U),
    kPDRUNCFG_PPD_SRAM_IF18 = MAKE_PD_BITS(PDRCFG3, 18U),
    kPDRUNCFG_PPD_SRAM_IF19 = MAKE_PD_BITS(PDRCFG3, 19U),
    kPDRUNCFG_PPD_SRAM_IF20 = MAKE_PD_BITS(PDRCFG3, 20U),
    kPDRUNCFG_PPD_SRAM_IF21 = MAKE_PD_BITS(PDRCFG3, 21U),
    kPDRUNCFG_PPD_SRAM_IF22 = MAKE_PD_BITS(PDRCFG3, 22U),
    kPDRUNCFG_PPD_SRAM_IF23 = MAKE_PD_BITS(PDRCFG3, 23U),
    kPDRUNCFG_PPD_SRAM_IF24 = MAKE_PD_BITS(PDRCFG3, 24U),
    kPDRUNCFG_PPD_SRAM_IF25 = MAKE_PD_BITS(PDRCFG3, 25U),
    kPDRUNCFG_PPD_SRAM_IF26 = MAKE_PD_BITS(PDRCFG3, 26U),
    kPDRUNCFG_PPD_SRAM_IF27 = MAKE_PD_BITS(PDRCFG3, 27U),
    kPDRUNCFG_PPD_SRAM_IF28 = MAKE_PD_BITS(PDRCFG3, 28U),
    kPDRUNCFG_PPD_SRAM_IF29 = MAKE_PD_BITS(PDRCFG3, 29U),
    kPDRUNCFG_PPD_SRAM_IF30 = MAKE_PD_BITS(PDRCFG3, 30U),
    kPDRUNCFG_PPD_SRAM_IF31 = MAKE_PD_BITS(PDRCFG3, 31U),
    /*
    This enum member has no practical meaning,it is used to avoid MISRA issue,
    user should not trying to use it.
    */
    kPDRUNCFG_ForceUnsigned = (int)0x80000000U,
} pd_bit_t;

/* Power mode configuration API parameter */
typedef enum _power_mode_config
{
    kPmu_Sleep = 0U,
    kPmu_Deep_Sleep = 1U,
    kPmu_Deep_PowerDown = 2U,
    kPmu_Full_Deep_PowerDown = 3U,
} power_mode_cfg_t;
/* PMIC mode pin configuration API parameter */
#define SYSCTL0_TUPLE_REG(reg) (*((volatile uint32_t *)(((uint32_t)(SYSCTL0)) + ((reg)&0xFFFU))))

typedef enum _pmic_mode_reg
{
    kCfg_Run = 0x610,
    kCfg_Sleep = 0x600,
} pmic_mode_reg_t;
/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief API to enable PDRUNCFG bit in the Sysctl0. Note that enabling the bit powers down the peripheral
 *
 * @param en    peripheral for which to enable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_EnablePD(pd_bit_t en)
{
    /* PDRUNCFGSET */
    SYSCTL0_PDRCFGSET_REG(en >> 8UL) = (1UL << (en & 0xffU));
}

/*!
 * @brief API to disable PDRUNCFG bit in the Sysctl0. Note that disabling the bit powers up the peripheral
 *
 * @param en    peripheral for which to disable the PDRUNCFG bit
 * @return none
 */
static inline void POWER_DisablePD(pd_bit_t en)
{
    /* PDRUNCFGCLR */
    SYSCTL0_PDRCFGCLR_REG(en >> 8UL) = (1UL << (en & 0xffU));
}
/*!
 * @brief API to apply updated PMC PDRUNCFG bits in the Sysctl0.
 *
 * @param none
 * @return none
 */
static inline void POWER_ApplyPD(void)
{
    PMC->CTRL |= PMC_CTRL_APPLYCFG_MASK;
    /*Wait all PMC finite state machines are idle*/
    while ((PMC->CTRL & PMC_STATUS_ACTIVEFSM_MASK) != 0)
        ;
}

/*!
 * @brief API to enable deep sleep bit in the ARM Core.
 *
 * @param none
 * @return none
 */
static inline void POWER_EnableDeepSleep(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

/*!
 * @brief API to disable deep sleep bit in the ARM Core.
 *
 * @param none
 * @return none
 */
static inline void POWER_DisableDeepSleep(void)
{
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

/**
 * @brief   Clears the PMC event flags state.
 * @param   statusMask : A bitmask of event flags that are to be cleared.
 * @return  none
 */
static inline void POWER_ClearEventFlags(uint32_t statusMask)
{
    PMC->FLAGS = statusMask;
}

/**
 * @brief   Get the PMC event flags state.
 * @param   none
 * @return  PMC FLAGS register value
 */
static inline uint32_t POWER_GetEventFlags(void)
{
    return PMC->FLAGS;
}

/**
 * @brief   Enable the PMC interrupt requests.
 * @param   interruptMask : A bitmask of of interrupts to enable.
 * @return  none
 */
static inline void POWER_EnableInterrupts(uint32_t interruptMask)
{
    PMC->CTRL |= interruptMask;
}

/**
 * @brief   Get PMIC_MODE pins configure value.
 * @param   reg : PDSLEEPCFG0 or PDRUNCFG0 register offset
 * @return  PMIC_MODE pins value in PDSLEEPCFG0
 */
static inline uint32_t POWER_GetPmicMode(pmic_mode_reg_t reg)
{
    return ((SYSCTL0_TUPLE_REG(reg) & (SYSCTL0_PDSLEEPCFG0_PMIC_MODE0_MASK | SYSCTL0_PDSLEEPCFG0_PMIC_MODE1_MASK)) >> SYSCTL0_PDRUNCFG0_PMIC_MODE0_SHIFT);
}

/**
 * @brief   Set PMIC_MODE pins configure value.
 * @param   mode : PMIC MODE pin value
 * @param   reg : PDSLEEPCFG0 or PDRUNCFG0 register offset
 * @return  PMIC_MODE pins value in PDSLEEPCFG0
 */
void POWER_SetPmicMode(uint32_t mode, pmic_mode_reg_t reg);

/**
 * @brief   Configures and enters in SLEEP low power mode
 * @param   none
 * @return  none
 */
void POWER_EnterSleep(void);

/**
 * @brief   PMC Deep Sleep function call
 * @param   exclude_from_pd  Bit mask of the PDRUNCFG0 ~ PDRUNCFG3 that needs to be powered on during Deep Sleep mode
 * selected.
 * @return  none
 */
void POWER_EnterDeepSleep(const uint32_t exclude_from_pd[4]);

/**
 * @brief   PMC Deep Power Down function call
 * @param   exclude_from_pd  Bit mask of the PDRUNCFG0 ~ PDRUNCFG3 that needs to be powered on during Deep Power Down
 * mode selected.
 * @return  none
 */
void POWER_EnterDeepPowerDown(const uint32_t exclude_from_pd[4]);

/**
 * @brief   PMC Full Deep Power Down function call
 * @param   exclude_from_pd  Bit mask of the PDRUNCFG0 ~ PDRUNCFG3 that needs to be powered on during Full Deep Power
 * Down mode selected.
 * @return  none
 */
void POWER_EnterFullDeepPowerDown(const uint32_t exclude_from_pd[4]);

/*!
 * @brief Power Library API to enter different power mode.
 *
 * @param exclude_from_pd  Bit mask of the PDRUNCFG0 ~ PDRUNCFG3 that needs to be powered on during power mode selected.
 * @return none
 */
void POWER_EnterPowerMode(power_mode_cfg_t mode, const uint32_t exclude_from_pd[4]);
/*!
 * @brief Enable specific interrupt for wake-up from deep-sleep mode.
 * Enable the interrupt for wake-up from deep sleep mode.
 * Some interrupts are typically used in sleep mode only and will not occur during
 * deep-sleep mode because relevant clocks are stopped. However, it is possible to enable
 * those clocks (significantly increasing power consumption in the reduced power mode),
 * making these wake-ups possible.
 * @note This function also enables the interrupt in the NVIC (EnableIRQ() is called internally).
 * @param interrupt The IRQ number.
 * @return none
 */
void EnableDeepSleepIRQ(IRQn_Type interrupt);

/*!
 * @brief Disable specific interrupt for wake-up from deep-sleep mode.
 * Disable the interrupt for wake-up from deep sleep mode.
 * Some interrupts are typically used in sleep mode only and will not occur during
 * deep-sleep mode because relevant clocks are stopped. However, it is possible to enable
 * those clocks (significantly increasing power consumption in the reduced power mode),
 * making these wake-ups possible.
 * @note This function also disables the interrupt in the NVIC (DisableIRQ() is called internally).
 * @param interrupt The IRQ number.
 * @return none
 */
void DisableDeepSleepIRQ(IRQn_Type interrupt);
/*!
 * @brief Power Library API to return the library version.
 *
 * @param none
 * @return version number of the power library
 */
uint32_t POWER_GetLibVersion(void);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _FSL_POWER_H_ */
