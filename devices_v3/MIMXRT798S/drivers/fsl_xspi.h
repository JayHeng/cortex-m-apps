/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FSL_XSPI_H_
#define FSL_XSPI_H_

#include "fsl_common.h"

/*!
 * @addtogroup xspi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
#define FSL_XSPI_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@{*/

/*! @brief Formula to form XSPI instructions in LUT table. */
#define XSPI_LUT_SEQ(cmd0, pad0, op0, cmd1, pad1, op1)                                                              \
    (XSPI_LUT_INSTR0(cmd0) | XSPI_LUT_PAD0(pad0) | XSPI_LUT_OPRND0(op0) | XSPI_LUT_INSTR1(cmd1) | \
     XSPI_LUT_PAD1(pad1) | XSPI_LUT_OPRND1(op1))

/*! @brief Status structure of XSPI.*/
enum
{
    kStatus_XSPI_Busy                     = MAKE_STATUS(kStatusGroup_XSPI, 0), /*!< XSPI is Busy */
    kStatus_XSPI_IpCommandUsageError      = MAKE_STATUS(kStatusGroup_XSPI, 1), /*!< XSPI Ip Command Usage Error*/
    kStatus_XSPI_IpCommandtriggerError    = MAKE_STATUS(kStatusGroup_XSPI, 2), /*!< XSPI Ip Command Trigger Error*/
    kStatus_XSPI_IllegalInstructionError  = MAKE_STATUS(kStatusGroup_XSPI, 3), /*!< XSPI Illegal Instruction Error*/
    kStatus_XSPI_SequenceExecutionTimeout = MAKE_STATUS(kStatusGroup_XSPI, 4), /*!< XSPI Sequence Execution Timeout*/
    kStatus_XSPI_FradCheckError           = MAKE_STATUS(kStatusGroup_XSPI, 5), /*!< XSPI Frad Check Error*/
    kStatus_XSPI_TgQueueWritingError      = MAKE_STATUS(kStatusGroup_XSPI, 6), /*!< XSPI Tg Queue Writing Error*/
    kStatus_XSPI_IpsBusTransError         = MAKE_STATUS(kStatusGroup_XSPI, 7), /*!< XSPI Ip Bus Transfer Error*/
};

/*! @brief CMD definition of XSPI, use to form LUT instruction, _xspi_command. */
enum
{
    kXSPI_Command_STOP              = 0x00U, /*!< Stop execution, deassert CS. */
    kXSPI_Command_SDR               = 0x01U, /*!< Transmit Command code to Flash, using SDR mode. */
    kXSPI_Command_RADDR_SDR         = 0x02U, /*!< Transmit Row Address to Flash, using SDR mode. */
    kXSPI_Command_DUMMY_SDR         = 0x03U, /*!< Leave data lines undriven by xSPI controller, using SDR mode. */
    kXSPI_Command_MODE_SDR          = 0x04U, /*!< Transmit 8-bit Mode bits to Flash, using SDR mode. */
    kXSPI_Command_MODE2_SDR         = 0x05U, /*!< Transmit 2-bit Mode bits to Flash, using SDR mode. */
    kXSPI_Command_MODE4_SDR         = 0x06U, /*!< Transmit 4-bit Mode bits to Flash, using SDR mode. */
    kXSPI_Command_READ_SDR          = 0x07U, /*!< Receive Read Data from Flash, using SDR mode. */
    kXSPI_Command_WRITE_SDR         = 0x08U, /*!< Transmit Programming Data to Flash, using SDR mode. */
    kXSPI_Command_JMP_ON_CS         = 0x09U, /*!< Stop execution, deassert CS and save operand[7:0] as the
                                               instruction start pointer for next sequence*/
    kXSPI_Command_RADDR_DDR         = 0x0AU, /*!< Transmit Row Address to Flash, using DDR mode. */
    kXSPI_Command_MODE_DDR          = 0x0BU, /*!< Transmit 8-bit Mode bits to Flash, using DDR mode. */
    kXSPI_Command_MODE2_DDR         = 0x0CU, /*!< Transmit 2-bit Mode bits to Flash, using DDR mode. */
    kXSPI_Command_MODE4_DDR         = 0x0DU, /*!< Transmit 4-bit Mode bits to Flash, using DDR mode. */
    kXSPI_Command_READ_DDR          = 0x0EU, /*!< Receive Read Data from Flash, using DDR mode. */
    kXSPI_Command_WRITE_DDR         = 0x0FU, /*!< Transmit Programming Data to Flash, using DDR mode. */
    kXSPI_Command_LEARN_DDR         = 0x10U, /*!< Receive Read Data or Preamble bit from Flash, DDR mode. */
    kXSPI_Command_DDR               = 0x11U, /*!< Transmit Command code to Flash, using DDR mode. */
    kXSPI_Command_CADDR_SDR         = 0x12U, /*!< Transmit Column Address to Flash, using SDR mode. */
    kXSPI_Command_CADDR_DDR         = 0x13U, /*!< Transmit Column Address to Flash, using DDR mode. */
    kXSPI_Command_JUMP_TO_SEQ       = 0x14U,
};

