/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "fsl_debug_console.h"
#if defined(MIMXRT798S_cm33_core0_SERIES)
#include "fsl_cache.h"
#endif
#include "fsl_clock.h"
#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
#include "fsl_lpi2c.h"
#endif /* SDK_I2C_BASED_COMPONENT_USED */
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(MIMXRT798S_cm33_core0_SERIES)
#define HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_READ   0
#define HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_WRITE  1
#define HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ  2
#define HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE 3
#define HYPERRAM_CMD_LUT_SEQ_IDX_REG_READ    4
#define HYPERRAM_CMD_LUT_SEQ_IDX_REG_WRITE   5
#define HYPERRAM_CMD_LUT_SEQ_IDX_RESET       6

#define CUSTOM_LUT_LENGTH 40
#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq;

#if defined(MIMXRT798S_cm33_core0_SERIES)
    CLOCK_AttachClk(BOARD_DEBUG_UART_FCCLK_ATTACH);
    CLOCK_SetClkDiv(BOARD_DEBUG_UART_FCCLK_DIV, 1U);

    /* attach FC0 clock to LP_FLEXCOMM (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
#else
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);
    CLOCK_SetClkDiv(BOARD_DEBUG_UART_CLK_DIV, 1U);
#endif

    uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

#if defined(MIMXRT798S_cm33_core0_SERIES)
void BOARD_ConfigMPU(void)
{
    uint8_t attr;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
    extern uint32_t Image$$RW_m_ncache$$Base[];
    /* RW_m_ncache_unused is a auxiliary region which is used to get the whole size of noncache section */
    extern uint32_t Image$$RW_m_ncache_unused$$Base[];
    extern uint32_t Image$$RW_m_ncache_unused$$ZI$$Limit[];
    uint32_t nonCacheStart = (uint32_t)Image$$RW_m_ncache$$Base;
    uint32_t nonCacheSize  = ((uint32_t)Image$$RW_m_ncache_unused$$Base == nonCacheStart) ?
                                 0 :
                                 ((uint32_t)Image$$RW_m_ncache_unused$$ZI$$Limit - nonCacheStart);
#elif defined(__MCUXPRESSO)
    extern uint32_t __base_NCACHE_REGION;
    extern uint32_t __top_NCACHE_REGION;
    uint32_t nonCacheStart = (uint32_t)(&__base_NCACHE_REGION);
    uint32_t nonCacheSize  = (uint32_t)(&__top_NCACHE_REGION) - nonCacheStart;
#elif defined(__ICCARM__) || defined(__GNUC__)
    extern uint32_t __NCACHE_REGION_START[];
    extern uint32_t __NCACHE_REGION_SIZE[];
    uint32_t nonCacheStart = (uint32_t)__NCACHE_REGION_START;
    uint32_t nonCacheSize  = (uint32_t)__NCACHE_REGION_SIZE;
#else
#error "Unsupported compiler"
#endif

    XCACHE_DisableCache(XCACHE0);
    XCACHE_DisableCache(XCACHE1);

    /* Disable MPU */
    ARM_MPU_Disable();

    /* Attr0: device memory. */
    ARM_MPU_SetMemAttr(0U, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE));
    /* Attr1: non cacheable. */
    ARM_MPU_SetMemAttr(1U, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
    /* Attr2: non transient, write through, read allocate. */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 0U, 1U, 0U);
    ARM_MPU_SetMemAttr(2U, ARM_MPU_ATTR(attr, attr));
    /* Attr3: non transient, write back, read/write allocate. */
    attr = ARM_MPU_ATTR_MEMORY_(0U, 1U, 1U, 1U);
    ARM_MPU_SetMemAttr(3U, ARM_MPU_ATTR(attr, attr));

    /* Region 0: [0x0, 0x1FFFFFFF], non-shareable, read/write, any privileged, executable. Attr 2 (write through). */
    ARM_MPU_SetRegion(0U, ARM_MPU_RBAR(0U, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x1FFFFFFFU, 2U));
    /* Region 2 (Peripherals): [0x40000000, 0x5FFFFFFF], non-shareable, read/write, non-privileged, executable. Attr 0
     * (device). */
    ARM_MPU_SetRegion(2U, ARM_MPU_RBAR(0x40000000U, ARM_MPU_SH_NON, 0U, 1U, 0U), ARM_MPU_RLAR(0x5FFFFFFF, 0U));

    if (nonCacheSize != 0)
    {
        /* The MPU region size should a granularity of 32 bytes. */
        assert((nonCacheSize & 0x1FU) == 0x0U);

        /* Region 1 setting : outter-shareable, read-write,  non-privileged, executable. Attr 1. (non-cacheable) */
        ARM_MPU_SetRegion(1U, ARM_MPU_RBAR(nonCacheStart, ARM_MPU_SH_OUTER, 0U, 1U, 0U),
                          ARM_MPU_RLAR(nonCacheStart + nonCacheSize - 1, 1U));
    }

    /*
     * Enable MPU and HFNMIENA feature
     * HFNMIENA ensures the core uses MPU configuration when in hard fault, NMI, and FAULTMASK handlers,
     * otherwise all memory regions are accessed without MPU protection.
     */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);

    /* Enable code & system cache */
    XCACHE_EnableCache(XCACHE0);
    XCACHE_EnableCache(XCACHE1);

    /* flush pipeline */
    __DSB();
    __ISB();
}

