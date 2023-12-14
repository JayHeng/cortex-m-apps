/*! Driver version: v1.0*/
/*! Date: 2023-11-24 */
#include "env_pmc_drv.h"
//#include "env_sleepcon_drv.h"

#if defined CPU_MIMXRT798SGFOA_cm33_core1
    PMC_Type *PMC_POINTER = PMC1;
#elif defined CPU_MIMXRT798SGFOA_cm33_core0
    PMC_Type *PMC_POINTER = PMC0;
#endif

/*************************************
 *power supply config
 *************************************/
#define OD_MODE   
   
system_run_mode_t curMode = INIT_RUN;
system_run_mode_t targetMode
#if defined OD_MODE
    = OD_RUN; //DCDC1.1v
#elif defined SD_MODE
    = SD_RUN; //DCDC1.0v
#elif defined MD_MODE
    = MD_RUN; //DCDC0.7v
#endif  
    
pmc_dcdc_config_t pmc_dcdc_active_cfg;
pmc_ldo_config_t pmc_ldo1_active_cfg, pmc_ldo2_active_cfg;

pmc_dcdc_config_t pmc_dcdc_od_cfg = {
    .dcdcMode = kPMC_HighPower,
    .dcdcVsel = kPMC_DCDC_VSEL1,//VSEL0 reset value = 1.0, VSEL1 reset value = 1.1
    .dcdcVoltage = 1100-6.25*5,//mV
};
pmc_ldo_config_t pmc_ldo1_od_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL3,//VSEL3 reset value = 1.0
    .ldoVoltage = 1000-12.5*2,
};
pmc_ldo_config_t pmc_ldo2_od_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL3,//VSEL3 reset value = 1.0
    .ldoVoltage = 1000-12.5*2,
};

pmc_dcdc_config_t pmc_dcdc_sd_cfg = {
    .dcdcMode = kPMC_HighPower,
    .dcdcVsel = kPMC_DCDC_VSEL0,//VSEL0 reset value = 1.0, VSEL1 reset value = 1.1
    .dcdcVoltage = 1000,//mV
};
pmc_ldo_config_t pmc_ldo1_sd_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL3,//VSEL3 reset value = 1.0
    .ldoVoltage = 1000,
};
pmc_ldo_config_t pmc_ldo2_sd_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL3,//VSEL3 reset value = 1.0
    .ldoVoltage = 1000,
};

pmc_dcdc_config_t pmc_dcdc_md_cfg = {
    .dcdcMode = kPMC_HighPower,
    .dcdcVsel = kPMC_DCDC_VSEL0,//VSEL0 reset value = 1.0, VSEL1 reset value = 1.1
    .dcdcVoltage = 900,//mV
};
pmc_ldo_config_t pmc_ldo1_md_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL2,//VSEL2 reset value = 0.9
    .ldoVoltage = 900,
};
pmc_ldo_config_t pmc_ldo2_md_cfg = {
    .ldoMode = kPMC_LDO_HighPower,
    .ldoVsel = kPMC_LDO_VSEL2,//VSEL2 reset value = 0.9
    .ldoVoltage = 900,
};

/*************************************
 *system active power config
 *************************************/
pmc_domain_power_switch_t psw_active_cfg = {
    .mediaVdd2Vddn_DSR = false,//bit 6, power on MEDIA domain
    .commonVdd2_DSR = false,//!bit 7, power on COMMON_VDD2 domain, config in PMC_SENSE
    .commonVddn_DSR = false,//bit 8, power on COMMON_VDDN domain
    .dsp_PD = false,//bit 9, power on DSP and HiFi 4
    .mipiPhy_PD = false,//bit 10, power on MIPI
};

pmc_body_bias_config_t bb_active_cfg = {
    .vdd1_bbMode = kPMC_AFBB_Performance,
    .vdd2_bbMode = kPMC_AFBB_Performance,
    .vddn_bbMode = kPMC_AFBB_Performance,
    .vdd1Sram1_bbMode = kPMC_SRAM_NoBB,
    .vdd2Sram0_bbMode = kPMC_SRAM_NoBB,
};