/*! @brief pad definition of XSPI, use to form LUT instruction. */
typedef enum _xspi_pad
{
    kXSPI_1PAD = 0x00U, /*!< Transmit command/address and transmit/receive data only through DATA0/DATA1. */
    kXSPI_2PAD = 0x01U, /*!< Transmit command/address and transmit/receive data only through DATA[1:0]. */
    kXSPI_4PAD = 0x02U, /*!< Transmit command/address and transmit/receive data only through DATA[3:0]. */
    kXSPI_8PAD = 0x03U, /*!< Transmit command/address and transmit/receive data only through DATA[7:0]. */
} xspi_pad_t;

/*! @brief XSPI interrupt status flags.*/
typedef enum _xspi_flags
{
    kXSPI_DataLearningFailedFlag = XSPI_FR_DLPFF_MASK, /*!< Data learning failed. */
    kXSPI_DllAbortFlag = XSPI_FR_DLLABRT_MASK,         /*!< DLL Abort. */
    kXSPI_DllUnlockFlag = XSPI_FR_DLLUNLCK_MASK,       /*!< This field is set whenever DLL unlock event occurs,
                                                         irrespective of flash memory access.*/
    kXSPI_TxBufferFillFlag = XSPI_FR_TBFF_MASK,        /*!< If the field remains cleared,
                                                         the TX buffer can be considered as full.*/
    kXSPI_TxBufferUnderrunFlag = XSPI_FR_TBUF_MASK,    /*!< This field is set if the module tries to pull data
                                                         when the TX buffer is empty*/
    kXSPI_RxBufferOverflowFlag = XSPI_FR_RBOF_MASK,    /*!< This field is set when no more data can be pushed
                                                         into the RX buffer from the serial flash memory device.*/
    kXSPI_RxBufferDrainFlag = XSPI_FR_RBDF_MASK,       /*!< This field is set if SR[RXWE] is asserted.*/
    kXSPI_AhbAbortErrorFlag = XSPI_FR_AAEF_MASK,       /*!< This flag can be set when AHB transaction error*/
    kXSPI_AhbIllegalTransactionErrorFlag = XSPI_FR_AITEF_MASK,
    kXSPI_AhbIllegalBurstSizeErrorFlag = XSPI_FR_AIBSEF_MASK,
    kXSPI_AhbBufferOverflowFlag = XSPI_FR_ABOF_MASK,
    kXSPI_AhbPerformanceMonitorOverflowFlag = XSPI_FR_PERFOVF_MASK,
    kXSPI_AhbReadAddressErrorFlag = XSPI_FR_RDADDR_MASK,
    kXSPI_IllegalInstructionErrorFlag = XSPI_FR_ILLINE_MASK, /*!< This field is set when an illegal instruction is
                                                               encountered by the controller in any of the sequences.*/
    kXSPI_IpCmdtriggerErrorFlag = XSPI_FR_IPIEF_MASK,
    kXSPI_PageProgramWaitFlag = XSPI_FR_PPWF_MASK,
    kXSPI_IPEDRxDecryptionErrorFlag = XSPI_FR_IPEDERR_MASK, /*!< only support XSPI0 XSPI1*/
    kXSPI_IpCommandExecutionDoneFlag = XSPI_FR_TFF_MASK,
    kXSPI_SequenceExecutionTimeoutFlag = XSPI_INT_EN_TO_ERR_MASK, /*!< Sequence execution timeout. */
    kXSPI_FradMatchErrorFlag = XSPI_INT_EN_FRADMTCH_MASK,   /*Transaction address does not lie within address range of
                                                              any FRAD descriptor.*/
    kXSPI_FradnAccErrorFlag = XSPI_INT_EN_FRAD0ACC_MASK | XSPI_INT_EN_FRAD1ACC_MASK | XSPI_INT_EN_FRAD2ACC_MASK |
                              XSPI_INT_EN_FRAD3ACC_MASK | XSPI_INT_EN_FRAD4ACC_MASK | XSPI_INT_EN_FRAD5ACC_MASK |
                              XSPI_INT_EN_FRAD6ACC_MASK | XSPI_INT_EN_FRAD7ACC_MASK,
    kXSPI_IpsErrorFlag = XSPI_INT_EN_IPS_ERR_MASK,
    kXSPI_Tg0SfarErrorFlag = XSPI_INT_EN_TG0SFAR_MASK, 
    kXSPI_Tg1SfarErrorFlag = XSPI_INT_EN_TG1SFAR_MASK,
    kXSPI_TgnIpcrErrorFlag = XSPI_INT_EN_TG0IPCR_MASK | XSPI_INT_EN_TG1IPCR_MASK,
    kXSPI_LockRegErrorFlag = XSPI_INT_EN_LCK_ERR_IE_MASK,
    kXSPI_ArbLockTimeoutFlag = XSPI_INT_EN_ARB_TO_IE_MASK,
    kXSPI_ArbWinEventFlag = XSPI_INT_EN_ARB_WIN_IE_MASK,
} xspi_flags_t;

