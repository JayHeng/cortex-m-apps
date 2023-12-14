/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_xspi.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xspi"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FREQ_1MHz             (1000000UL)
#define XSPI_LUT_KEY_VAL   (0x5AF05AF0UL)

#define XSPI_TX_WATERMARK_DEFAULT            (0x8U)
#define XSPI_RX_WATERMARK_DEFAULT            (0x8U)

#define XSPI_HIGH_SAMPLE_FREQUENCY    (13000000UL)

/*! @brief Common sets of flags used by the driver, _xspi_flag_constants. */
enum kXSPI_Flags
{
    /*! IRQ sources enabled by the non-blocking transactional API. */
    kIrqFlags = kXSPI_TxBufferFillFlag | kXSPI_TxBufferUnderrunFlag | kXSPI_RxBufferOverflowFlag |
                kXSPI_RxBufferDrainFlag | kXSPI_IllegalInstructionErrorFlag |
                kXSPI_IpCmdtriggerErrorFlag,

    /*! IP CMD Errors to check for. */
    kIpcmdErrorFlags = kXSPI_IllegalInstructionErrorFlag | kXSPI_IpCmdtriggerErrorFlag,

    kFlashProtectionErrorFlags = kXSPI_FradMatchErrorFlag | kXSPI_FradnAccErrorFlag |
                kXSPI_IpsErrorFlag | kXSPI_Tg0SfarErrorFlag |
                kXSPI_Tg1SfarErrorFlag | kXSPI_TgnIpcrErrorFlag,
};

/* XSPI FSM status. */
enum
{
    kXSPI_TransactionIsGrantedXSPIsBusy = 0x0U,    /*!< Transaction is granted, but XSPI is busy
                                                     with any previous DMA transaction is ongoing*/
    kXSPI_TbdrLockIsOpen = 0x1U,                   /*!< TBDR lock is open. IPS master can write in TBDR.*/
    kXSPI_WriteTransferIsTriggered = 0x2U,           /*!< Write transfer is triggered. SEQID is written to XSPI.*/
    kXSPI_ReadTransferIsTriggered = 0x3U,           /*!< Read transfer is triggered. SEQID is written to XSPI.*/
};

/* XSPI transfer state, _xspi_transfer_state. */
enum
{
    kXSPI_Idle      = 0x0U, /*!< Transfer is done. */
    kXSPI_BusyWrite = 0x1U, /*!< XSPI is busy write transfer. */
    kXSPI_BusyRead  = 0x2U, /*!< XSPI is busy write transfer. */
};


/*! @brief Typedef for interrupt handler. */
typedef void (*xspi_isr_t)(XSPI_Type *base, xspi_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void XSPI_Memset(void *src, uint8_t value, size_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to xspi bases for each instance. */
static XSPI_Type *const s_xspiBases[] = XSPI_BASE_PTRS;

/*! @brief Pointers to xspi IRQ number for each instance. */
static const IRQn_Type s_xspiIrqs[] = XSPI_IRQS;

/*! @brief Pointers to xspi amba base for each instance. */
static uint32_t s_xspiAmbaBase[] = XSPI_AMBA_BASES;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Clock name array */
static const clock_ip_name_t s_xspiClock[] = XSPI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*! @brief Pointers to XSPI resets for each instance. */
static const reset_ip_name_t s_xspiResets[] = XSPI_RSTS;
/*******************************************************************************
 * Code
 ******************************************************************************/
/* To avoid compiler opitimizing this API into memset() in library. */
#if defined(__ICCARM__)
#pragma optimize = none
#endif /* defined(__ICCARM__) */

static void XSPI_Memset(void *src, uint8_t value, size_t length)
{
    assert(src != NULL);
    uint8_t *p = src;

    for (uint32_t i = 0U; i < length; i++)
    {
        *p = value;
        p++;
    }
}

void XSPI_IPS_CMD_WaitFinish(XSPI_Type *base)
{
    while ((base->SR & XSPI_SR_IP_ACC_MASK) != 0U)
    {
    }

    while ((base->FR & XSPI_FR_TFF_MASK) == 0U)
    {
    }
}

uint32_t XSPI_GetInstance(XSPI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_xspiBases); instance++)
    {
        if (s_xspiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_xspiBases));

    return instance;
}

status_t XSPI_CheckAndClearError(XSPI_Type *base, uint32_t status)
{
    status_t result = kStatus_Success;

    /* Check for error. */
    status &= (uint32_t)kFlashProtectionErrorFlags;
    if (0U != status)
    {
        /* Select the correct error code.. */
        if (0U != (status & (uint32_t)kXSPI_SequenceExecutionTimeoutFlag))
        {
            result = kStatus_XSPI_SequenceExecutionTimeout;
            /* Clear the flags. */
            base->ERRSTAT |= kXSPI_SequenceExecutionTimeoutFlag;
        }
        else if (0U != ((status & (uint32_t)kXSPI_FradMatchErrorFlag) | (status & (uint32_t)kXSPI_FradnAccErrorFlag)))
        {
            result = kStatus_XSPI_FradCheckError;
            /* Clear the flags. */
            if (0U != (status & (uint32_t)kXSPI_FradMatchErrorFlag))
            {
                base->ERRSTAT |= kXSPI_FradMatchErrorFlag;
            }
            else
            {
                base->ERRSTAT |= kXSPI_FradnAccErrorFlag;
            }
        }
        else if (0U != (status & (uint32_t)kXSPI_IpsErrorFlag))
        {
            result = kStatus_XSPI_IpsBusTransError;
            /* Clear the flags. */
            base->IPSERROR |= XSPI_IPSERROR_CLR_MASK;
        }
        else if (0U != ((status & (uint32_t)kXSPI_Tg0SfarErrorFlag) | (status & (uint32_t)kXSPI_Tg1SfarErrorFlag)
                        | (status & (uint32_t)kXSPI_TgnIpcrErrorFlag)))
        {
            result = kStatus_XSPI_TgQueueWritingError;
            /* Clear the flags. */
            if (0U != ((status & (uint32_t)kXSPI_Tg0SfarErrorFlag) | (status & (uint32_t)kXSPI_Tg1SfarErrorFlag)))
            {
                base->TGSFARS |= XSPI_TGSFARS_CLR_MASK;
            }
            else
            {
                base->TGIPCRS |= XSPI_TGIPCRS_CLR_MASK;
            }
        }
        else
        {
            assert(false);
        }

        /* Reset fifos. These flags clear automatically. */
        base->MCR |= XSPI_MCR_CLR_RXF_MASK;
        base->MCR |= XSPI_MCR_CLR_TXF_MASK;
    }

    return result;
}