pmc_power_config_group_1_t group1_active_cfg = {
    .tempSensor_PD = true,
    .vrefMode = kPMC_Vref_InDueCourseLowPower,
    .vdd1v8Hvd_PD = false,
    .vdd1PorMode = kPMC_HighPower,
    .vdd1LvdMode = kPMC_HighPower,
    .vdd1Hvd_PD = false,
    .vdd1Agdet_Disable = false,
    .vdd2PorMode = kPMC_HighPower,
    .vdd2LvdMode = kPMC_HighPower,
    .vdd2Hvd_PD = false,
    .vdd2Agdet_Disable = false,
    .vddnPorMode = kPMC_HighPower,
    .vddnLvdMode = kPMC_HighPower,
    .vddnHvd_PD = false,

    .otp_PD = false,//!config in PMC_COMPUTE
    .rom_PD = false,//!config in PMC_COMPUTE
};

//group2
pmc_ram_power_down_cfg_t main_ram_array_active_cfg = {
    .powerDown = kPMC_MainRamArray_AllOn,
};
//group3
pmc_ram_power_down_cfg_t main_ram_interface_active_cfg = {
    .powerDown = kPMC_MainRamArray_AllOn,
};
//group4
pmc_ram_power_down_cfg_t periph_ram_array_active_cfg = {
    .powerDown = kPMC_PeriphRam_AllOn,
};
//group5
pmc_ram_power_down_cfg_t periph_ram_interface_active_cfg = {
    .powerDown = kPMC_PeriphRam_AllOn,
};

/*************************************
 *DCDC, LDO, PMIC configs
 *************************************/
//Written value is clipped to 0x68 = max value = 1.15V. min = 500mV
//SENSE cannot control
#if defined CPU_MIMXRT798SGFOA_cm33_core0
void PMC_SetDcdcVoltage(pmc_dcdc_vsel_t num, double voltage_mV)
{
    uint32_t value = (uint32_t)((voltage_mV - 500) / 6.25);//step = 6.25mV
    assert(value < 128);
    uint32_t reg = PMC0->DCDCVSEL;
    if(num)
    {
        reg &= ~PMC_DCDCVSEL_VSEL1_MASK;
        reg |= PMC_DCDCVSEL_VSEL1(value);
    }
    else
    {
        reg &= ~PMC_DCDCVSEL_VSEL0_MASK;
        reg |= PMC_DCDCVSEL_VSEL0(value);
    }
    PMC0->DCDCVSEL = reg;

    PMC_ApplyUpdatedSetting();
}

//Written value is clipped to 0x38 = 1.15V. min = 450mV
//SENSE cannot control
void PMC_SetLdoVoltage_Vdd2(pmc_ldo_vsel_t num, double voltage_mV)
{
    uint32_t value = (uint32_t)((voltage_mV - 450) / 12.5);//step = 12.5mV
    assert(value < 64);
    uint32_t reg = PMC0->LDOVDD2VSEL;
    if(num == kPMC_LDO_VSEL0)
    {
        reg &= ~PMC_LDOVDD2VSEL_VSEL0_MASK;
        reg |= PMC_LDOVDD2VSEL_VSEL0(value);
    }
    else if(num == kPMC_LDO_VSEL1)
    {
        reg &= ~PMC_LDOVDD2VSEL_VSEL1_MASK;
        reg |= PMC_LDOVDD2VSEL_VSEL1(value);
    }
    else if(num == kPMC_LDO_VSEL2)
    {
        reg &= ~PMC_LDOVDD2VSEL_VSEL2_MASK;
        reg |= PMC_LDOVDD2VSEL_VSEL2(value);
    }
    else if(num == kPMC_LDO_VSEL3)
    {
        reg &= ~PMC_LDOVDD2VSEL_VSEL3_MASK;
        reg |= PMC_LDOVDD2VSEL_VSEL3(value);
    }
    PMC0->LDOVDD2VSEL = reg;

    PMC_ApplyUpdatedSetting();
}
#endif