/*! @brief Specifies which clock to use for data alignment. */
typedef enum _xspi_sample_time_reference
{
    kXSPI_InternalReferenceClock = 0x0U,  /*!< Data aligned with the posedge of internal reference clock of XSPI*/
    kXSPI_2xFlashHalfClock = 0x1U,        /*!< Data aligned with 2x serial flash memory half clock*/
} xspi_sample_time_reference_t;

/*! @brief Target group. */
typedef enum _xspi_target_group
{
    kXSPI_TargetGroup0 = 0x0U,          /*!< Target groupe queue 0*/
    kXSPI_TargetGroup1 = 0x1U,          /*!< Target groupe queue 1*/
} xspi_target_group_t;

/*! @brief sample delay with reference edge*/
typedef enum _xspi_delay_with_reference_edge
{
    kXSPI_SameWithDqs = 0x0U,          /*!< The same with DQS*/
    kXSPI_HalfCycleEarlyDqs = 0x1U,    /*!< Half cycle early with DQS*/
} xspi_delay_with_reference_edge_t;

/*! @brief XSPI defines the secure attribute selection criteria for entry into descriptor queue...*/
typedef enum _xspi_secure_attribute
{
    kXSPI_AttributeMasterNonsecureOnly        = 0x1U, /*!< Allow the bus attribute for this master to
                                                           non-secure only*/
    kXSPI_AttributeMasterSecureOnly           = 0x2U, /*!< Allow the bus attribute for this master to secure only*/
    kXSPI_AttributeMasterNonsecureSecureBoth  = 0x3U, /*!< Allow the bus master's attribute: Both secure and
                                                           non-secure*/
} xspi_secure_attribute_t;

/*! @brief Descriptor lock. */
typedef enum _xspi_descriptor_lock
{
    kXSPI_DescriptorLockDisabled               = 0x0U, /*!< Descriptor registers can be written by any master*/
    kXSPI_DescriptorLockEnabledTillHardReset   = 0x1U, /*!< FRAD descriptor is read-only till next hard reset.*/
    kXSPI_DescriptorLockEnabledExceptMasterId  = 0x2U, /*!< Descriptors are read-only. MDnACP fields can be programmed only
                                                         by the master with ID matching the MID.*/
    kXSPI_DescriptorLockEnabled                = 0x3U, /*!< Descriptor registers are read-only.*/
} xspi_descriptor_lock_t;

/*! @brief Exclusive access lock. */
typedef enum _xspi_exclusive_access_lock
{
    kXSPI_ExclusiveAccessLockDisabled          = 0x0U, /*!< Write permissions available for all masters
                                                         based on their MDxACP evaluation.*/
    kXSPI_ExclusiveAccessLockEnabled           = 0x2U, /*!< Write permissions revoked for all domains. Any write
                                                       transaction coming to this flash address will result in an error..*/
    kXSPI_ExclusiveAccessLockExceptMasterId    = 0x3U, /*!< Write permissions are revoked for all masters except the master ID specified by FRADn_WORD2[EALO] fields.*/
} xspi_exclusive_access_lock_t;

/*! @brief XSPI AHB access towards flash is broken if this AHB alignment boundary is crossed.
 This field is only supported XSPI0 XSPI1.*/
typedef enum _xspi_ahb_alignment
{
    kXSPI_AhbAlignmentNoLimit   = 0x0U,            /*!< no limit*/
    kXSPI_AhbAlignment256BLimit = 0x1U,            /*!< 256B limit*/
    kXSPI_AhbAlignment512BLimit = 0x2U,            /*!< 512B limit*/
    kXSPI_AhbAlignment1KBLimit  = 0x3U,            /*!< 1KB limit*/
} xspi_ahb_alignment_t;

/*! @brief XSPI AHB Split Size These bits are used to control the split size when split function is enabled BFGENCR[SPLITEN] is '1'.*/
typedef enum _xspi_ahb_split_size
{
    kXSPI_AhbSplitSize8b  = 0x0U,                 /*!< split size is 8 bytes*/
    kXSPI_AhbSplitSize16b = 0x1U,                 /*!< split size is 16 bytes*/
    kXSPI_AhbSplitSize32b = 0x2U,                 /*!< split size is 32 bytes*/
    kXSPI_AhbSplitSize64b = 0x3U,                 /*!< split size is 64 bytes*/
} xspi_ahb_split_size_t;

/*! @brief Byte ordering endianness. */
typedef enum _xspi_byte_order
{
    kXSPI_64BitBE  =  0x0U,                      /*!< 64 bit big endian*/
    kXSPI_32BitLE  =  0x1U,                      /*!< 32 bit little endian*/
    kXSPI_32BitBE  =  0x2U,                      /*!< 32 bit big endian*/
    kXSPI_64BitLE  =  0x3U,                      /*!< 64 bit little endian*/
} xspi_byte_order_t;