/*!
 * brief Initializes the XSPI module and internal state.
 *
 * This function  configures the XSPI with the
 * input configure parameters. Users should call this function before any XSPI operations.
 *
 * param base XSPI peripheral base address.
 * param config XSPI configure structure.
 */

void XSPI_Init(XSPI_Type *base, const xspi_config_t *config)
{
    uint32_t configValue = 0;
    uint32_t instance = XSPI_GetInstance(base);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the xspi clock */
    (void)CLOCK_EnableClock(s_xspiClock[XSPI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    /* Reset the XSPI module */
    RESET_PeripheralReset(s_xspiResets[XSPI_GetInstance(base)]);

    /*XSPI configuratio items*/
    configValue = XSPI_MCR_DOZE(config->enableDoze) |
                  XSPI_MCR_DQS_EN(config->enableDqs) | XSPI_MCR_DQS_OUT_EN(config->enableDqsOut) |
                  XSPI_MCR_DQS_FA_SEL(config->rxSampleClock) | XSPI_MCR_DQS_LAT_EN(config->enableDqsLatency) |
                  XSPI_MCR_DLPEN(config->enableDataLearn) | XSPI_MCR_VAR_LAT_EN(config->enableVariableLat) |
                  XSPI_MCR_CKN_FA_EN(config->enableCknPad) | XSPI_MCR_END_CFG(config->byteOrder);
    base->MCR = configValue;

    /*XSPI IP SFP configurations*/
    base->MGC = XSPI_MGC_GVLD(config->enableGloAccCtr) | XSPI_MGC_GVLDMDAD(config->mdadConfig.enableMdad) |
                XSPI_MGC_GVLDFRAD(config->fradConfig.enableFrad);

    /*XSPI IP SFP TG0MDAD TG1MDAD configurations*/
    if(config->enableGloAccCtr & config->mdadConfig.enableMdad)
    {
    configValue = XSPI_TG0MDAD_MIDMATCH(config->mdadConfig.tgmdad[0].masterIdReference)  |
                  XSPI_TG0MDAD_VLD(config->mdadConfig.tgmdad[0].assignIsValid) |
                  XSPI_TG0MDAD_SA(config->mdadConfig.tgmdad[0].secureAttribute) |
                  XSPI_TG0MDAD_MASKTYPE(config->mdadConfig.tgmdad[0].maskType) |
                  XSPI_TG0MDAD_MASK(config->mdadConfig.tgmdad[0].mask) |
                  XSPI_TG0MDAD_LCK(config->mdadConfig.tgmdad[0].enableDescriptorLock);
    base->TG0MDAD = configValue;

    configValue = XSPI_TG1MDAD_MIDMATCH(config->mdadConfig.tgmdad[1].masterIdReference)  |
                  XSPI_TG1MDAD_VLD(config->mdadConfig.tgmdad[1].assignIsValid) |
                  XSPI_TG1MDAD_SA(config->mdadConfig.tgmdad[1].secureAttribute) |
                  XSPI_TG1MDAD_MASKTYPE(config->mdadConfig.tgmdad[1].maskType) |
                  XSPI_TG1MDAD_MASK(config->mdadConfig.tgmdad[1].mask) |
                  XSPI_TG1MDAD_LCK(config->mdadConfig.tgmdad[1].enableDescriptorLock);
    base->TG1MDAD = configValue;
    }

    /*XSPI IP FRAD configurations*/
    if(config->enableGloAccCtr & config->fradConfig.enableFrad)
    {
    /* Configure start/end address, access control */
    base->FRAD0_WORD0 = config->fradConfig.frad[0].startAddress;
    base->FRAD0_WORD1 = config->fradConfig.frad[0].endAddress;
    /* R/W enable for all Secure mode */
    base->FRAD0_WORD2 = XSPI_FRAD0_WORD2_MD0ACP(config->fradConfig.frad[0].tg0MasterAccess) |
                        XSPI_FRAD0_WORD2_MD1ACP(config->fradConfig.frad[0].tg1MasterAccess);
    base->FRAD0_WORD3 = XSPI_FRAD0_WORD3_VLD(config->fradConfig.frad[0].assignIsValid);
    }

    /* XSPI Configure AHB rx buffer0. */
    configValue = base->BUF0CR;
    configValue &= ~(XSPI_BUF0CR_HP_EN_MASK | XSPI_BUF0CR_SUB_DIV_EN_MASK |
                         XSPI_BUF0CR_MSTRID_MASK | XSPI_BUF0CR_ADATSZ_MASK);
    configValue |= XSPI_BUF0CR_HP_EN(config->ahbConfig.buffer[0].enaPri.enablePriority) |
                   XSPI_BUF0CR_SUB_DIV_EN(config->ahbConfig.buffer[0].enableSubdivision) |
                   XSPI_BUF0CR_ADATSZ(config->ahbConfig.buffer[0].transferSize) |
                   XSPI_BUF0CR_MSTRID(config->ahbConfig.buffer[0].masterIndex);
    /*config subbuffer division*/
    if(config->ahbConfig.buffer[0].enableSubdivision)
    {
    configValue |= XSPI_BUF0CR_SUBBUF0_DIV(config->ahbConfig.buffer[0].subBuffer[0]) |
                   XSPI_BUF0CR_SUBBUF1_DIV(config->ahbConfig.buffer[0].subBuffer[1]) |
                   XSPI_BUF0CR_SUBBUF2_DIV(config->ahbConfig.buffer[0].subBuffer[2]);
    }
    base->BUF0CR = configValue;

    configValue = base->BUFIND[0];
    configValue &= ~XSPI_BUFIND_TPINDX_MASK;
    configValue |= XSPI_BUFIND_TPINDX(config->ahbConfig.buffer[0].topIndex);
    base->BUFIND[0] = configValue;

    /* XSPI Configure AHB rx buffer1. */
    configValue = base->BUF1CR;
    configValue &= ~(XSPI_BUF1CR_HP_EN_MASK | XSPI_BUF1CR_SUB_DIV_EN_MASK |
                         XSPI_BUF1CR_MSTRID_MASK | XSPI_BUF1CR_ADATSZ_MASK);
    configValue |= XSPI_BUF1CR_HP_EN(config->ahbConfig.buffer[1].enaPri.enablePriority) |
                   XSPI_BUF1CR_SUB_DIV_EN(config->ahbConfig.buffer[1].enableSubdivision) |
                   XSPI_BUF1CR_ADATSZ(config->ahbConfig.buffer[1].transferSize) |
                   XSPI_BUF1CR_MSTRID(config->ahbConfig.buffer[1].masterIndex);
    /*config subbuffer1 division*/
    if(config->ahbConfig.buffer[1].enableSubdivision)
    {
    configValue |= XSPI_BUF0CR_SUBBUF0_DIV(config->ahbConfig.buffer[1].subBuffer[0]) |
                   XSPI_BUF0CR_SUBBUF1_DIV(config->ahbConfig.buffer[1].subBuffer[1]) |
                   XSPI_BUF0CR_SUBBUF2_DIV(config->ahbConfig.buffer[1].subBuffer[2]);
    }
    base->BUF1CR = configValue;

    configValue = base->BUFIND[1];
    configValue &= ~XSPI_BUFIND_TPINDX_MASK;
    configValue |= XSPI_BUFIND_TPINDX(config->ahbConfig.buffer[1].topIndex);
    base->BUFIND[1] = configValue;

    /* XSPI Configure AHB rx buffer2. */
    configValue = base->BUF2CR;
    configValue &= ~(XSPI_BUF2CR_SUB_DIV_EN_MASK |
                         XSPI_BUF2CR_MSTRID_MASK | XSPI_BUF2CR_ADATSZ_MASK);
    configValue |= XSPI_BUF2CR_SUB_DIV_EN(config->ahbConfig.buffer[2].enableSubdivision) |
                   XSPI_BUF2CR_ADATSZ(config->ahbConfig.buffer[2].transferSize) |
                   XSPI_BUF2CR_MSTRID(config->ahbConfig.buffer[2].masterIndex);
    /*config subbuffer2 division*/
    if(config->ahbConfig.buffer[2].enableSubdivision)
    {
    configValue |= XSPI_BUF0CR_SUBBUF0_DIV(config->ahbConfig.buffer[2].subBuffer[0]) |
                   XSPI_BUF0CR_SUBBUF1_DIV(config->ahbConfig.buffer[2].subBuffer[1]) |
                   XSPI_BUF0CR_SUBBUF2_DIV(config->ahbConfig.buffer[2].subBuffer[2]);
    }
    base->BUF2CR = configValue;

    configValue = base->BUFIND[2];
    configValue &= ~XSPI_BUFIND_TPINDX_MASK;
    configValue |= XSPI_BUFIND_TPINDX(config->ahbConfig.buffer[2].topIndex);
    base->BUFIND[2] = configValue;


    /* XSPI Configure AHB rx buffer3. */
    configValue = base->BUF3CR;
    configValue &= ~(XSPI_BUF3CR_ALLMST_MASK | XSPI_BUF3CR_SUB_DIV_EN_MASK |
                         XSPI_BUF3CR_MSTRID_MASK | XSPI_BUF3CR_ADATSZ_MASK);
    configValue |= XSPI_BUF3CR_ALLMST(config->ahbConfig.buffer[3].enaPri.enableAllMaster) |
                   XSPI_BUF3CR_SUB_DIV_EN(config->ahbConfig.buffer[3].enableSubdivision) |
                   XSPI_BUF3CR_ADATSZ(config->ahbConfig.buffer[3].transferSize) |
                   XSPI_BUF3CR_MSTRID(config->ahbConfig.buffer[3].masterIndex);
    /*config subbuffer3 division*/
    if(config->ahbConfig.buffer[3].enableSubdivision)
    {
    configValue |= XSPI_BUF0CR_SUBBUF0_DIV(config->ahbConfig.buffer[3].subBuffer[0]) |
                   XSPI_BUF0CR_SUBBUF1_DIV(config->ahbConfig.buffer[3].subBuffer[1]) |
                   XSPI_BUF0CR_SUBBUF2_DIV(config->ahbConfig.buffer[3].subBuffer[2]);
    }
    base->BUF3CR = configValue;

    /*AHB prefech*/
    configValue = base->SPTRCLR;
    configValue |= XSPI_SPTRCLR_PREFETCH_DIS(!(config->ahbConfig.enableAHBPrefetch));
    base->SPTRCLR = configValue;

    /*Page Program Wait Write enable*/
    configValue = base->AWRCR;
    configValue |= XSPI_AWRCR_PPW_WR_DIS(!(config->ahbConfig.enableAHBWaitWrite));
    base->AWRCR = configValue;

    /*Page Program Wait read enable*/
    configValue = base->AWRCR;
    configValue |= XSPI_AWRCR_PPW_RD_DIS(!(config->ahbConfig.enableAHBWaitRead));
    base->AWRCR = configValue;

    /*XSPI ahb alignment  slite config*/
    configValue = base->BFGENCR;
    configValue |= XSPI_BFGENCR_ALIGN(config->ahbConfig.ahbAlignment) |
            XSPI_BFGENCR_SPLITEN(config->ahbConfig.enableAHBSlite);
    if (config->ahbConfig.enableAHBSlite)
    {
        if (instance == 0UL || instance == 1UL)
        {
            /*XSPI ahb Alignment config only support XSPI0 XSPI1*/
            configValue |= XSPI_BFGENCR_AHBSSIZE(config->ahbConfig.ahbSplitSize);
        }
    }
    base->BFGENCR |= configValue;

    /* XSPI Configure IP Fifo watermarks. */
    base->RBCT &= ~XSPI_RBCT_WMRK_MASK;
    base->RBCT |= XSPI_RBCT_WMRK((uint32_t)config->rxWatermark / 4U - 1U);
    base->TBCT &= ~XSPI_TBCT_WMRK_MASK;
    base->TBCT |= XSPI_TBCT_WMRK((uint32_t)config->txWatermark / 4U);

    /* Reset flash size on all ports */

}

/*!
 * brief Gets default settings for XSPI.
 *
 * param base XSPI peripheral base address.
 * param config XSPI configuration structure.
 * param devconfig Flash configuration parameters.

 */
void XSPI_GetDefaultConfig(XSPI_Type *base, xspi_config_t *config, xspi_device_config_t *devconfig)
{
    uint32_t instance = XSPI_GetInstance(base);

    /* Initializes the configure structure to zero. */
    XSPI_Memset(config, 0, sizeof(*config));

    config->rxSampleClock          = kXSPI_ReadSampleClkExternalInputFromDqsPad;
    config->byteOrder              = kXSPI_64BitLE;
    config->enableDoze             = true;
    config->enableDqs              = true;
    config->enableDqsOut           = false; /*only for psram*/
    config->enableDqsLatency       = false;
    config->enableDataLearn        = false; /* only for sample clock source set to LoopbackFromDqsPad */
    config->enableVariableLat      = false;
    config->enableCknPad           = false;

    config->txWatermark            = XSPI_TX_WATERMARK_DEFAULT;
    config->rxWatermark            = XSPI_RX_WATERMARK_DEFAULT;

    /*XSPI IP SFP configurations*/
    config->enableGloAccCtr        = true;
    config->mdadConfig.enableMdad  = true;
    config->fradConfig.enableFrad  = true;
    /*Mdad Config*/
    XSPI_Memset(config->mdadConfig.tgmdad, 0, sizeof(config->mdadConfig.tgmdad));

    config->mdadConfig.tgmdad[0].assignIsValid     = true;
    config->mdadConfig.tgmdad[0].secureAttribute   = kXSPI_AttributeMasterNonsecureSecureBoth;

    config->mdadConfig.tgmdad[1].assignIsValid     = true;
    config->mdadConfig.tgmdad[1].secureAttribute   = kXSPI_AttributeMasterNonsecureSecureBoth;

    /*Frad Config*/
    XSPI_Memset(config->fradConfig.frad, 0, sizeof(config->fradConfig.frad));
    config->fradConfig.frad[0].startAddress  = s_xspiAmbaBase[instance];

    /*flashBase + XSPI_MEMORY_SIZE*/
    config->fradConfig.frad[0].endAddress = (uint32_t)(s_xspiAmbaBase[instance]) +
            (uint32_t) ((devconfig->flashA1Size) << 10);
    config->fradConfig.frad[0].tg0MasterAccess = 0x6U;
    config->fradConfig.frad[0].tg1MasterAccess = 0x6U;
    config->fradConfig.frad[0].assignIsValid   = true;

    XSPI_Memset(config->ahbConfig.buffer, 0, sizeof(config->ahbConfig.buffer));

    /*  AHB buffer0 default config. */
    config->ahbConfig.buffer[0].enaPri.enablePriority = false;
    config->ahbConfig.buffer[0].enableSubdivision = false;
    config->ahbConfig.buffer[0].transferSize = 0x20U;
    config->ahbConfig.buffer[0].masterIndex = 0xFU; /* Invalid master index which is not used, so will never hit. */
    config->ahbConfig.buffer[0].topIndex = 0x30;
    /*  AHB buffer1 default config. */
    config->ahbConfig.buffer[1].enaPri.enablePriority = false;
    config->ahbConfig.buffer[1].enableSubdivision = false;
    config->ahbConfig.buffer[1].transferSize = 0x20U;
    config->ahbConfig.buffer[1].masterIndex = 0xFU; /* Invalid master index which is not used, so will never hit. */
    config->ahbConfig.buffer[1].topIndex = 0x60;
    /*  AHB buffer2 default config. */
    config->ahbConfig.buffer[2].enableSubdivision = false;
    config->ahbConfig.buffer[2].transferSize = 0x20U;
    config->ahbConfig.buffer[2].masterIndex = 0xFU; /* Invalid master index which is not used, so will never hit. */
    config->ahbConfig.buffer[2].topIndex = 0x90;
    /*  AHB buffer3 default config. */
    config->ahbConfig.buffer[3].enaPri.enableAllMaster = true;
    config->ahbConfig.buffer[3].enableSubdivision = false;
    config->ahbConfig.buffer[3].transferSize = 0x20U;
    config->ahbConfig.buffer[3].masterIndex = 0xFU; /* Invalid master index which is not used, so will never hit. */


    config->ahbConfig.enableAHBPrefetch  = false;
    config->ahbConfig.enableAHBWaitWrite = true;
    config->ahbConfig.enableAHBWaitRead  = true;
    config->ahbConfig.enableAHBSlite     = false;

    /* Dll default device config */
    devconfig->dllConfig.slaveFineOffset         = 0;
    devconfig->dllConfig.dllReferenceCounter     = 2U;
    devconfig->dllConfig.dllResolution           = 4U;
    devconfig->dllConfig.enableCdl8              = true;
    /* sample default device config */
    devconfig->smprConfig.tapNumber = 4U;
    devconfig->smprConfig.delayWithRef = kXSPI_SameWithDqs;
    devconfig->smprConfig.enableAtInvertedClock = 0;
}

/*!
 * brief Deinitializes the XSPI module.
 *
 * Clears the XSPI state and  XSPI module registers.
 * param base XSPI peripheral base address.
 */
void XSPI_Deinit(XSPI_Type *base)
{
    /* Reset peripheral. */
    XSPI_SoftwareReset(base);
}

/*!
 * brief Update XSPI DLL value depending on currently xspi root clock.
 *
 * param base XSPI peripheral base address.
 * param devconfig Flash configuration parameters.
 */
static void XSPI_UpdateDllValue(XSPI_Type *base, xspi_device_config_t *devconfig)
{
    uint32_t configValue = 0;
    uint32_t smprValue = 0;
    uint32_t rxSampleClock = (base->MCR & XSPI_MCR_DQS_FA_SEL_MASK) >> XSPI_MCR_DQS_FA_SEL_SHIFT;
    bool enableDataLearn = (base->MCR & XSPI_MCR_DLPEN_MASK) >> XSPI_MCR_DLPEN_SHIFT;

    /* Wait for bus to be idle before changing flash configuration. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }

    /* SDR and Bypass mode need config slaveDelayCoarse dllResolution and disable auto mode*/
    if ((!devconfig->enableDdr) && (devconfig->dllConfig.dllMode == kXSPI_BypassMode))
    {
        devconfig->dllConfig.slaveDelayCoarse   = 7U;
        devconfig->dllConfig.dllResolution      = 2U;
    }

    /* SDR and Auto mode need config slaveFineOffset */
    if ((!devconfig->enableDdr) && (devconfig->dllConfig.dllMode == kXSPI_AutoUpdateMode))
    {
        devconfig->dllConfig.slaveFineOffset = 2U;
    }

    /* Look back form DQS and Data learn need config dllResolution and dllReferenceCounter */
    if (enableDataLearn && (kXSPI_ReadSampleClkLoopbackFromDqsPad == rxSampleClock))
    {
        devconfig->dllConfig.dllResolution       = 6U;
        devconfig->dllConfig.dllReferenceCounter = 1U;
    }

    /* Bypass mode need config dllReferenceCounter and disable auto mode */
    if (devconfig->dllConfig.dllMode == kXSPI_BypassMode)
    {
        devconfig->dllConfig.dllReferenceCounter = 1U;
    }

    base->DLLCR[0] &=~ XSPI_DLLCR_SLV_UPD_MASK;
    base->DLLCR[0] |= XSPI_DLLCR_SLV_EN_MASK;
    base->DLLCR[0] |= XSPI_DLLCR_SLV_DLY_FINE_MASK;
    configValue = base->DLLCR[0];

    configValue |= XSPI_DLLCR_DLL_REFCNTR(devconfig->dllConfig.dllReferenceCounter) |
              XSPI_DLLCR_DLLRES(devconfig->dllConfig.dllResolution) |
              XSPI_DLLCR_SLV_DLY_COARSE(devconfig->dllConfig.slaveDelayCoarse) |
              XSPI_DLLCR_SLV_DLY_OFFSET(devconfig->dllConfig.slaveDelayOffset) |
              XSPI_DLLCR_SLV_FINE_OFFSET(devconfig->dllConfig.slaveFineOffset) |
              XSPI_DLLCR_DLL_CDL8(devconfig->dllConfig.enableCdl8);

    if (devconfig->dllConfig.dllMode == kXSPI_BypassMode)
	{
        configValue |= XSPI_DLLCR_SLV_DLL_BYPASS_MASK;
    }
	else if (devconfig->dllConfig.dllMode == kXSPI_AutoUpdateMode)
    {
        configValue |= XSPI_DLLCR_SLAVE_AUTO_UPDT_MASK;
	}

    if (devconfig->xspiRootClk > XSPI_HIGH_SAMPLE_FREQUENCY)
    {
        configValue |= XSPI_DLLCR_FREQEN_MASK;
    }

    base->DLLCR[0] |= configValue;
    base->DLLCR[0] |= XSPI_DLLCR_SLV_UPD_MASK;

    if (devconfig->dllConfig.dllMode == kXSPI_BypassMode)
	{
        /* Bypass mode need check slave delay chain locked */
        while(!((base->DLLSR & XSPI_DLLSR_SLVA_LOCK_MASK) == XSPI_DLLSR_SLVA_LOCK_MASK));
    }
	else if (devconfig->dllConfig.dllMode == kXSPI_AutoUpdateMode)
    {
        /* Auto mode need check slave delay chain and master delay chain both locked */
        while(!(((base->DLLSR & XSPI_DLLSR_SLVA_LOCK_MASK) == XSPI_DLLSR_SLVA_LOCK_MASK) &&
                 ((base->DLLSR & XSPI_DLLSR_DLLA_LOCK_MASK) == XSPI_DLLSR_DLLA_LOCK_MASK)));
	}
    base->DLLCR[0] &=~ XSPI_DLLCR_SLV_UPD_MASK;

    if (!devconfig->enableDdr)
    {
        devconfig->smprConfig.tapNumber = 7U;
        devconfig->smprConfig.delayWithRef = kXSPI_HalfCycleEarlyDqs;
        devconfig->smprConfig.enableAtInvertedClock = true;
    }

    if (enableDataLearn && (kXSPI_ReadSampleClkLoopbackFromDqsPad == rxSampleClock))
    {
        devconfig->smprConfig.tapNumber = 0U;
        devconfig->smprConfig.delayWithRef = kXSPI_HalfCycleEarlyDqs;
        devconfig->smprConfig.enableAtInvertedClock = true;
    }
    /* selecting tap number in slave delay chain tem 4*/
    smprValue = base->SMPR;
    smprValue |= XSPI_SMPR_DLLFSMPFA(devconfig->smprConfig.tapNumber) |
                 XSPI_SMPR_FSDLY(devconfig->smprConfig.delayWithRef) |
                 XSPI_SMPR_FSPHS(devconfig->smprConfig.enableAtInvertedClock);
    base->SMPR = smprValue;
}

/*!
 * brief Configures the connected device parameter.
 *
 * This function configures the connected device relevant parameters, such as the size, command, and so on.
 * The flash configuration value cannot have a default value. The user needs to configure it according to the
 * connected device.
 *
 * @param base XSPI peripheral base address.
 * param devconfig Flash configuration parameters.
 */
void XSPI_SetFlashConfig(XSPI_Type *base, xspi_device_config_t *devconfig)
{
    uint32_t configValue = 0;
    uint32_t instance = XSPI_GetInstance(base);
    uint32_t rxSampleClock = (base->MCR & XSPI_MCR_DQS_FA_SEL_MASK) >> XSPI_MCR_DQS_FA_SEL_SHIFT;
    bool enableDataLearn = (base->MCR & XSPI_MCR_DLPEN_MASK) >> XSPI_MCR_DLPEN_SHIFT;

    /* Wait for bus to be idle before changing flash configuration. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }

    if (enableDataLearn && (kXSPI_ReadSampleClkLoopbackFromDqsPad == rxSampleClock))
    {
        devconfig->CSHoldTime = 2U;
        devconfig->CSSetupTime = 2U;
    }

    base->FLSHCR = XSPI_FLSHCR_TCSS(devconfig->CSSetupTime) | XSPI_FLSHCR_TCSH(devconfig->CSHoldTime);

    if(devconfig->enableDdr)
    {
        configValue = base->MCR;
        configValue |= XSPI_MCR_DDR_EN(devconfig->enableDdr);
        base->MCR = configValue;
        configValue = base->FLSHCR;
        configValue |= XSPI_FLSHCR_TDH(devconfig->sampleTimeRef);
        base->FLSHCR = configValue;
    }

    /*!< Enable X16 Mode. Only for default 16 I/O bus PSRAM*/
    if(devconfig->enableX16Mode)
    {
        configValue = base->MCR;
        configValue |= XSPI_MCR_X16_EN_MASK;
        base->MCR = configValue;
    }

    /*XSPI set Word Addressable and columspace*/
    configValue = base->SFACR;
    base->SFACR |= XSPI_SFACR_CAS(devconfig->columnSpace) | XSPI_SFACR_WA(devconfig->enableWordAddress) |
                   XSPI_SFACR_PPWB(devconfig->PageProgramBoundary) |
                   XSPI_SFACR_WA_4B_EN(devconfig->enable4BWordAddress);

    /*Set Top address for serial flash memory A1  */
    base->SFAD[0][0] = s_xspiAmbaBase[instance] + XSPI_SFAD_TPAD(devconfig->flashA1Size);
    base->SFAD[0][1] = s_xspiAmbaBase[instance] + XSPI_SFAD_TPAD(devconfig->flashA1Size) +
                   XSPI_SFAD_TPAD(devconfig->flashA2Size);

    /*xspi AHB R/W/SR CONFIG*/
    configValue |= XSPI_BFGENCR_SEQID_WR((uint32_t)devconfig->AWRSeqIndex) |
                   XSPI_BFGENCR_SEQID_WR_EN_MASK | XSPI_BFGENCR_WR_FLUSH_EN(devconfig->enableWriteFlush);

    configValue |= XSPI_BFGENCR_SEQID((uint32_t)devconfig->ARDSeqIndex);

    configValue |= XSPI_BFGENCR_SEQID_RDSR((uint32_t)devconfig->ARDSRSeqIndex) |
                   XSPI_BFGENCR_PPWF_CLR_MASK;

    base->BFGENCR |= configValue;

    /* Configure DLL. */
    XSPI_UpdateDllValue(base, devconfig);

    /* Wait for bus to be idle before use it access to external flash. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }
}

/*! brief Updates the LUT table.
 *
 * param base XSPI peripheral base address.
 * param index From which index start to update. It could be any index of the LUT table, which
 * also allows user to update command content inside a command. Each command consists of up to
 * 10 instructions and occupy 4*32-bit memory.
 * param cmd Command sequence array.
 * param count Number of sequences.
 */
void XSPI_UpdateLUT(XSPI_Type *base, uint8_t index, const uint32_t *cmd, uint8_t count)
{
    assert(index < 80U);

    uint32_t i = 0;
    volatile uint32_t *lutBase;

    /* Wait for bus to be idle before changing flash configuration. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }

    /* Unlock LUT for update. */
    base->LUTKEY = XSPI_LUT_KEY_VAL;
    base->LCKCR = 0x02;

    lutBase = &base->LUT[index];
    for (i = 0; i < count; i++)
    {
        *lutBase++ = *cmd++;
    }

    /* Lock LUT. */
    base->LUTKEY = XSPI_LUT_KEY_VAL;
    base->LCKCR = 0x01;
}

/*! brief Update read sample clock source
 *
 * param base XSPI peripheral base address.
 * param clockSource clockSource of type #xspi_read_sample_clock_t
 */
void XSPI_UpdateRxSampleClock(XSPI_Type *base, xspi_read_sample_clock_t clockSource)
{
    uint32_t mcrVal;

    /* Wait for bus to be idle before changing flash configuration. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }

    mcrVal = base->MCR;
    mcrVal &= ~XSPI_MCR_DQS_FA_SEL_MASK;
    mcrVal |= XSPI_MCR_DQS_FA_SEL(clockSource);
    base->MCR = mcrVal;

    /* Reset peripheral. */
    XSPI_SoftwareReset(base);
}

/*!
 * brief Sends a buffer of data bytes using blocking method.
 * note This function blocks via polling until all bytes have been sent.
 * param base XSPI peripheral base address
 * param buffer The data bytes to send
 * param size The number of data bytes to send
 * retval kStatus_Success write success without error
 * retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * retval kStatus_XSPI_IpCommandSequenceError IP command sequence error detected
 * retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_WriteBlocking(XSPI_Type *base, uint8_t *buffer, size_t size)
{
    status_t result = kStatus_Success;
    uint32_t i      = 0;
    uint8_t fsmStatusIsValid;
    uint32_t fsmStatus;

    base->TBCT = 256 - (size/4 - 1);

    fsmStatusIsValid = (base->FSMSTAT) & XSPI_FSMSTAT_VLD_MASK >> XSPI_FSMSTAT_VLD_SHIFT;
    fsmStatus = (base->FSMSTAT & XSPI_FSMSTAT_STATE_MASK) >> XSPI_FSMSTAT_STATE_SHIFT;
    /* Wait until passed MDAD and FRAD checks and the transaction is granted the arbitration. */
    while (!((1U == fsmStatusIsValid) & (fsmStatus == (uint32_t)kXSPI_TbdrLockIsOpen ||
                                    fsmStatus == (uint32_t)kXSPI_WriteTransferIsTriggered)))
    {
    }
    /* Send data buffer */
    while (0U != size)
    {

        result = XSPI_CheckAndClearError(base, base->ERRSTAT);

        if (kStatus_Success != result)
        {
            return result;
        }

        /* Write watermark level data into tx fifo . */
            /* Write word aligned data into tx fifo. */
        for (i = 0U; i < (size / 4U); i++)
        {
            while(1 == (base->SR & XSPI_SR_TXFULL_MASK) >> XSPI_SR_TXFULL_SHIFT)
            {
            }
            base->TBDR = *(uint32_t *)buffer;
            buffer += 4U;
        }

        /* Adjust size by the amount processed. */
        size -= 4U * i;

        /* Write word un-aligned data into tx fifo. */
        if (0x00U != size)
        {
            uint32_t tempVal = 0x00U;

            for (uint32_t j = 0U; j < size; j++)
            {
                tempVal |= ((uint32_t)*buffer++ << (8U * j));
            }

            while(1 == (base->SR & XSPI_SR_TXFULL_MASK) >> XSPI_SR_TXFULL_SHIFT)
            {
            }
            base->TBDR = tempVal;
            }

            size = 0U;
        }

    /*clear TX Buffer Fill Flag*/
    base->FR |= XSPI_FR_TBFF_MASK;

    XSPI_IPS_CMD_WaitFinish(base);

    return result;
}

/*!
 * brief Receives a buffer of data bytes using a blocking method.
 * note This function blocks via polling until all bytes have been sent.
 * param base XSPI peripheral base address
 * param buffer The data bytes to send
 * param size The number of data bytes to receive
 * retval kStatus_Success read success without error
 * retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * retval kStatus_XSPI_IpCommandSequenceError IP command sequence error detected
 * retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_ReadBlocking(XSPI_Type *base, uint8_t *buffer, size_t size)
{
    uint32_t rxWatermark = base->RBCT + 1;
    status_t result = kStatus_Success;
    uint32_t i      = 0;

    while(1 != (base->SR & XSPI_SR_BUSY_MASK) && ((base->SR & XSPI_SR_IP_ACC_MASK) >> XSPI_SR_IP_ACC_SHIFT))
    {
    }
    /* Send data buffer */
    while (0U != size)
    {

        result = XSPI_CheckAndClearError(base, base->ERRSTAT);

        if (kStatus_Success != result)
        {
            break;
        }

        /* Read word aligned data from rx fifo. */
        if (size >= 4 * rxWatermark)
        {
            while(0 == (base->SR & XSPI_SR_RXWE_MASK) >> XSPI_SR_RXWE_SHIFT)
            {
            }
            for (i = 0U; i < rxWatermark; i++)
            {
                *(uint32_t *)buffer = base->RBDR[i];
                buffer += 4U;
            }

            /* RX buffer POP */
            base->FR |= XSPI_FR_RBDF_MASK;
            size = size - 4U * rxWatermark;
        }
        else
        {
            for (i = 0U; i < (size / 4U); i++)
            {
                *(uint32_t *)buffer = base->RBDR[i];
                buffer += 4U;
            }

            /* Adjust size by the amount processed. */
            size -= 4U * i;

            /* Read word un-aligned data from rx fifo. */
            if (0x00U != size)
            {
                uint32_t tempVal = base->RBDR[i];

                for (i = 0U; i < size; i++)
                {
                    *buffer++ = ((uint8_t)(tempVal >> (8U * i)) & 0xFFU);
                }
            }

            size = 0;
        }
    }

    return result;
}

/*!
 * brief Execute command to transfer a buffer data bytes using a blocking method.
 * param base XSPI peripheral base address
 * param xfer pointer to the transfer structure.
 * retval kStatus_Success command transfer success without error
 * retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * retval kStatus_XSPI_IpCommandSequenceError IP command sequence error detected
 * retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_TransferBlocking(XSPI_Type *base, xspi_transfer_t *xfer)
{
    uint32_t configValue = 0;
    status_t result      = kStatus_Success;

    if (xfer->targetGroup == kXSPI_TargetGroup1)
    {
        /*check the target groupe 1 is empty*/
        while (0U != ((base->SUB_REG_MDAM_ARRAY[0].TGSFARS_SUB & XSPI_TGSFARS_SUB_VLD_MASK) >> XSPI_TGSFARS_SUB_VLD_SHIFT))
        {
        }

        /* Set target groupe 1 Serial Flash Address register*/
        base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_SFAR = xfer->deviceAddress;

        /* Configure target groupe 1 data size. */
        if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
        {
            configValue = XSPI_SFP_TG_SUB_IPCR_IDATSZ(xfer->dataSize);
        }

        /*config target groupe 1 data size sequence ID.*/
        configValue |= XSPI_SFP_TG_SUB_IPCR_SEQID(xfer->seqIndex);
        base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_IPCR = configValue;
    }
    else
    {
         /*check the target groupe 0 is empty*/
        while (0U != ((base->TGSFARS & XSPI_TGSFARS_VLD_MASK) >> XSPI_TGSFARS_VLD_SHIFT))
        {
        }

        /* Set target groupe 0 Serial Flash Address register*/
        base->SFP_TG_SFAR = xfer->deviceAddress;

        /* Configure target groupe 0 data size. */
        if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
        {
            configValue = XSPI_SFP_TG_IPCR_IDATSZ(xfer->dataSize);
        }

        /*config target groupe 0 data size sequence ID.*/
        configValue |= XSPI_SFP_TG_IPCR_SEQID(xfer->seqIndex);
        base->SFP_TG_IPCR = configValue;
    }

    /* Just Clear RX  TX FIFO PT*/
    base->MCR |= XSPI_MCR_CLR_RXF_MASK;
    base->MCR |= XSPI_MCR_CLR_TXF_MASK;

    if ((xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
    {
        result = XSPI_WriteBlocking(base, (uint8_t *)xfer->data, xfer->dataSize);
    }
    else if (xfer->cmdType == kXSPI_Read)
    {
        /*wait ip cmd finish*/
        XSPI_IPS_CMD_WaitFinish(base);
        result = XSPI_ReadBlocking(base, (uint8_t *)xfer->data, xfer->dataSize);
    }
    else
    {
        /* Empty else. */
    }

    /* Wait for bus to be idle before changing flash configuration. */
    while (!XSPI_GetBusIdleStatus(base))
    {
    }

    if (xfer->cmdType == kXSPI_Command)
    {
        result = XSPI_CheckAndClearError(base, base->ERRSTAT);
        /*wait ip cmd finish*/
        XSPI_IPS_CMD_WaitFinish(base);
    }

    return result;
}

/*!
 * brief Initializes the XSPI handle which is used in transactional functions.
 *
 * param base XSPI peripheral base address.
 * param handle pointer to xspi_handle_t structure to store the transfer state.
 * param callback pointer to user callback function.
 * param userData user parameter passed to the callback function.
 */
void XSPI_TransferCreateHandle(XSPI_Type *base,
                                  xspi_handle_t *handle,
                                  xspi_transfer_callback_t callback,
                                  void *userData)
{
    assert(NULL != handle);

    uint32_t instance = XSPI_GetInstance(base);

    /* Zero handle. */
    (void)memset(handle, 0, sizeof(*handle));

    /* Set callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;


    /* Enable NVIC interrupt. */
    (void)EnableIRQ(s_xspiIrqs[instance]);
}

/*!
 * brief Performs a interrupt non-blocking transfer on the XSPI bus.
 *
 * note Calling the API returns immediately after transfer initiates. The user needs
 * to call XSPI_GetTransferCount to poll the transfer status to check whether
 * the transfer is finished. If the return status is not kStatus_XSPI_Busy, the transfer
 * is finished. For XSPI_Read, the dataSize should be multiple of rx watermark level, or
 * XSPI could not read data properly.
 *
 * param base XSPI peripheral base address.
 * param handle pointer to xspi_handle_t structure which stores the transfer state.
 * param xfer pointer to xspi_transfer_t structure.
 * retval kStatus_Success Successfully start the data transmission.
 * retval kStatus_XSPI_Busy Previous transmission still not finished.
 */
status_t XSPI_TransferNonBlocking(XSPI_Type *base, xspi_handle_t *handle, xspi_transfer_t *xfer)
{
    uint32_t configValue = 0;
    status_t result      = kStatus_Success;

    assert(NULL != handle);
    assert(NULL != xfer);

    /* Check if the I2C bus is idle - if not return busy status. */
    if (handle->state != (uint32_t)kXSPI_Idle)
    {
        result = kStatus_XSPI_Busy;
    }
    else
    {
        handle->data              = (uint8_t *)xfer->data;
        handle->dataSize          = xfer->dataSize;
        handle->transferTotalSize = xfer->dataSize;
        handle->state = (xfer->cmdType == kXSPI_Read) ? (uint32_t)kXSPI_BusyRead : (uint32_t)kXSPI_BusyWrite;

        if (xfer->targetGroup == kXSPI_TargetGroup1)
        {
                /*check the target groupe 1 is empty*/
                while (0U != ((base->SUB_REG_MDAM_ARRAY[0].TGSFARS_SUB & XSPI_TGSFARS_SUB_VLD_MASK) >> XSPI_TGSFARS_SUB_VLD_SHIFT))
                {
                }

                /* Set target groupe 1 Serial Flash Address register*/
                base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_SFAR = xfer->deviceAddress;

                /* Configure target groupe 1 data size. */
                if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
                {
                        configValue = XSPI_SFP_TG_SUB_IPCR_IDATSZ(xfer->dataSize);
                }

                /*config target groupe 1 data size sequence ID.*/
                configValue |= XSPI_SFP_TG_SUB_IPCR_SEQID(xfer->seqIndex);
                base->SUB_REG_MDAM_ARRAY[0].SFP_TG_SUB_IPCR = configValue;
        }
        else
        {
                 /*check the target groupe 0 is empty*/
                while (0U != ((base->TGSFARS & XSPI_TGSFARS_VLD_MASK) >> XSPI_TGSFARS_VLD_SHIFT))
                {
                }

                /* Set target groupe 0 Serial Flash Address register*/
                base->SFP_TG_SFAR = xfer->deviceAddress;

                /* Configure target groupe 0 data size. */
                if ((xfer->cmdType == kXSPI_Read) || (xfer->cmdType == kXSPI_Write) || (xfer->cmdType == kXSPI_Config))
                {
                        configValue = XSPI_SFP_TG_IPCR_IDATSZ(xfer->dataSize);
                }

                /*config target groupe 0 data size sequence ID.*/
                configValue |= XSPI_SFP_TG_IPCR_SEQID(xfer->seqIndex);
                base->SFP_TG_IPCR = configValue;
        }

        /* Just Clear RX  TX FIFO PT*/
        base->MCR |= XSPI_MCR_CLR_RXF_MASK;
        base->MCR |= XSPI_MCR_CLR_TXF_MASK;



        if (handle->state == (uint32_t)kXSPI_BusyRead)
        {
            XSPI_EnableInterrupts(base, (uint32_t)kXSPI_RxBufferOverflowFlag |
                                        (uint32_t)kXSPI_RxBufferDrainFlag |
                                        (uint32_t)kXSPI_IpCmdtriggerErrorFlag |
                                        (uint32_t)kXSPI_IllegalInstructionErrorFlag);
        }
        else
        {
            XSPI_EnableInterrupts(
                base, (uint32_t)kXSPI_TxBufferFillFlag | (uint32_t)kXSPI_TxBufferUnderrunFlag |
                      (uint32_t)kXSPI_IpCmdtriggerErrorFlag | (uint32_t)kXSPI_IllegalInstructionErrorFlag);
        }
    }

    return result;
}

/*!
 * brief Gets the master transfer status during a interrupt non-blocking transfer.
 *
 * param base XSPI peripheral base address.
 * param handle pointer to xspi_handle_t structure which stores the transfer state.
 * param count Number of bytes transferred so far by the non-blocking transaction.
 * retval kStatus_InvalidArgument count is Invalid.
 * retval kStatus_Success Successfully return the count.
 */
status_t XSPI_TransferGetCount(XSPI_Type *base, xspi_handle_t *handle, size_t *count)
{
    assert(NULL != handle);

    status_t result = kStatus_Success;

    if (handle->state == (uint32_t)kXSPI_Idle)
    {
        result = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->transferTotalSize - handle->dataSize;
    }

    return result;
}

/*!
 * brief Aborts an interrupt non-blocking transfer early.
 *
 * note This API can be called at any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * param base XSPI peripheral base address.
 * param handle pointer to xspi_handle_t structure which stores the transfer state
 */
void XSPI_TransferAbort(XSPI_Type *base, xspi_handle_t *handle)
{
    assert(NULL != handle);

    XSPI_DisableInterrupts(base, (uint32_t)kIrqFlags);
    handle->state = (uint32_t)kXSPI_Idle;
}