//Written value is clipped to 0x38 = 1.15V. min = 450mV
void PMC_SetLdoVoltage_Vdd1(pmc_ldo_vsel_t num, double voltage_mV)
{
    uint32_t value = (uint32_t)((voltage_mV - 450) / 12.5);//step = 12.5mV
    assert(value < 64);
    uint32_t reg = PMC_POINTER->LDOVDD1VSEL;
    if(num == kPMC_LDO_VSEL0)
    {
        reg &= ~PMC_LDOVDD1VSEL_VSEL0_MASK;
        reg |= PMC_LDOVDD1VSEL_VSEL0(value);
    }
    else if(num == kPMC_LDO_VSEL1)
    {
        reg &= ~PMC_LDOVDD1VSEL_VSEL1_MASK;
        reg |= PMC_LDOVDD1VSEL_VSEL1(value);
    }
    else if(num == kPMC_LDO_VSEL2)
    {
        reg &= ~PMC_LDOVDD1VSEL_VSEL2_MASK;
        reg |= PMC_LDOVDD1VSEL_VSEL2(value);
    }
    else if(num == kPMC_LDO_VSEL3)
    {
        reg &= ~PMC_LDOVDD1VSEL_VSEL3_MASK;
        reg |= PMC_LDOVDD1VSEL_VSEL3(value);
    }
    PMC_POINTER->LDOVDD1VSEL = reg;

    PMC_ApplyUpdatedSetting();
}

void PMC_SetDcdcMode(pmc_power_mode_t dcdcMode, pmc_dcdc_vsel_t dcdcVsel)
{
    uint32_t reg = PMC_POINTER->PDRUNCFG0;
    reg &= ~(PMC_PDRUNCFG0_DCDC_LP_MASK | PMC_PDRUNCFG0_DCDC_VSEL_MASK);
    reg |=  PMC_PDRUNCFG0_DCDC_LP(dcdcMode) | PMC_PDRUNCFG0_DCDC_VSEL(dcdcVsel);
    PMC_POINTER->PDRUNCFG0 = reg;
    
    PMC_ApplyUpdatedSetting();
}

void PMC_SetLdoMode(pmc_ldo_name_t ldo, pmc_ldo_power_mode_t ldoMode, pmc_ldo_vsel_t ldoVsel)
{
    uint32_t reg = PMC_POINTER->PDRUNCFG0;
    if(ldo == kPMC_VDD1_LDO)
    {
        reg &= ~(PMC_PDRUNCFG0_LDO1_MODE_MASK | PMC_PDRUNCFG0_LDO1_VSEL_MASK);
        reg |= PMC_PDRUNCFG0_LDO1_MODE(ldoMode) | PMC_PDRUNCFG0_LDO1_VSEL(ldoVsel);
    }
    else if(ldo == kPMC_VDD2_LDO)
    {
        reg &= ~(PMC_PDRUNCFG0_LDO2_MODE_MASK | PMC_PDRUNCFG0_LDO2_VSEL_MASK);
        reg |= PMC_PDRUNCFG0_LDO2_MODE(ldoMode) | PMC_PDRUNCFG0_LDO2_VSEL(ldoVsel);
    }
    PMC_POINTER->PDRUNCFG0 = reg;

    PMC_ApplyUpdatedSetting();
}


void PMC_ConfigDcdcOutput(pmc_dcdc_config_t *config)
{
#if defined CPU_MIMXRT798SGFOA_cm33_core0
    PMC_SetDcdcVoltage(config->dcdcVsel, config->dcdcVoltage);
#endif
    PMC_SetDcdcMode(config->dcdcMode, config->dcdcVsel);
}

void PMC_ConfigLdoOutput(pmc_ldo_name_t ldo, pmc_ldo_config_t *config)
{
    if(ldo == kPMC_VDD1_LDO)
        PMC_SetLdoVoltage_Vdd1(config->ldoVsel, config->ldoVoltage);
#if defined CPU_MIMXRT798SGFOA_cm33_core0
    else if(ldo == kPMC_VDD2_LDO)
        PMC_SetLdoVoltage_Vdd2(config->ldoVsel, config->ldoVoltage);
#endif

    PMC_SetLdoMode(ldo, config->ldoMode, config->ldoVsel);
}

void PMC_SetPmicModePins(uint32_t num)
{
    uint32_t reg = PMC_POINTER->PDRUNCFG0;
    reg &= ~0x3UL;
    reg |= (num & 0x3UL);
    PMC_POINTER->PDRUNCFG0 = reg;

    PMC_ApplyUpdatedSetting();
}