/*! @brief Subbuffer division factor. */
typedef enum _xspi_subbuffer_division_factor
{
    kXSPI_Division0  = 0x0U,                     /*!< division factor 0*/
    kXSPI_Division2  = 0x1U,                     /*!< division factor 2 subbuffer size/2*/
    kXSPI_Division4  = 0x2U,                     /*!< division factor 4 subbuffer size/4*/
    kXSPI_Division8  = 0x3U,                     /*!< division factor 8 subbuffer size/8*/
    kXSPI_Division16 = 0x4U,                     /*!< division factor 16 subbuffer size/16*/
} xspi_subbuffer_division_factor_t;

/*! @brief XSPI sample clock source selection for Flash Reading.*/
typedef enum _xspi_read_sample_clock
{
    kXSPI_ReadSampleClkLoopbackFromDqsPad = 0x2U,      /*!< Dummy Read strobe generated by FlexSPI Controller
                                                               and loopback from DQS pad. */
    kXSPI_ReadSampleClkExternalInputFromDqsPad = 0x3U, /*!< Flash provided Read strobe and input from DQS pad. */
} xspi_read_sample_clock_t;

/*! @brief Command type. */
typedef enum _xspi_command_type
{
    kXSPI_Command, /*!< XSPI operation: Only command, both TX and Rx buffer are ignored. */
    kXSPI_Config,  /*!< XSPI operation: Configure device mode, the TX fifo size is fixed in LUT. */
    kXSPI_Read,    /*!< XSPI operation: Read, only Rx Buffer is effective. */
    kXSPI_Write,   /*!< XSPI operation: Read, only Tx Buffer is effective. */
} xspi_command_type_t;

/*! @brief Dll Mode. */
typedef enum _xspi_dll_mode
{
    kXSPI_AutoUpdateMode,    /*!< Dll mode : Auto update mode. */
    kXSPI_BypassMode,        /*!< Dll mode : Bypass mode. */
} xspi_dll_mode_t;

/*! @brief MDAD configuration. */
typedef struct _xspi_mdad_config_t
{
    bool assignIsValid;                      /*!< Indicates whether MDAD Descriptor for the target groupn is valid*/
    bool enableDescriptorLock;               /*!< This field provides a means to make the MDAD descriptor read-only.
                                               Once written '1' this field will remain '1' until hard reset*/
    uint8_t maskType;                        /*!< 0b - ANDed mask 1b - ORed mask*/
    uint8_t mask;                            /*!< Defines the 6-bit mask value for the ID-Match comparison*/
    uint8_t masterIdReference;               /*!< Specifies the reference value of the Master-ID for MID-comparison*/
    xspi_secure_attribute_t secureAttribute;   /*!< Defines the secure attribute selection criteria for entry into
                                               descriptor queue*/
} xspi_mdad_config_t;

/*! @brief FRAD configuration. */
typedef struct _xspi_frad_config_t
{
    uint32_t startAddress;        /*!< Specifies the specific flash memory region starting address*/
    uint32_t endAddress;          /*!< Specifies the specific flash memory region end address*/
    uint8_t excAccLockOwer;       /*!< When FRADn_WORD3[EAL] = 11, this field indicates the Master ID that owns the
                                    exclusive access lock*/
    uint8_t tg0MasterAccess;      /*!< This field define the access restrictions for respective Master Domain
                                    corresponding to this FRAD region. Access permissions are decided  based on secure
                                    an privilege attributes of current transaction. Read access is not restricted.*/
    uint8_t tg1MasterAccess;
    bool assignIsValid;           /*!< This field indicates whether the FRAD Descriptor for a specific flash region
                                    is valid.*/
    xspi_descriptor_lock_t  descriptorLock;            /*!< This field enables masking of accidental write on FRAD registers.
                                                       Lock is enabled/disabled by Secure/ Privileged master.*/
    xspi_exclusive_access_lock_t exclusiveAccessLock;  /*!< This field provides exclusive write lock over a
                                                       FRAD region based on MDnACP.*/
} xspi_frad_config_t;

/*! @brief ahb buffer configuration. */
typedef struct _xspi_ahbBuffer_config
{
    uint8_t masterIndex;          /*!< AHB Master ID the AHB RX Buffer is assigned. */
    uint16_t transferSize;        /*!< AHB transfer size in byte. */
    uint16_t topIndex;            /*!< AHB Buffer specifies the top index for buffer 0, which defines its size.*/
    union
    {
        bool enablePriority;      /*!< High Priority Enable, it can be written '1' only when OTFAD is disabled. */
        bool enableAllMaster;     /*!< When set, buffer3 acts as an all-master buffer.buff[i] routed to buffer3*/
    } enaPri;
    bool enableSubdivision;       /*!< AHB Buffer Sub-division Enable */
	xspi_subbuffer_division_factor_t  subBuffer[3];   /*!< subBuffer division factor*/
} xspi_ahbBuffer_config_t;

