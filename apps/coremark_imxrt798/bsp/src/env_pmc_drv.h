/*! Driver version: v1.0*/
/*! Date: 2023-11-24 */
#ifndef __ENV_PMC_DRV_H_
#define __ENV_PMC_DRV_H_
#include "fsl_common.h"
extern PMC_Type *PMC_POINTER;

#define PMC_PDRUNCFG0_AFBBSR1_PD_MASK (0x40000000U)

typedef enum _pmc_status
{
    kPMC_Status_Busy = 0U,

    kPMC_Status_VDD1_LVD = 18U,
    kPMC_Status_VDD2_LVD = 19U,
    kPMC_Status_VDDN_LVD = 20U,
    kPMC_Status_VDD1_HVD = 21U,
    kPMC_Status_VDD2_HVD = 22U,
    kPMC_Status_VDDN_HVD = 23U,
    kPMC_Status_VDD1V8_HVD = 24U,
    kPMC_Status_RTC_DPDWakeup = 25U,

    kPMC_Status_IntPin = 27U,

    kPMC_Status_Sense_DeepSleep = 29U,
    kPMC_Status_Compute_DeepSleep = 30U,
} pmc_status_t;

typedef enum _pmc_control_flag
{
    kPMC_DualCoreDeepSleep = 0U,
    kPMC_ITRC_Reset = 1U,
    kPMC_VDD1Sram_BBError = 2U,
    kPMC_VDD2Sram_BBError = 3U,
    kPMC_VDD1_BBError = 4U,
    kPMC_VDD2_BBError = 5U,
    kPMC_VDDN_BBError = 6U,
    kPMC_VDD1_SCPC_OverCur = 7U,
    kPMC_VDD2_SCPC_OverCur = 8U,
    kPMC_DCDC_HPM_OverCur = 9U,
    kPMC_DCDC_LPM_OverCur = 10U,
    kPMC_VDD1_POR = 11U,
    kPMC_VDD2_POR = 12U,
    kPMC_VDDN_POR = 13U,
    kPMC_VDD1V8_POR = 14U,
    kPMC_VDD1V8AON_POR = 15U,
    kPMC_VDD1_AGDET = 16U,
    kPMC_VDD2_AGDET = 17U,
    kPMC_VDD1_LVD = 18U,
    kPMC_VDD2_LVD = 19U,
    kPMC_VDDN_LVD = 20U,
    kPMC_VDD1_HVD = 21U,
    kPMC_VDD2_HVD = 22U,
    kPMC_VDDN_HVD = 23U,
    kPMC_VDD1V8_HVD = 24U,
    kPMC_RTC_DPDWakeup = 25U,
    kPMC_PMC_AutoWakeup = 26U,
    kPMC_IntPin = 27U,
    kPMC_ResetPin = 28U,
    kPMC_Sense_DeepSleep = 29U,
    kPMC_Compute_DeepSleep = 30U,
    kPMC_DPDWakeup = 31U,
} pmc_control_flag_t;


typedef struct _pmc_domain_power_switch
{
    bool full_DSR;//bit 2, for low power
    bool DPD;//bit 3, for low power
    bool full_DPD;//bit 4, for low power
    bool compute_DSR;//bit 5, for low power
    bool mediaVdd2Vddn_DSR;
    bool commonVdd2_DSR;//bit 7, for low power, and PMC_SENSE can set in run
    bool commonVddn_DSR;
    bool dsp_PD;
    bool mipiPhy_PD;
} pmc_domain_power_switch_t;

typedef enum _pmc_dcdc_vsel
{
    kPMC_DCDC_VSEL0 = 0U,
    kPMC_DCDC_VSEL1 = 1U,
} pmc_dcdc_vsel_t;

typedef enum _pmc_ldo_name
{
    kPMC_VDD1_LDO = 1U,
    kPMC_VDD2_LDO = 2U,
} pmc_ldo_name_t;

typedef enum _pmc_ldo_vsel
{
    kPMC_LDO_VSEL0 = 0U,
    kPMC_LDO_VSEL1 = 1U,
    kPMC_LDO_VSEL2 = 2U,
    kPMC_LDO_VSEL3 = 3U,
} pmc_ldo_vsel_t;