status_t xspi_hyper_ram_write_mcr(XSPI_Type *base, uint32_t regAddr, uint32_t *mrVal)
{
    xspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
    flashXfer.deviceAddress = regAddr;
    flashXfer.cmdType       = kXSPI_Write;
    flashXfer.seqIndex      = HYPERRAM_CMD_LUT_SEQ_IDX_REG_WRITE;
    flashXfer.data          = mrVal;
    flashXfer.dataSize      = 2;

    status = XSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t xspi_hyper_ram_reset(XSPI_Type *base, uint32_t ambaAddr)
{
    xspi_transfer_t flashXfer;
    status_t status;

    /* Write data */
    flashXfer.deviceAddress = ambaAddr;
    flashXfer.cmdType       = kXSPI_Command;
    flashXfer.seqIndex      = HYPERRAM_CMD_LUT_SEQ_IDX_RESET;

    status = XSPI_TransferBlocking(base, &flashXfer);

    if (status == kStatus_Success)
    {
        for (uint32_t i = 300000U; i > 0; i--)
        {
            __NOP();
        }
    }
    return status;
}

status_t BOARD_Init16bitsPsRam(XSPI_Type *base)
{
    xspi_config_t config;
    uint32_t mr0Val[1];
    uint32_t mr1Val[1];
    uint32_t mr2Val[1];
    status_t ret      = kStatus_Success;
    uint32_t ambaAddr = 0U;

    const uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
        /* Linear Sync Read */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_READ] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0, kXSPI_Command_DDR, kXSPI_8PAD, 0),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_READ + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x8),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_READ + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_READ_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_STOP, kXSPI_8PAD, 0),

        /* Linear Sync Write */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_WRITE] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0x80, kXSPI_Command_DDR, kXSPI_8PAD, 0x80),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_WRITE + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x8),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_SYNC_WRITE + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_WRITE_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_STOP, kXSPI_8PAD, 0),

        /* Linear Burst Read */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DDR, kXSPI_8PAD, 0x20),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0xc),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x2, kXSPI_Command_READ_DDR, kXSPI_8PAD, 0x40),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ + 3] =
            XSPI_LUT_SEQ(kXSPI_Command_STOP, kXSPI_8PAD, 0x0, kXSPI_Command_STOP, kXSPI_8PAD, 0x0),

        /* Linear Burst Write */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0xA0, kXSPI_Command_DDR, kXSPI_8PAD, 0xA0),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x4),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x2, kXSPI_Command_WRITE_DDR, kXSPI_8PAD, 0x40),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE + 3] =
            XSPI_LUT_SEQ(kXSPI_Command_STOP, kXSPI_8PAD, 0x0, kXSPI_Command_STOP, kXSPI_8PAD, 0x0),

        /* Mode Register Read */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_READ] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0x40, kXSPI_Command_DDR, kXSPI_8PAD, 0x40),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_READ + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x2),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_READ + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_DUMMY_SDR, kXSPI_8PAD, 0x2, kXSPI_Command_READ_DDR, kXSPI_8PAD, 0x4),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_READ + 3] =
            XSPI_LUT_SEQ(kXSPI_Command_STOP, kXSPI_8PAD, 0x0, kXSPI_Command_STOP, kXSPI_8PAD, 0x0),

        /* Mode Register write */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_WRITE] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0xC0, kXSPI_Command_DDR, kXSPI_8PAD, 0xC0),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_WRITE + 1] =
            XSPI_LUT_SEQ(kXSPI_Command_RADDR_DDR, kXSPI_8PAD, 0x20, kXSPI_Command_WRITE_DDR, kXSPI_8PAD, 0x1),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_REG_WRITE + 2] =
            XSPI_LUT_SEQ(kXSPI_Command_STOP, kXSPI_8PAD, 0x0, kXSPI_Command_STOP, kXSPI_8PAD, 0x0),

        /* reset */
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_RESET] =
            XSPI_LUT_SEQ(kXSPI_Command_DDR, kXSPI_8PAD, 0xFF, kXSPI_Command_DDR, kXSPI_8PAD, 0xFF),
        [5 * HYPERRAM_CMD_LUT_SEQ_IDX_RESET + 1] XSPI_LUT_SEQ(kXSPI_Command_STOP, kXSPI_8PAD, 0x0, kXSPI_Command_STOP,
                                                              kXSPI_8PAD, 0x0),
    };

    xspi_device_config_t deviceconfig = {
        .flashA1Size   = 0x8000U, /* 256Mb/KByte */
        .sampleTimeRef = kXSPI_2xFlashHalfClock,
        .CSHoldTime    = 3,
        .CSSetupTime   = 3,
        .AWRSeqIndex   = HYPERRAM_CMD_LUT_SEQ_IDX_BURST_WRITE,
        .ARDSeqIndex   = HYPERRAM_CMD_LUT_SEQ_IDX_BURST_READ,
        .enableDdr     = true,
    };

    if (base == XSPI2)
    {
        ambaAddr = XSPI2_AMBA_BASE;
        POWER_DisablePD(kPDRUNCFG_APD_XSPI2);
        POWER_DisablePD(kPDRUNCFG_PPD_XSPI2);
        POWER_ApplyPD();

        CLOCK_AttachClk(kCOMMON_BASE_to_XSPI2);
        CLOCK_SetClkDiv(kCLOCK_DivXspi2Clk, 1);
#if (defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
        CLOCK_EnableClock(kCLOCK_Xspi2);
#endif
    }
    else
    {
        ambaAddr = XSPI1_AMBA_BASE;
        POWER_DisablePD(kPDRUNCFG_APD_XSPI1);
        POWER_DisablePD(kPDRUNCFG_PPD_XSPI1);
        POWER_ApplyPD();

        CLOCK_AttachClk(kCOMPUTE_BASE_to_XSPI1);
        CLOCK_SetClkDiv(kCLOCK_DivXspi1Clk, 1);
#if (defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
        CLOCK_EnableClock(kCLOCK_Xspi1);
#endif
    }

    /* Get XSPI default settings and configure the xspi. */
    XSPI_GetDefaultConfig(base, &config, &deviceconfig);
    config.rxSampleClock = kXSPI_ReadSampleClkExternalInputFromDqsPad;
    /*Set AHB buffer size for reading data through AHB bus. */
    config.ahbConfig.enableAHBPrefetch = false;
    config.enableDqsOut                = true;

    XSPI_Init(base, &config);
    /* Configure flash settings according to serial flash feature. */
    XSPI_SetFlashConfig(base, &deviceconfig);

    /* Update LUT table. */
    XSPI_UpdateLUT(base, 0, customLUT, CUSTOM_LUT_LENGTH);

    /* Reset hyper ram. */
    ret = xspi_hyper_ram_reset(base, ambaAddr);
    if (ret != kStatus_Success)
    {
        return ret;
    }

    /* change wlc to 7 */
    mr0Val[0] = 0x20;
    ret       = xspi_hyper_ram_write_mcr(base, ambaAddr + 0x4U, mr0Val);
    if (ret != kStatus_Success)
    {
        return ret;
    }
    /* change rlc to 14 */
    mr1Val[0] = 0x30;
    ret       = xspi_hyper_ram_write_mcr(base, ambaAddr + 0U, mr1Val);
    if (ret != kStatus_Success)
    {
        return ret;
    }
    mr2Val[0] = 0x4D;
    ret       = xspi_hyper_ram_write_mcr(base, ambaAddr + 0x8U, mr2Val);
    if (ret != kStatus_Success)
    {
        return ret;
    }

    /* Enable X16 mode. */
    XSPI_EnableX16Mode(base);

    return ret;
}