/*! @brief XSPI configuration structure. */
typedef struct _xspi_config
{
    xspi_read_sample_clock_t rxSampleClock;  /*!< Sample Clock source selection for Flash Reading.*/
    xspi_byte_order_t byteOrder;             /*!< Byte ordering endianness*/
    bool enableCknPad;                       /*!< Enable/disable clock on differential CKN pad for flash memory A.*/
    bool enableDoze;                         /*!< Enable/disable doze mode support. */
    bool enableDqs;                          /*!< Enable/disable DQS*/
    bool enableDqsLatency;                   /*!< Enable/disable DQS latency*/
    bool enableDataLearn;                    /*!< Enable/disable Data Learning*/
    bool enableDqsOut;                       /*!< Enable/disable DQS Out only for HyperRAM*/
    bool enableVariableLat;                  /*!< Enable/disable variable latency feature only for HyperRAM*/

    bool enableGloAccCtr;                    /*!< Enable/disable Global access ctrol*/
    struct
    {
        xspi_mdad_config_t tgmdad[2];
        bool enableMdad;                     /*!< Enable/disable first MDAD check*/
    } mdadConfig;
    struct
    {
        xspi_frad_config_t frad[8];    
        bool enableFrad;                     /*!< Enable/disable second FRAD check only for write commond*/
    } fradConfig;
    uint8_t txWatermark;                     /*!< XSPI IP transmit watermark value. */
    uint8_t rxWatermark;                     /*!< XSPI receive watermark value. */
    struct
    {
        xspi_ahbBuffer_config_t buffer[4];   /*!< AHB buffer size. */

        bool enableAHBPrefetch;              /*!< Enable/disable AHB read prefetch feature, when enabled, XSPI
                                               will fetch more data than current AHB burst. */
        bool enableAHBSlite;                 /*!< When this bit is set to 1, all AHB read/write burst will be spliT
                                               into small transactions at Flash SPI bus side.*/
        bool enableAHBWaitWrite;             /*!< <enable> After the first write transaction
                                               next AHB write transfer is kept waiting by keeping AHB HREADY low.*/
        bool enableAHBWaitRead;              /*!< <disable> subsequent reads  <enable> After the first write
                                               transaction, next AHB read transfer (in case of buffer miss)
                                               is kept waiting by keeping AHB HREADY low.*/
        xspi_ahb_split_size_t ahbSplitSize;  /*!< the split size when split function is enabled BFGENCR[SPLITEN]
                                               is '1'.*/
        xspi_ahb_alignment_t   ahbAlignment; /*!< AHB access towards flash is broken if this AHB alignment boundary
                                               is crossed, only support XSPI0 XSPI1*/
    } ahbConfig;
} xspi_config_t;

/*! @brief External device configuration items. */
typedef struct _xspi_device_config
{
    bool enableX16Mode;                         /*!< Enable X16 Mode Only for default 16 I/O bus PSRAM. */
    bool enableWriteFlush;                      /*!< Enable write flush. */
    bool enableDdr;                             /*!< Enable DDR mode. */
    uint32_t xspiRootClk;                       /*!< XSPI serial root clock. */
    uint32_t flashA1Size;                       /*!< Flash A1 size in KByte. */
    uint32_t flashA2Size;                       /*!< Flash A2 size in KByte. */
    uint8_t CSHoldTime;                         /*!< CS line hold time. */
    uint8_t CSSetupTime;                        /*!< CS line setup time. */
    xspi_sample_time_reference_t sampleTimeRef; /*data aligned clock in hold time only support DDR mode*/
    uint8_t columnSpace;                        /*!< Column space size. */
    uint8_t PageProgramBoundary;                /*!< page program boundary size should be programmed in Log2 (size in
                                                     bytes) format. The default is 8 for 256-byte page program size.*/
    bool enableWordAddress;                     /*!< If enable word address.*/
    bool enable4BWordAddress;                   /*!< If enable 4B word address.*/
    uint8_t AWRSeqIndex;                        /*!< Sequence ID for AHB write command. */
    uint8_t ARDSeqIndex;                        /*!< Sequence ID for AHB read command. */
    uint8_t ARDSRSeqIndex;                      /*!< Sequence ID for AHB reading the status register from
                                                     flash in case of page program wait period.*/
    struct
    {
        xspi_dll_mode_t dllMode;              /*!< Dll mode auto update mode or bypass mode */
        bool enableCdl8;                      /*!< Enable CDL8. */
        uint8_t dllReferenceCounter;          /*!< DLL reference counter. */
        uint8_t dllResolution;                /*!< DLL resolution. */
        uint8_t slaveFineOffset;              /*!< Fine offset delay elements in incoming DQS*/
        uint8_t slaveDelayOffset;             /*!< T/16 offset delay elements in incoming DQS*/
        uint8_t slaveDelayCoarse;             /*!< Delay elements in each delay tap The field is used to over DLL
                                                values and works when the value of SLV_DLL_BYPASS is 1.*/
    } dllConfig;

    struct
    {
        uint8_t tapNumber;                              /*!< Selects the n tap provided by slave delay chain
                                                             for flash memory A*/
        xspi_delay_with_reference_edge_t delayWithRef;  /*!< This field selects the delay in accordance with the
                                                             reference edge for the valid sample point.*/
        bool enableAtInvertedClock;                     /*!< This field selects the edge of the sampling clock for
                                                             full-speed commands.*/
    } smprConfig;
} xspi_device_config_t;