/*************************************
 *BB configs
 *************************************/
void PMC_ConfigVddRailBodyBias(pmc_sys_config_t sysMode, pmc_vdd_rail_t vdd, pmc_vdd_body_bias_t bbMode)
{
    uint32_t reg;
    if(sysMode)
        reg = PMC_POINTER->PDSLEEPCFG0;
    else
        reg = PMC_POINTER->PDRUNCFG0;

    if(vdd == kPMC_Vdd1)
    {
        if(bbMode == kPMC_AFBB_Performance)
        {
            reg |= PMC_PDRUNCFG0_RBB1_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_AFBB1_PD_MASK;
        }
        else if(bbMode == kPMC_RBB_PowerSave)
        {
            reg |= PMC_PDRUNCFG0_AFBB1_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_RBB1_PD_MASK;
        }
        else if(bbMode == kPMC_NoBB)
        {
            reg |= PMC_PDRUNCFG0_AFBB1_PD_MASK | PMC_PDRUNCFG0_RBB1_PD_MASK;
        }
    }
    else if(vdd == kPMC_Vdd2)
    {
        if(bbMode == kPMC_AFBB_Performance)
        {
            reg |= PMC_PDRUNCFG0_RBB2_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_AFBB2_PD_MASK;
        }
        else if(bbMode == kPMC_RBB_PowerSave)
        {
            reg |= PMC_PDRUNCFG0_AFBB2_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_RBB2_PD_MASK;
        }
        else if(bbMode == kPMC_NoBB)
        {
            reg |= PMC_PDRUNCFG0_AFBB2_PD_MASK | PMC_PDRUNCFG0_RBB2_PD_MASK;
        }
    }
    else if(vdd == kPMC_Vddn)
    {
        if(bbMode == kPMC_AFBB_Performance)
        {
            reg |= PMC_PDRUNCFG0_RBBN_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_AFBBN_PD_MASK;
        }
        else if(bbMode == kPMC_RBB_PowerSave)
        {
            reg |= PMC_PDRUNCFG0_AFBBN_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_RBBN_PD_MASK;
        }
        else if(bbMode == kPMC_NoBB)
        {
            reg |= PMC_PDRUNCFG0_AFBBN_PD_MASK | PMC_PDRUNCFG0_RBBN_PD_MASK;
        }
    }

    if(sysMode)
        PMC_POINTER->PDSLEEPCFG0 = reg;
    else
        PMC_POINTER->PDRUNCFG0 = reg;

    PMC_ApplyUpdatedSetting();
}

void PMC_ConfigSramArbiterBodyBias(pmc_sys_config_t sysMode, pmc_sram_arbiter_t vdd, pmc_vdd_sram_body_bias_t bbMode)
{
    uint32_t reg;
    if(sysMode)
        reg = PMC_POINTER->PDSLEEPCFG0;
    else
        reg = PMC_POINTER->PDRUNCFG0;

    if(vdd == kPMC_SramArbiter1_VDD1)
    {
        if(bbMode == kPMC_SRAM_AFBB_Performance)
        {
            reg |= PMC_PDRUNCFG0_RBBSR1_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_AFBBSR1_PD_MASK;
        }
        else if(bbMode == kPMC_SRAM_RBB_PowerSave)
        {
            reg |= PMC_PDRUNCFG0_AFBBSR1_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_RBBSR1_PD_MASK;
        }
        else if(bbMode == kPMC_SRAM_NoBB)
        {
            reg |= PMC_PDRUNCFG0_AFBBSR1_PD_MASK | PMC_PDRUNCFG0_RBBSR1_PD_MASK;
        }
    }
    else if(vdd == kPMC_SramArbiter0_VDD2)
    {
        if(bbMode == kPMC_SRAM_AFBB_Performance)
        {
            reg |= PMC_PDRUNCFG0_RBBSR2_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_AFBBSR2_PD_MASK;
        }
        else if(bbMode == kPMC_SRAM_RBB_PowerSave)
        {
            reg |= PMC_PDRUNCFG0_AFBBSR2_PD_MASK;
            reg &= ~PMC_PDRUNCFG0_RBBSR2_PD_MASK;
        }
        else if(bbMode == kPMC_SRAM_NoBB)
        {
            reg |= PMC_PDRUNCFG0_AFBBSR2_PD_MASK | PMC_PDRUNCFG0_RBBSR2_PD_MASK;
        }
    }

    if(sysMode)
        PMC_POINTER->PDSLEEPCFG0 = reg;
    else
        PMC_POINTER->PDRUNCFG0 = reg;

    PMC_ApplyUpdatedSetting();
}