typedef enum _pmc_power_mode
{
    kPMC_HighPower = 0U,
    kPMC_LowPower = 1U,
} pmc_power_mode_t;

typedef enum _pmc_ldo_power_mode
{
    kPMC_LDO_Bypass = 0U,
    kPMC_LDO_HighPower = 1U,
    kPMC_LDO_LowPower = 2U,
    kPMC_LDO_LowPower_Also = 3U,
} pmc_ldo_power_mode_t;

typedef struct _pmc_dcdc_config
{
    pmc_power_mode_t dcdcMode;
    pmc_dcdc_vsel_t  dcdcVsel;
    double dcdcVoltage;
} pmc_dcdc_config_t;

typedef struct _pmc_ldo_config
{
    pmc_ldo_power_mode_t ldoMode;
    pmc_ldo_vsel_t       ldoVsel;
    double ldoVoltage;
} pmc_ldo_config_t;

typedef enum _pmc_sys_config
{
    kPMC_ActiveConfig = 0U,
    kPMC_LowPowerConfig = 1U,
} pmc_sys_config_t;

typedef enum _pmc_vdd_rail
{
    kPMC_Vdd1 = 1U,
    kPMC_Vdd2 = 2U,
    kPMC_Vddn = 3U,
} pmc_vdd_rail_t;

typedef enum _pmc_vdd_body_bias
{
    kPMC_AFBB_Performance = 1U,
    kPMC_RBB_PowerSave = 2U,
    kPMC_NoBB = 3U,
} pmc_vdd_body_bias_t;

typedef enum _pmc_sram_arbiter
{
    kPMC_SramArbiter0_VDD2 = 0U,
    kPMC_SramArbiter1_VDD1 = 1U,
} pmc_sram_arbiter_t;

typedef enum _pmc_vdd_sram_body_bias
{
    kPMC_SRAM_AFBB_Performance = 1U,
    kPMC_SRAM_RBB_PowerSave = 4U,
    kPMC_SRAM_NoBB = 5U,
} pmc_vdd_sram_body_bias_t;

typedef struct _pmc_body_bias_config
{
    pmc_vdd_body_bias_t      vdd1_bbMode;
    pmc_vdd_body_bias_t      vdd2_bbMode;
    pmc_vdd_body_bias_t      vddn_bbMode;
    pmc_vdd_sram_body_bias_t vdd1Sram1_bbMode;
    pmc_vdd_sram_body_bias_t vdd2Sram0_bbMode;
    // bool vdd1RBB_PowerDown;
    // bool vdd1AFBB_PowerDown;
    // bool vdd2RBB_PowerDown;
    // bool vdd2AFBB_PowerDown;
    // bool vddnRBB_PowerDown;
    // bool vddnAFBB_PowerDown;
    // bool vdd1SramRBB_PowerDown;
    // bool vdd2SramRBB_PowerDown;
    // bool vdd1SramAFBB_PowerDown;
    // bool vdd2SramAFBB_PowerDown;
} pmc_body_bias_config_t;

typedef enum _pmc_vref_mode
{
    kPMC_Vref_ForceHighPower = 0U,
    kPMC_Vref_InDueCourseLowPower = 1U,
} pmc_vref_mode_t;

typedef struct _pmc_power_config_group_1
{
    bool tempSensor_PD;
    pmc_vref_mode_t  vrefMode;
    bool vdd1v8Hvd_PD;
    pmc_power_mode_t vdd1PorMode;
    pmc_power_mode_t vdd1LvdMode;
    bool vdd1Hvd_PD;
    bool vdd1Agdet_Disable;
    pmc_power_mode_t vdd2PorMode;
    pmc_power_mode_t vdd2LvdMode;
    bool vdd2Hvd_PD;
    bool vdd2Agdet_Disable;
    pmc_power_mode_t vddnPorMode;
    pmc_power_mode_t vddnLvdMode;
    bool vddnHvd_PD;

    bool otp_PD;//sense cannot control
    bool rom_PD;//sense cannot control
} pmc_power_config_group_1_t;