/*! @brief Transfer structure for XSPI. */
typedef struct _xspi_transfer
{
    uint32_t deviceAddress;         /*!< Operation device address. */
    xspi_command_type_t cmdType;    /*!< Execution command type. */
    uint8_t seqIndex;               /*!< Sequence ID for command. */
    uint32_t *data;                 /*!< Data buffer. */
    size_t dataSize;                /*!< Data size in bytes. */
    xspi_target_group_t targetGroup;  /*!< Target group. include targetGroup[0] targetGroup[1]*/
} xspi_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _xspi_handle xspi_handle_t;

/*! @brief XSPI transfer callback function. */
typedef void (*xspi_transfer_callback_t)(XSPI_Type *base,
                                            xspi_handle_t *handle,
                                            status_t status,
                                            void *userData);

/*! @brief Transfer handle structure for XSPI. */
struct _xspi_handle
{
    uint32_t state;                                 /*!< Internal state for XSPI transfer */
    uint8_t *data;                                  /*!< Data buffer. */
    size_t dataSize;                                /*!< Remaining Data size in bytes. */
    size_t transferTotalSize;                       /*!< Total Data size in bytes. */
    xspi_transfer_callback_t completionCallback;    /*!< Callback for users while transfer finish or error occurred */
    void *userData;                                 /*!< XSPI callback function parameter.*/
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus. */

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Get the instance number for XSPI.
 *
 * @param base XSPI base pointer.
 */
uint32_t XSPI_GetInstance(XSPI_Type *base);

/*!
 * @brief Check and clear IP command execution errors.
 *
 * @param base XSPI base pointer.
 * @param status interrupt status.
 */
status_t XSPI_CheckAndClearError(XSPI_Type *base, uint32_t status);

/*!
 * @brief Initializes the XSPI module and internal state.
 *
 * This function configures the XSPI with the
 * input configure parameters. Users should call this function before any XSPI operations.
 *
 * @param base XSPI peripheral base address.
 * @param config XSPI configure structure.
 */
void XSPI_Init(XSPI_Type *base, const xspi_config_t *config);

/*!
 * @brief Gets default settings for XSPI.
 * @param base XSPI peripheral base address.
 * @param config XSPI configuration structure.
 */
void XSPI_GetDefaultConfig(XSPI_Type *base, xspi_config_t *config, xspi_device_config_t *devconfig);

/*!
 * @brief Deinitializes the XSPI module.
 *
 * Clears the XSPI state and  XSPI module registers.
 * @param base XSPI peripheral base address.
 * @param devconfig Flash configuration parameters.
 */
void XSPI_Deinit(XSPI_Type *base);

/*!
 * @brief Configures the connected device parameter.
 *
 * This function configures the connected device relevant parameters, such as the size, command, and so on.
 * The flash configuration value cannot have a default value. The user needs to configure it according to the
 * connected device.
 *
 * @param base XSPI peripheral base address.
 * @param devconfig Flash configuration parameters.
 */
void XSPI_SetFlashConfig(XSPI_Type *base, xspi_device_config_t *devconfig);

/*!
 * @brief Software reset for the XSPI AND AHB.
 *
 * This function sets the software reset flags for both AHB and buffer domain and
 * resets both AHB buffer and also IP FIFOs.
 *
 * @param base XSPI peripheral base address.
 */
static inline void XSPI_SoftwareReset(XSPI_Type *base)
{
    base->MCR |= XSPI_MCR_SWRSTSD_MASK | XSPI_MCR_SWRSTHD_MASK;
    for(uint32_t i = 0;i < 6;i++)
    {
        __NOP();
    }
    base->MCR &= ~(XSPI_MCR_SWRSTSD_MASK | XSPI_MCR_SWRSTHD_MASK);
    base->MCR |= XSPI_MCR_IPS_TG_RST_MASK | XSPI_MCR_MDIS_MASK;
    base->MCR &= ~XSPI_MCR_ISD3FA_MASK;
    base->MCR &= ~XSPI_MCR_MDIS_MASK;
    base->MCR |= XSPI_MCR_MDIS_MASK;
    base->MCR |= XSPI_MCR_ISD3FA_MASK;
    base->MCR &= ~ XSPI_MCR_MDIS_MASK;
}

/*!
 * @brief Enables XSPI X16 mode. Only support XSPI1 and XSPI2.
 *
 * @param base XSPI peripheral base address.
 */
static inline void XSPI_EnableX16Mode(XSPI_Type *base)
{
    base->MCR |= XSPI_MCR_MDIS_MASK;
    base->MCR |= XSPI_MCR_X16_EN_MASK;
    base->MCR &= ~XSPI_MCR_MDIS_MASK;
}

/*!
 * @brief Enables or disables the XSPI module.
 *
 * @param base XSPI peripheral base address.
 * @param enable True means enable XSPI, false means disable.
 */
static inline void XSPI_Enable(XSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->MCR &= ~XSPI_MCR_MDIS_MASK;
    }
    else
    {
        base->MCR |= XSPI_MCR_MDIS_MASK;
    }
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */
/*!
 * @brief Enables the XSPI interrupts.
 *
 * @param base XSPI peripheral base address.
 * @param mask XSPI interrupt source.
 */

static inline void XSPI_EnableInterrupts(XSPI_Type *base, uint32_t mask)
{
    base->RSER |= mask;
}

/*!
 * @brief Disable the XSPI interrupts.
 *
 * @param base XSPI peripheral base address.
 * @param mask XSPI interrupt source.
 */
static inline void XSPI_DisableInterrupts(XSPI_Type *base, uint32_t mask)
{
    base->RSER &= ~mask;
}

/* @} */

/*! @name DMA control */
/*@{*/

/*!
 * @brief Enables or disables XSPI IP Tx FIFO DMA requests.
 *
 * @param base XSPI peripheral base address.
 * @param enable Enable flag for transmit DMA request. Pass true for enable, false for disable.
 */
static inline void XSPI_EnableTxDMA(XSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->RSER |= XSPI_RSER_TBFDE_MASK;
    }
    else
    {
        base->RSER &= ~XSPI_RSER_TBFDE_MASK;
    }
}