#endif /* MIMXRT798S_cm33_core0_SERIES */

/* Disable the secure check for Media Domain */
void BOARD_InitAHBSC(void)
{
    /* Write enable  */
    GLIKEY2->CTRL_0 = 0x00060000;
    GLIKEY2->CTRL_0 = 0x00020001;
    GLIKEY2->CTRL_0 = 0x00010001;
    GLIKEY2->CTRL_1 = 0x00290000;
    GLIKEY2->CTRL_0 = 0x00020001;
    GLIKEY2->CTRL_1 = 0x00280000;
    GLIKEY2->CTRL_0 = 0x00000001;

    /*Disable secure and secure privilege checking. */
    AHBSC_MEDIA->MISC_CTRL_DP_REG = 0x000086aa;
    AHBSC_MEDIA->MISC_CTRL_REG    = 0x000086aa;
}

#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_I2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz)
{
    lpi2c_master_config_t i2cConfig = {0};

    LPI2C_MasterGetDefaultConfig(&i2cConfig);
    LPI2C_MasterInit(base, &i2cConfig, clkSrc_Hz);
}

status_t BOARD_I2C_Send(LPI2C_Type *base,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize)
{
    lpi2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = txBuff;
    masterXfer.dataSize       = txBuffSize;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    return LPI2C_MasterTransferBlocking(base, &masterXfer);
}

status_t BOARD_I2C_Receive(LPI2C_Type *base,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{
    lpi2c_master_transfer_t masterXfer;

    /* Prepare transfer structure. */
    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.subaddress     = subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxBuffSize;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    return LPI2C_MasterTransferBlocking(base, &masterXfer);
}

void BOARD_PMIC_I2C_Init(void)
{
    BOARD_I2C_Init(BOARD_PMIC_I2C_BASEADDR, BOARD_PMIC_I2C_CLOCK_FREQ);
}

status_t BOARD_PMIC_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize)
{
    return BOARD_I2C_Send(BOARD_PMIC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, (uint8_t *)txBuff,
                          txBuffSize);
}

status_t BOARD_PMIC_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    return BOARD_I2C_Receive(BOARD_PMIC_I2C_BASEADDR, deviceAddress, subAddress, subAddressSize, rxBuff, rxBuffSize);
}
#endif