void PMC_ConfigBodyBias(pmc_sys_config_t sysMode, pmc_body_bias_config_t *config)
{
    PMC_ConfigVddRailBodyBias(sysMode, kPMC_Vdd1, config->vdd1_bbMode);
    PMC_ConfigVddRailBodyBias(sysMode, kPMC_Vdd2, config->vdd2_bbMode);
    PMC_ConfigVddRailBodyBias(sysMode, kPMC_Vddn, config->vddn_bbMode);
    PMC_ConfigSramArbiterBodyBias(sysMode, kPMC_SramArbiter1_VDD1, config->vdd1Sram1_bbMode);
    PMC_ConfigSramArbiterBodyBias(sysMode, kPMC_SramArbiter0_VDD2, config->vdd2Sram0_bbMode);
}

/*************************************
 *PD*CFG configs
 *************************************/
void PMC_Run_ConfigDomainPowerSwitch(pmc_domain_power_switch_t *config)
{
    uint32_t reg = PMC_POINTER->PDRUNCFG0;
    reg &= ~0x7C0UL;//!clear bit 6-10
//#if defined CPU_MIMXRT798SGFOA_cm33_core1
////    reg = 0xA20UL;//PMC_SENSE reserved bits
//    reg |= config->commonVdd2_DSR ? PMC_PDRUNCFG0_V2COM_DSR(1U) : PMC_PDRUNCFG0_V2COM_DSR(0U);//bit 7
//#el
#if defined CPU_MIMXRT798SGFOA_cm33_core0
//    reg = 0x800UL;//PMC_COMPT reserved bits
    reg |= config->dsp_PD ? PMC_PDRUNCFG0_V2DSP_PD(1U) : PMC_PDRUNCFG0_V2DSP_PD(0U);//bit 9
#endif
    reg |= config->mediaVdd2Vddn_DSR ? PMC_PDRUNCFG0_V2NMED_DSR(1U) : PMC_PDRUNCFG0_V2NMED_DSR(0U);//bit 6
    reg |= config->commonVddn_DSR ? PMC_PDRUNCFG0_VNCOM_DSR(1U) : PMC_PDRUNCFG0_VNCOM_DSR(0U);//bit 8
    reg |= config->mipiPhy_PD ? PMC_PDRUNCFG0_V2MIPI_PD(1U) : PMC_PDRUNCFG0_V2MIPI_PD(0U);//bit 10
    PMC_POINTER->PDRUNCFG0 = reg;

    PMC_ApplyUpdatedSetting();
}