//group 2 and 3
typedef enum _pmc_main_ram_power_down
{
    //common vdd2 domain
    kPMC_MainRamArray0  = 1UL << 0U,//compute cannot control Main Ram 0
    kPMC_MainRamArray1  = 1UL << 1U,
    kPMC_MainRamArray2  = 1UL << 2U,
    kPMC_MainRamArray3  = 1UL << 3U,
    kPMC_MainRamArray4  = 1UL << 4U,
    kPMC_MainRamArray5  = 1UL << 5U,
    kPMC_MainRamArray6  = 1UL << 6U,
    kPMC_MainRamArray7  = 1UL << 7U,
    kPMC_MainRamArray8  = 1UL << 8U,
    kPMC_MainRamArray9  = 1UL << 9U,
    kPMC_MainRamArray10 = 1UL << 10U,
    kPMC_MainRamArray11 = 1UL << 11U,
    kPMC_MainRamArray12 = 1UL << 12U,
    kPMC_MainRamArray13 = 1UL << 13U,
    kPMC_MainRamArray14 = 1UL << 14U,
    kPMC_MainRamArray15 = 1UL << 15U,
    kPMC_MainRamArray16 = 1UL << 16U,
    kPMC_MainRamArray17 = 1UL << 17U,

    //sense domain
    kPMC_MainRamArray18 = 1UL << 18U,
    kPMC_MainRamArray19 = 1UL << 19U,
    kPMC_MainRamArray20 = 1UL << 20U,
    kPMC_MainRamArray21 = 1UL << 21U,
    kPMC_MainRamArray22 = 1UL << 22U,
    kPMC_MainRamArray23 = 1UL << 23U,
    kPMC_MainRamArray24 = 1UL << 24U,
    kPMC_MainRamArray25 = 1UL << 25U,
    kPMC_MainRamArray26 = 1UL << 26U,
    kPMC_MainRamArray27 = 1UL << 27U,
    kPMC_MainRamArray28 = 1UL << 28U,
    kPMC_MainRamArray29 = 1UL << 29U,

    kPMC_MainRamArray_AllOn = 0UL,
    kPMC_MainRamArray_AllOff = 0x3FFFFFFF,
    kPMC_MainRamArray_DefaultOff = 0x3FFFFFFE,//defaultly only Main Ram 0 is on
    kPMC_MainRamArray_Reserved = 0xC0000000,
} pmc_main_ram_power_down_t;

//group 4 and 5
typedef enum _pmc_periph_ram_power_down
{
    kPMC_PeriphRam_Sdhc0 = 1UL << 0U,
    kPMC_PeriphRam_Sdhc1 = 1UL << 1U,
    kPMC_PeriphRam_Usb0 = 1UL << 2U,
    kPMC_PeriphRam_Usb1 = 1UL << 3U,
    kPMC_PeriphRam_Jpeg = 1UL << 4U,
    kPMC_PeriphRam_Png = 1UL << 5U,
    kPMC_PeriphRam_Mipi = 1UL << 6U,
    kPMC_PeriphRam_Gpu = 1UL << 7U,
    kPMC_PeriphRam_Dma2_3 = 1UL << 8U,
    kPMC_PeriphRam_Dma0_1_Pkc_Etf = 1UL << 9U,
    kPMC_PeriphRam_Cpu0_Ccache = 1UL << 10U,
    kPMC_PeriphRam_Cpu0_Scache = 1UL << 11U,
    kPMC_PeriphRam_Dsp_Icache = 1UL << 12U,
    kPMC_PeriphRam_Dsp_Dcache = 1UL << 13U,
    kPMC_PeriphRam_Dsp_Itcm = 1UL << 14U,
    kPMC_PeriphRam_Dsp_Dtcm = 1UL << 15U,
    kPMC_PeriphRam_Ezh_Tcm = 1UL << 16U,
    kPMC_PeriphRam_Neutron = 1UL << 17U,
    kPMC_PeriphRam_Xspi0_Mmu0_Cache = 1UL << 18U,
    kPMC_PeriphRam_Xspi1_Mmu1_Cache = 1UL << 19U,
    kPMC_PeriphRam_Xspi2_Mmu2_Cache = 1UL << 20U,
    kPMC_PeriphRam_Lcd = 1UL << 21U,
    kPMC_PeriphRam_Ocotp = 1UL << 22U,

    kPMC_PeriphRam_AllOn = 0UL,
    kPMC_PeriphRam_AllOff = 0x7FFFFFUL,
    kPMC_PeriphRam_ComputeDefaultOff = 0x3FF1FFUL,
    kPMC_PeriphRam_SenseDefaultOff = 0x3FFFFFUL,
    kPMC_PeriphRam_Reserved = 0xFF800000,
} pmc_periph_ram_power_down_t;