/*!
 * @brief Enables or disables XSPI IP Rx FIFO DMA requests.
 *
 * @param base XSPI peripheral base address.
 * @param enable Enable flag for receive DMA request. Pass true for enable, false for disable.
 */
static inline void XSPI_EnableRxDMA(XSPI_Type *base, bool enable)
{
    if (enable)
    {
        base->RSER |= XSPI_RSER_RBDDE_MASK;
    }
    else
    {
        base->RSER &= ~XSPI_RSER_RBDDE_MASK;
    }
}

/*!
 * @brief Gets XSPI IP tx fifo address for DMA transfer.
 *
 * @param base XSPI peripheral base address.
 * @retval The tx fifo address.
 */
static inline uint32_t XSPI_GetTxFifoAddress(XSPI_Type *base)
{
    return (uint32_t)&base->TBDR;
}

/*!
 * @brief Gets XSPI IP rx fifo address for DMA transfer.
 *
 * @param base XSPI peripheral base address.
 * @retval The rx fifo address.
 */
static inline uint32_t XSPI_GetRxFifoAddress(XSPI_Type *base)
{
    return (uint32_t)&base->RBDR[0];
}

/*@}*/

/*! @name FIFO control */
/*@{*/

/*! @brief Clears the XSPI IP FIFO logic.
 *
 * @param base XSPI peripheral base address.
 * @param txFifo Pass true to reset TX FIFO.
 * @param rxFifo Pass true to reset RX FIFO.
 */
static inline void XSPI_ResetFifos(XSPI_Type *base, bool txFifo, bool rxFifo)
{
    if (txFifo)
    {
        base->MCR |= XSPI_MCR_CLR_TXF_MASK;
    }
    if (rxFifo)
    {
        base->MCR |= XSPI_MCR_CLR_RXF_MASK;
    }
}
/*@}*/

/*!
 * @name Status
 * @{
 */
/*!
 * @brief Get the XSPI interrupt status flags.
 *
 * @param base XSPI peripheral base address.
 * @retval interrupt status flag, use status flag to AND #xspi_flags_t could get the related status.
 */
static inline uint32_t XSPI_GetInterruptStatusFlags(XSPI_Type *base)
{
    return base->ERRSTAT;
}

/*! @brief Returns whether the bus is idle.
 *
 * @param base XSPI peripheral base address.
 * @retval true Bus is idle.
 * @retval false Bus is busy.
 */
static inline bool XSPI_GetBusIdleStatus(XSPI_Type *base)
{
    return (1U != (base->SR & XSPI_SR_BUSY_MASK));
}
/*@}*/

/*!
 * @name Bus Operations
 * @{
 */

/*! @brief Update read sample clock source
 *
 * @param base XSPI peripheral base address.
 * @param clockSource clockSource of type #flexspi_read_sample_clock_t
 */
void XSPI_UpdateRxSampleClock(XSPI_Type *base, xspi_read_sample_clock_t clockSource);