void PMC_LP_ConfigDomainPowerSwitch(pmc_domain_power_switch_t *config)
{
    uint32_t reg = PMC_POINTER->PDSLEEPCFG0;
    reg &= ~0x7FCUL;//!clear bit 2-10
#if defined CPU_MIMXRT798SGFOA_cm33_core1
//    reg = 0xA20UL;//PMC_SENSE reserved bits
#elif defined CPU_MIMXRT798SGFOA_cm33_core0
//    reg = 0x800UL;//PMC_COMPT reserved bits
    reg |= config->compute_DSR ? PMC_PDSLEEPCFG0_V2COMP_DSR(1U) : PMC_PDSLEEPCFG0_V2COMP_DSR(0U);//bit 5
    reg |= config->dsp_PD ? PMC_PDSLEEPCFG0_V2DSP_PD(1U) : PMC_PDSLEEPCFG0_V2DSP_PD(0U);//bit 9
#endif
    reg |= config->full_DSR ? PMC_PDSLEEPCFG0_FDSR(1U) : PMC_PDSLEEPCFG0_FDSR(0U);//bit 2
    reg |= config->DPD ? PMC_PDSLEEPCFG0_DPD(1U) : PMC_PDSLEEPCFG0_DPD(0U);//bit 3
    reg |= config->full_DPD ? PMC_PDSLEEPCFG0_FDPD(1U) : PMC_PDSLEEPCFG0_FDPD(0U);//bit 4
    reg |= config->mediaVdd2Vddn_DSR ? PMC_PDSLEEPCFG0_V2NMED_DSR(1U) : PMC_PDSLEEPCFG0_V2NMED_DSR(0U);//bit 6
    reg |= config->commonVdd2_DSR ? PMC_PDSLEEPCFG0_V2COM_DSR(1U) : PMC_PDSLEEPCFG0_V2COM_DSR(0U);//bit 7
    reg |= config->commonVddn_DSR ? PMC_PDSLEEPCFG0_VNCOM_DSR(1U) : PMC_PDSLEEPCFG0_VNCOM_DSR(0U);//bit 8
    reg |= config->mipiPhy_PD ? PMC_PDSLEEPCFG0_V2MIPI_PD(1U) : PMC_PDSLEEPCFG0_V2MIPI_PD(0U);//bit 10
    PMC_POINTER->PDSLEEPCFG0 = reg;
}

void PMC_ConfigGroup1(pmc_sys_config_t sysMode, pmc_power_config_group_1_t *config)
{
    assert(config != NULL);
    uint32_t reg;

#if defined CPU_MIMXRT798SGFOA_cm33_core1
    reg = 0x7FFFC000;//PMC_SENSE reserved bits
#elif defined CPU_MIMXRT798SGFOA_cm33_core0
    reg = 0x7FFE4000;//PMC_COMPT reserved bits
    reg |= config->otp_PD ? PMC_PDRUNCFG1_OTP_PD(1U) : PMC_PDRUNCFG1_OTP_PD(0U);
    reg |= config->rom_PD ? PMC_PDRUNCFG1_ROM_PD(1U) : PMC_PDRUNCFG1_ROM_PD(0U);
#endif
    //PDRUNCFG1 and PDSLEEPCFG1 have same reserved bits

    reg |= config->tempSensor_PD ? PMC_PDRUNCFG1_TEMP_PD(1U) : PMC_PDRUNCFG1_TEMP_PD(0U);
    reg |= PMC_PDRUNCFG1_PMCREF_LP(config->vrefMode);
    reg |= config->vdd1v8Hvd_PD ? PMC_PDRUNCFG1_HVD1V8_PD(1U) : PMC_PDRUNCFG1_HVD1V8_PD(0U);
    reg |= PMC_PDRUNCFG1_POR1_LP(config->vdd1PorMode);
    reg |= PMC_PDRUNCFG1_LVD1_LP(config->vdd1LvdMode);
    reg |= config->vdd1Hvd_PD ? PMC_PDRUNCFG1_HVD1_PD(1U) : PMC_PDRUNCFG1_HVD1_PD(0U);
    reg |= config->vdd1Agdet_Disable ? PMC_PDRUNCFG1_AGDET1_PD(1U) : PMC_PDRUNCFG1_AGDET1_PD(0U);
    reg |= PMC_PDRUNCFG1_POR2_LP(config->vdd2PorMode);
    reg |= PMC_PDRUNCFG1_LVD2_LP(config->vdd2LvdMode);
    reg |= config->vdd2Hvd_PD ? PMC_PDRUNCFG1_HVD2_PD(1U) : PMC_PDRUNCFG1_HVD2_PD(0U);
    reg |= config->vdd2Agdet_Disable ? PMC_PDRUNCFG1_AGDET2_PD(1U) : PMC_PDRUNCFG1_AGDET2_PD(0U);
    reg |= PMC_PDRUNCFG1_PORN_LP(config->vddnPorMode);
    reg |= PMC_PDRUNCFG1_LVDN_LP(config->vddnLvdMode);
    reg |= config->vddnHvd_PD ? PMC_PDRUNCFG1_HVDN_PD(1U) : PMC_PDRUNCFG1_HVDN_PD(0U);

    if(sysMode)
        PMC_POINTER->PDSLEEPCFG1 = reg;
    else
    {
        PMC_POINTER->PDRUNCFG1 = reg;
        PMC_ApplyUpdatedSetting();
    }
}