typedef struct _pmc_ram_power_down_cfg
{
    uint32_t powerDown;
} pmc_ram_power_down_cfg_t;

typedef enum _pmc_power_ctrl
{
    kPMC_PowerOn = 0U,
    kPMC_PowerDown = 1U,
} pmc_power_ctrl_t;


/*************************************
 *Get Status -- Shows currently aggregated value of Sense domain and Compute domain so should be the same
 *************************************/
typedef enum _pmc_pd_status_2bit
{
    kPMC_PD_Status_Pmic_Mode = 0U,
    kPMC_PD_Status_Ldo1_Mode = 14U,
    kPMC_PD_Status_Ldo1_Vsel = 16U,
    kPMC_PD_Status_Ldo2_Mode = 18U,
    kPMC_PD_Status_Ldo2_Vsel = 20U,
} pmc_pd_status_2bit_t;

typedef enum _pmc_pd_status
{
    kPMC_PD_Status_FullDSR        = 2U,
    kPMC_PD_Status_DPD             = 3U,
    kPMC_PD_Status_FullDPD         = 4U,
    kPMC_PD_Status_Compute_DSR    = 5U,
    kPMC_PD_Status_MediaVdd2_n_DSR= 6U,
    kPMC_PD_Status_CommonVdd2_DSR = 7U,
    kPMC_PD_Status_CommonVddn_DSR = 8U,
    kPMC_PD_Status_Dsp_PD          = 9U,
    kPMC_PD_Status_MipiPhy_PD      = 10U,
    kPMC_PD_Status_Dcdc_LP         = 12U,
    kPMC_PD_Status_Dcdc_Vsel       = 13U,
    
    kPMC_PD_Status_Vdd1RBB_PD      = 22U,
    kPMC_PD_Status_Vdd1AFBB_PD     = 23U,
    kPMC_PD_Status_Vdd2RBB_PD      = 24U,
    kPMC_PD_Status_Vdd2AFBB_PD     = 25U,
    kPMC_PD_Status_VddnRBB_PD      = 26U,
    kPMC_PD_Status_VddnAFBB_PD     = 27U,
    kPMC_PD_Status_Vdd1SramRBB_PD  = 28U,
    kPMC_PD_Status_Vdd2SramRBB_PD  = 29U,
    kPMC_PD_Status_Vdd1SramAFBB_PD = 30U,
    kPMC_PD_Status_Vdd2SramAFBB_PD = 31U,

    kPMC_PD_Status_TempSensor_PD   = (0U + 32U),
    kPMC_PD_Status_PMCREF_LP       = (1U + 32U),
    kPMC_PD_Status_Vdd1v8HVD_PD    = (2U + 32U),
    kPMC_PD_Status_Vdd1_POR        = (3U + 32U),
    kPMC_PD_Status_Vdd1_LVD        = (4U + 32U),
    kPMC_PD_Status_Vdd1HVD_PD      = (5U + 32U),
    kPMC_PD_Status_Vdd1AgdetHVD_PD = (6U + 32U),
    kPMC_PD_Status_Vdd2_POR        = (7U + 32U),
    kPMC_PD_Status_Vdd2_LVD        = (8U + 32U),
    kPMC_PD_Status_Vdd2HVD_PD      = (9U + 32U),
    kPMC_PD_Status_Vdd2AgdetHVD_PD = (10U + 32U),
    kPMC_PD_Status_Vddn_POR        = (11U + 32U),
    kPMC_PD_Status_Vddn_LVD        = (12U + 32U),
    kPMC_PD_Status_VddnHVD_PD      = (13U + 32U),
    kPMC_PD_Status_Otp_PD          = (15U + 32U),
    kPMC_PD_Status_Rom_PD          = (16U + 32U),

    kPMC_PD_Status_Sram_Sleep      = (31U + 32U),
} pmc_pd_status_t;