/*! @brief Updates the LUT table.
 *
 * @param base XSPI peripheral base address.
 * @param index From which index start to update. It could be any index of the LUT table, which
 * also allows user to update command content inside a command. Each command consists of up to
 * 10 instructions and occupy 5*32-bit memory.
 * @param cmd Command sequence array.
 * @param count Number of sequences.
 */
void XSPI_UpdateLUT(XSPI_Type *base, uint8_t index, const uint32_t *cmd, uint8_t count);

/*!
 * @brief Writes data into FIFO.
 *
 * @param base XSPI peripheral base address
 * @param data The data bytes to send
 */
static inline void XSPI_WriteData(XSPI_Type *base, uint32_t data)
{
    base->TBDR = data;
}

/*!
 * @brief Receives data from data FIFO.
 *
 * @param base XSPI peripheral base address
 * @param fifoIndex Source fifo index.
 * @return The data in the FIFO.
 */
static inline uint32_t XSPI_ReadData(XSPI_Type *base, uint8_t fifoIndex)
{
    return base->RBDR[fifoIndex];
}

/*!
 * @brief Sends a buffer of data bytes using blocking method.
 * @note This function blocks via polling until all bytes have been sent.
 * @param base XSPI peripheral base address
 * @param buffer The data bytes to send
 * @param size The number of data bytes to send
 * @retval kStatus_Success write success without error
 * @retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_XSPI_IpCommandSequenceError IP command sequence error detected
 * @retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_WriteBlocking(XSPI_Type *base, uint8_t *buffer, size_t size);

/*!
 * @brief Receives a buffer of data bytes using a blocking method.
 * @note This function blocks via polling until all bytes have been sent.
 * @param base XSPI peripheral base address
 * @param buffer The data bytes to send
 * @param size The number of data bytes to receive
 * @retval kStatus_Success read success without error
 * @retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_XSPI_IpCommandSequenceError IP command sequencen error detected
 * @retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_ReadBlocking(XSPI_Type *base, uint8_t *buffer, size_t size);

/*!
 * @brief Execute command to transfer a buffer data bytes using a blocking method.
 * @param base XSPI peripheral base address
 * @param xfer pointer to the transfer structure.
 * @retval kStatus_Success command transfer success without error
 * @retval kStatus_XSPI_SequenceExecutionTimeout sequence execution timeout
 * @retval kStatus_XSPI_IpCommandSequenceError IP command sequence error detected
 * @retval kStatus_XSPI_IpCommandGrantTimeout IP command grant timeout detected
 */
status_t XSPI_TransferBlocking(XSPI_Type *base, xspi_transfer_t *xfer);
/*! @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the XSPI handle which is used in transactional functions.
 *
 * @param base XSPI peripheral base address.
 * @param handle pointer to xspi_handle_t structure to store the transfer state.
 * @param callback pointer to user callback function.
 * @param userData user parameter passed to the callback function.
 */
void XSPI_TransferCreateHandle(XSPI_Type *base,
                                  xspi_handle_t *handle,
                                  xspi_transfer_callback_t callback,
                                  void *userData);

/*!
 * @brief Performs a interrupt non-blocking transfer on the XSPI bus.
 *
 * @note Calling the API returns immediately after transfer initiates. The user needs
 * to call XSPI_GetTransferCount to poll the transfer status to check whether
 * the transfer is finished. If the return status is not kStatus_XSPI_Busy, the transfer
 * is finished. For XSPI_Read, the dataSize should be multiple of rx watermark level, or
 * XSPI could not read data properly.
 *
 * @param base XSPI peripheral base address.
 * @param handle pointer to xspi_handle_t structure which stores the transfer state.
 * @param xfer pointer to xspi_transfer_t structure.
 * @retval kStatus_Success Successfully start the data transmission.
 * @retval kStatus_XSPI_Busy Previous transmission still not finished.
 */
status_t XSPI_TransferNonBlocking(XSPI_Type *base, xspi_handle_t *handle, xspi_transfer_t *xfer);

/*!
 * @brief Gets the master transfer status during a interrupt non-blocking transfer.
 *
 * @param base XSPI peripheral base address.
 * @param handle pointer to xspi_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 * @retval kStatus_InvalidArgument count is Invalid.
 * @retval kStatus_Success Successfully return the count.
 */
status_t XSPI_TransferGetCount(XSPI_Type *base, xspi_handle_t *handle, size_t *count);

/*!
 * @brief Aborts an interrupt non-blocking transfer early.
 *
 * @note This API can be called at any time when an interrupt non-blocking transfer initiates
 * to abort the transfer early.
 *
 * @param base XSPI peripheral base address.
 * @param handle pointer to xspi_handle_t structure which stores the transfer state
 */
void XSPI_TransferAbort(XSPI_Type *base, xspi_handle_t *handle);
/*! @} */

#if defined(__cplusplus)
}
#endif /*_cplusplus. */
/*@}*/

#endif /* FSL_XSPI_H_ */