/*************************************
 *group 2 and 3 config together
 *************************************/
void PMC_MainRamArrayPowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config)
{
    if(sysMode)
        PMC_POINTER->PDSLEEPCFG2 = config->powerDown | 0xC0000000/*reset value*/;
    else
    {
        // #if defined CPU_MIMXRT798SGFOA_cm33_core0
        //     assert((config->powerDown & kPMC_MainRamArray0) == 0);
        // #endif
        PMC_POINTER->PDRUNCFG2 = config->powerDown | 0xC0000000/*reset value*/;
        PMC_ApplyUpdatedSetting();
    }
}

void PMC_MainRamInterfacePowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config)
{
    if(sysMode)
        PMC_POINTER->PDSLEEPCFG3 = config->powerDown | 0xC0000000/*reset value*/;
    else
    {
        // #if defined CPU_MIMXRT798SGFOA_cm33_core0
        //     assert((config->powerDown & kPMC_MainRamArray0) == 0);
        // #endif
        PMC_POINTER->PDRUNCFG3 = config->powerDown | 0xC0000000/*reset value*/;
        PMC_ApplyUpdatedSetting();
    }
}

/*************************************
 *group 2 and 3 config separately
 *************************************/
void PMC_MainRamArrayPowerControl_Separate(pmc_sys_config_t sysMode, pmc_main_ram_power_down_t array, pmc_power_ctrl_t enable)
{
    if(sysMode)
    {
        if(enable)
            PMC_POINTER->PDSLEEPCFG2 |= array;
        else
            PMC_POINTER->PDSLEEPCFG2 &= ~array;
    }
    else
    {
        #if defined CPU_MIMXRT798SGFOA_cm33_core0
            assert(array != kPMC_MainRamArray0);//compute cannot power down sram 0 in PDRUNCFG2 & 3
        #endif
        if(enable)
            PMC_POINTER->PDRUNCFG2 |= array;
        else
            PMC_POINTER->PDRUNCFG2 &= ~array;

        PMC_ApplyUpdatedSetting();
    }
}

void PMC_MainRamInterfacePowerControl_Separate(pmc_sys_config_t sysMode, pmc_main_ram_power_down_t array, pmc_power_ctrl_t enable)
{
    if(sysMode)
    {
        if(enable)
            PMC_POINTER->PDSLEEPCFG3 |= array;
        else
            PMC_POINTER->PDSLEEPCFG3 &= ~array;
    }
    else
    {
        #if defined CPU_MIMXRT798SGFOA_cm33_core0
            assert(array != kPMC_MainRamArray0);//compute cannot power down sram 0 in PDRUNCFG2 & 3
        #endif
        if(enable)
            PMC_POINTER->PDRUNCFG3 |= array;
        else
            PMC_POINTER->PDRUNCFG3 &= ~array;

        PMC_ApplyUpdatedSetting();
    }
}

/*************************************
 *group 4 and 5 config together
 *************************************/
void PMC_PeriphRamArrayPowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config)
{
    if(sysMode)
        PMC_POINTER->PDSLEEPCFG4 = config->powerDown | 0xFF800000/*reset value*/;
    else
    {
        PMC_POINTER->PDRUNCFG4 = config->powerDown | 0xFF800000/*reset value*/;
        PMC_ApplyUpdatedSetting();
    }
}

void PMC_PeriphRamInterfacePowerControl_Merge(pmc_sys_config_t sysMode, pmc_ram_power_down_cfg_t *config)
{
    if(sysMode)
        PMC_POINTER->PDSLEEPCFG5 = config->powerDown | 0xFF800000/*reset value*/;
    else
    {
        PMC_POINTER->PDRUNCFG5 = config->powerDown | 0xFF800000/*reset value*/;
        PMC_ApplyUpdatedSetting();
    }
}

/*************************************
 *group 4 and 5 config separately
 *************************************/