typedef enum _pmc_domain_power_status
{
	kPMC_Status_Sense_DSR = 0U,
	kPMC_Status_Compute_DSR = 1U,
	kPMC_Status_MediaVdd2Vddn_DSR = 2U,
	kPMC_Status_CommonVdd2_DSR = 3U,
	kPMC_Status_CommonVddn_DSR = 4U,
	kPMC_Status_DSP_PD = 5U,
	kPMC_Status_MIPI_PD = 6U,
	kPMC_Status_OTP_PD = 7U,
} pmc_domain_power_status_t;


static inline void PMC_ApplyUpdatedSetting(void)
{
    while(PMC_POINTER->STATUS & PMC_STATUS_BUSY_MASK);
    PMC_POINTER->CTRL |= PMC_CTRL_APPLYCFG_MASK;
    while(PMC_POINTER->CTRL & PMC_CTRL_APPLYCFG_MASK);
}

/*************************************
 *Get Status -- Shows currently aggregated value of Sense domain and Compute domain so should be the same
 *************************************/
static inline double PMC_GetDcdcVoltage(void)
{
    uint32_t value;
    uint32_t sel = (PMC_POINTER->PDRUNCFG0 & PMC_PDRUNCFG0_DCDC_VSEL_MASK);
    if(sel)
        value = (PMC_POINTER->DCDCVSEL & PMC_DCDCVSEL_VSEL1_MASK) >> PMC_DCDCVSEL_VSEL1_SHIFT;
    else
        value = (PMC_POINTER->DCDCVSEL & PMC_DCDCVSEL_VSEL0_MASK) >> PMC_DCDCVSEL_VSEL0_SHIFT;
    return (double)(value * 6.25 + 500);
}

static inline double PMC_GetLdoVoltage_Vdd1(void)
{
    uint32_t value;
    uint32_t sel = (PMC_POINTER->PDRUNCFG0 & PMC_PDRUNCFG0_LDO1_VSEL_MASK) >> PMC_PDRUNCFG0_LDO1_VSEL_SHIFT;
    if(sel == 0)
        value = (PMC_POINTER->LDOVDD1VSEL & PMC_LDOVDD1VSEL_VSEL0_MASK) >> PMC_LDOVDD1VSEL_VSEL0_SHIFT;
    else if(sel == 1)
        value = (PMC_POINTER->LDOVDD1VSEL & PMC_LDOVDD1VSEL_VSEL1_MASK) >> PMC_LDOVDD1VSEL_VSEL1_SHIFT;
    else if(sel == 2)
        value = (PMC_POINTER->LDOVDD1VSEL & PMC_LDOVDD1VSEL_VSEL2_MASK) >> PMC_LDOVDD1VSEL_VSEL2_SHIFT;
    else if(sel == 3)
        value = (PMC_POINTER->LDOVDD1VSEL & PMC_LDOVDD1VSEL_VSEL3_MASK) >> PMC_LDOVDD1VSEL_VSEL3_SHIFT;
    return (double)(value * 12.5 + 450);
}

static inline double PMC_GetLdoVoltage_Vdd2(void)
{
    uint32_t value;
    uint32_t sel = (PMC_POINTER->PDRUNCFG0 & PMC_PDRUNCFG0_LDO2_VSEL_MASK) >> PMC_PDRUNCFG0_LDO2_VSEL_SHIFT;
    if(sel == 0)
        value = (PMC_POINTER->LDOVDD2VSEL & PMC_LDOVDD2VSEL_VSEL0_MASK) >> PMC_LDOVDD2VSEL_VSEL0_SHIFT;
    else if(sel == 1)
        value = (PMC_POINTER->LDOVDD2VSEL & PMC_LDOVDD2VSEL_VSEL1_MASK) >> PMC_LDOVDD2VSEL_VSEL1_SHIFT;
    else if(sel == 2)
        value = (PMC_POINTER->LDOVDD2VSEL & PMC_LDOVDD2VSEL_VSEL2_MASK) >> PMC_LDOVDD2VSEL_VSEL2_SHIFT;
    else if(sel == 3)
        value = (PMC_POINTER->LDOVDD2VSEL & PMC_LDOVDD2VSEL_VSEL3_MASK) >> PMC_LDOVDD2VSEL_VSEL3_SHIFT;
    return (double)(value * 12.5 + 450);
}