void PMC_PeriphRamArrayPowerControl_Separate(pmc_sys_config_t sysMode, pmc_periph_ram_power_down_t name, pmc_power_ctrl_t enable)
{
    if(sysMode)
    {
        if(enable)
            PMC_POINTER->PDSLEEPCFG4 |= name;
        else
            PMC_POINTER->PDSLEEPCFG4 &= ~name;
    }
    else
    {
        if(enable)
            PMC_POINTER->PDRUNCFG4 |= name;
        else
            PMC_POINTER->PDRUNCFG4 &= ~name;

        PMC_ApplyUpdatedSetting();
    }
}

void PMC_PeriphRamInterfacePowerControl_Separate(pmc_sys_config_t sysMode, pmc_periph_ram_power_down_t name, pmc_power_ctrl_t enable)
{
    if(sysMode)
    {
        if(enable)
            PMC_POINTER->PDSLEEPCFG5 |= name;
        else
            PMC_POINTER->PDSLEEPCFG5 &= ~name;
    }
    else
    {
        if(enable)
            PMC_POINTER->PDRUNCFG5 |= name;
        else
            PMC_POINTER->PDRUNCFG5 &= ~name;

        PMC_ApplyUpdatedSetting();
    }
}


/*************************************
 *environment
 *************************************/
void ENV_PMC_ConfigActivePowerSupply(void)
{
    PMC_ConfigDcdcOutput(&pmc_dcdc_active_cfg);
    PMC_ConfigLdoOutput(kPMC_VDD1_LDO, &pmc_ldo1_active_cfg);
    PMC_ConfigLdoOutput(kPMC_VDD2_LDO, &pmc_ldo2_active_cfg);
    PMC_SetPmicModePins(0);//default config
}

void ENV_PMC_ConfigDomainAndMemoryPower(void)
{
    PMC_Run_ConfigDomainPowerSwitch(&psw_active_cfg);
    PMC_ConfigGroup1(kPMC_ActiveConfig, &group1_active_cfg);
//    SLEEPCON_GateClockAndPower_Merge(kPMC_ActiveConfig, &sleepcon_cg_pg_active_cfg);

    PMC_ConfigBodyBias(kPMC_ActiveConfig, &bb_active_cfg);
    PMC_MainRamArrayPowerControl_Merge(kPMC_ActiveConfig, &main_ram_array_active_cfg);
    PMC_MainRamInterfacePowerControl_Merge(kPMC_ActiveConfig, &main_ram_interface_active_cfg);
    PMC_PeriphRamArrayPowerControl_Merge(kPMC_ActiveConfig, &periph_ram_array_active_cfg);
    PMC_PeriphRamInterfacePowerControl_Merge(kPMC_ActiveConfig, &periph_ram_interface_active_cfg);
}

/*************************************
 *for standalone
 *************************************/
void PMC_ReleasePower(void)
{
#if defined CPU_MIMXRT798SGFOA_cm33_core1
    PMC1->PDRUNCFG0 &= ~0x5C0UL;
    PMC1->PDRUNCFG2 = 0xC0000000;
    PMC1->PDRUNCFG3 = 0xC0000000;
    PMC1->PDRUNCFG4 = 0xFF800000;
    PMC1->PDRUNCFG5 = 0xFF800000;
    PMC1->CTRL |= PMC_CTRL_APPLYCFG_MASK;
    while(PMC1->CTRL & PMC_CTRL_APPLYCFG_MASK);
    while(PMC1->STATUS & 0x1UL);

    SLEEPCON1->RUNCFG = 0;
#elif defined CPU_MIMXRT798SGFOA_cm33_core0
    PMC0->PDRUNCFG0 &= ~0x740UL;
    PMC0->PDRUNCFG2 = 0xC0000000;
    PMC0->PDRUNCFG3 = 0xC0000000;
    PMC0->PDRUNCFG4 = 0xFF800000;
    PMC0->PDRUNCFG5 = 0xFF800000;
    PMC0->CTRL |= PMC_CTRL_APPLYCFG_MASK;
    while(PMC0->CTRL & PMC_CTRL_APPLYCFG_MASK);
    while(PMC0->STATUS & 0x1UL);

    SLEEPCON0->RUNCFG = 0;
#endif
}