static inline uint32_t PMC_GetStatus(pmc_status_t bitShift)
{
    return (PMC_POINTER->STATUS & (1UL << bitShift)) >> bitShift;
}

static inline uint32_t PMC_GetPmcFlag(pmc_control_flag_t bitShift)
{
    return (PMC_POINTER->FLAGS & (1UL << bitShift)) >> bitShift;
}

static inline void PMC_ClearPmcFlag(pmc_control_flag_t bitShift)
{
// #if defined CPU_MIMXRT798SGFOA_cm33_core1
//     assert(bitShift != 14U && bitShift != 15U && bitShift != 31U);
// #endif
    PMC_POINTER->FLAGS = (1UL << bitShift);
}

#if defined CPU_MIMXRT798SGFOA_cm33_core0
static inline void PMC_DetectResetEnable(pmc_control_flag_t bitShift, bool enable)
{
    assert(bitShift >= 16U && bitShift <= 24U);
    if(enable)
        PMC0->CTRL |= 1UL << bitShift;
    else
        PMC0->CTRL &= ~(1UL << bitShift);
}
#endif

static inline uint32_t PMC_GetDetectResetEnableStatus(pmc_control_flag_t bitShift)
{
    assert(bitShift >= 16U && bitShift <= 24U);
    return (PMC_POINTER->CTRL & (1UL << bitShift)) >> bitShift;
}

static inline void PMC_DetectIntEnable(pmc_control_flag_t bitShift, bool enable)
{
    assert((bitShift >= 2U && bitShift <= 10U) 
        || (bitShift >= 16U && bitShift <= 30U));
    if(enable)
        PMC_POINTER->INTRCTRL |= 1UL << bitShift;
    else
        PMC_POINTER->INTRCTRL &= ~(1UL << bitShift);
}

static inline uint32_t PMC_GetConfigStatus_2bit(pmc_pd_status_2bit_t bitShift)
{
    return (PMC_POINTER->PDCFGSTATUS0 & (3UL << bitShift)) >> bitShift;
}

static inline uint32_t PMC_GetConfigStatus(pmc_pd_status_t bitShift)
{
    uint32_t stat;
    if(bitShift < 32U)
        stat = (PMC_POINTER->PDCFGSTATUS0 & (1UL << bitShift)) >> bitShift;
    else
        stat = (PMC_POINTER->PDCFGSTATUS1 & (1UL << (bitShift - 32U))) >> (bitShift - 32U);
    return stat;
}

//!if array = kPMC_MainRamArray_All, can check any one array
static inline uint32_t PMC_GetConfigStatus_MainRamArrayPower(pmc_main_ram_power_down_t array)
{
    return PMC_POINTER->PDCFGSTATUS2 & array;
}

//!if array = kPMC_MainRamArray_All, can check any one array
static inline uint32_t PMC_GetConfigStatus_MainRamInterfacePower(pmc_main_ram_power_down_t array)
{
    return PMC_POINTER->PDCFGSTATUS3 & array;
}

//!if array = kPMC_PeriphRam_All, can check any one array
static inline uint32_t PMC_GetConfigStatus_PeriphRamArrayPower(pmc_periph_ram_power_down_t array)
{
    return PMC_POINTER->PDCFGSTATUS4 & array;
}

//!if array = kPMC_PeriphRam_All, can check any one array
static inline uint32_t PMC_GetConfigStatus_PeriphRamInterfacePower(pmc_periph_ram_power_down_t array)
{
    return PMC_POINTER->PDCFGSTATUS5 & array;
}

////////////////how to use PDWAKECFG


//The Compute and Sense slots each have their own set of flags. 
//Clearing a flag in the Compute slot will not clear the corresponding flag in the Sense slot.
static inline uint32_t PMC_GetDomainLastPowerDownRecord(pmc_domain_power_status_t flag)
{
	return (PMC_POINTER->PWRFLAGS & (1UL << flag)) >> flag;
}

static inline void PMC_ClearDomainLastPowerDownStatus(pmc_domain_power_status_t flag)
{
    PMC_POINTER->PWRFLAGS |= 1UL << flag;
}
typedef enum _system_run_mode
{
    MD_RUN = 1U,
    SD_RUN = 2U,
    OD_RUN = 3U,
    INIT_RUN = 4U,
} system_run_mode_t;
 
/********************************************************************/
extern system_run_mode_t curMode;
extern system_run_mode_t targetMode;
extern pmc_dcdc_config_t pmc_dcdc_active_cfg, pmc_dcdc_od_cfg, pmc_dcdc_sd_cfg, pmc_dcdc_md_cfg;
extern pmc_ldo_config_t pmc_ldo1_active_cfg, pmc_ldo2_active_cfg, pmc_ldo1_od_cfg, pmc_ldo1_sd_cfg, pmc_ldo1_md_cfg, pmc_ldo2_od_cfg, pmc_ldo2_sd_cfg, pmc_ldo2_md_cfg;
extern pmc_domain_power_switch_t psw_active_cfg;
extern pmc_body_bias_config_t bb_active_cfg;
extern pmc_power_config_group_1_t group1_active_cfg;
extern pmc_ram_power_down_cfg_t main_ram_array_active_cfg;
extern pmc_ram_power_down_cfg_t main_ram_interface_active_cfg;
extern pmc_ram_power_down_cfg_t periph_ram_array_active_cfg;
extern pmc_ram_power_down_cfg_t periph_ram_interface_active_cfg;

/*************************************
 *DCDC, LDO, PMIC configs
 *************************************/
#if defined CPU_MIMXRT798SGFOA_cm33_core0
void PMC_SetDcdcVoltage(pmc_dcdc_vsel_t num, double voltage_mV);
void PMC_SetLdoVoltage_Vdd2(pmc_ldo_vsel_t num, double voltage_mV);
#endif
void PMC_SetLdoVoltage_Vdd1(pmc_ldo_vsel_t num, double voltage_mV);

void PMC_SetDcdcMode(pmc_power_mode_t dcdcMode, pmc_dcdc_vsel_t dcdcVsel);
void PMC_SetLdoMode(pmc_ldo_name_t ldo, pmc_ldo_power_mode_t ldoMode, pmc_ldo_vsel_t ldoVsel);

void PMC_ConfigDcdcOutput(pmc_dcdc_config_t *config);
void PMC_ConfigLdoOutput(pmc_ldo_name_t ldo, pmc_ldo_config_t *config);
void PMC_SetPmicModePins(uint32_t num);

/*************************************
 *BB configs
 *************************************/
void PMC_ConfigVddRailBodyBias(pmc_sys_config_t sysMode, pmc_vdd_rail_t vdd, pmc_vdd_body_bias_t bbMode);
void PMC_ConfigSramArbiterBodyBias(pmc_sys_config_t sysMode, pmc_sram_arbiter_t vdd, pmc_vdd_sram_body_bias_t bbMode);
void PMC_ConfigBodyBias(pmc_sys_config_t sysMode, pmc_body_bias_config_t *config);

/*************************************
 *PD*CFG configs
 *************************************/
void PMC_Run_ConfigDomainPowerSwitch(pmc_domain_power_switch_t *config);
void PMC_LP_ConfigDomainPowerSwitch(pmc_domain_power_switch_t *config);

void PMC_ConfigGroup1(pmc_sys_config_t sysMode, pmc_power_config_group_1_t *config);

void PMC_MainRamArrayPowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config);
void PMC_MainRamInterfacePowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config);
void PMC_MainRamArrayPowerControl_Separate(pmc_sys_config_t sysMode, pmc_main_ram_power_down_t array, pmc_power_ctrl_t enable);
void PMC_MainRamInterfacePowerControl_Separate(pmc_sys_config_t sysMode, pmc_main_ram_power_down_t array, pmc_power_ctrl_t enable);

void PMC_PeriphRamArrayPowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config);
void PMC_PeriphRamInterfacePowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config);
void PMC_PeriphRamArrayPowerControl_Separate(pmc_sys_config_t sysMode, pmc_periph_ram_power_down_t name, pmc_power_ctrl_t enable);
void PMC_PeriphRamInterfacePowerControl_Separate(pmc_sys_config_t sysMode, pmc_periph_ram_power_down_t name, pmc_power_ctrl_t enable);

/*************************************
 *environment
 *************************************/
void ENV_PMC_ConfigActivePowerSupply(void);
void ENV_PMC_ConfigDomainAndMemoryPower(void);
void PMC_ReleasePower(void);

#endif