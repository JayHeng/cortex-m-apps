/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "flash_config.h"
#include "board.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(BOOT_HEADER_ENABLE) && (BOOT_HEADER_ENABLE == 1)
#if defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".flash_conf")))
#elif defined(__ICCARM__)
#pragma location = ".flash_conf"
#endif

const qspi_boot_config_t qspi_config = {
    .tag = FLASH_CONFIG_BLOCK_TAG,
    .version = FLASH_CONFIG_BLOCK_VERSION,
    .readSamplingOption = 3,
    .csHoldTime = 3,
    .csSetupTime = 3,
    .columAddressWidth = 0,
    .deviceModeCfgEnable = 1,
    .deviceModeType = 2,
    .waitTimeCfgCommands = 1,
    .deviceModeSeq = 0x0601,
    .deviceModeArg = 2,
    .configCmdEnable = 0,
    .configModeType = {0},
    .configCmdSeqs = {0},
    .configCmdArgs = {0},
    .controllerMiscOption = 0x50,
    .deviceType = 0,
    .sflashPadType = QSPI_MODE_OF_OP_OCTAL,
    .serialClkFreq = QSPI_SERIAL_CLOCK_FREQ_48MHZ,
    .flashA1Size = BOARD_FLASH_SIZE,
    .flashA2Size = 0,
    .flashB1Size = 0,
    .flashB2Size = 0,
    .csPadSettingOverride = 0,
    .sclkPadSettingOverride = 0,
    .dataPadSettingOverride = 0,
    .dqsPadSettingOverride = 0,
    .timeoutInMs = 0,
    .coarseTuning = 0,
    .fineTuning = 0x19,
    .samplePoint = 0,
    .dataHoldTime = 1,
    .busyOffset = 0,
    .busyBitPolarity = 0,
    .lut = {[0] = 0x471147EE, [1] = 0x0F042B20, [2] = 0x3B04, [4] = 0x1C040405, [8] = 0x47FA4705, [9] = 0x47004700,
            [10] = 0x47004700, [11] = 0x3B04, [12] = 0x0406, [16] = 0x47F94706, [20] = 0x47DE4721, [21] = 0x2B20,
            [24] = 0x04000472, [25] = 0x04000400, [26] = 0x20010400, [32] = 0x472347DC, [33] = 0x2B20,
            [36] = 0x47ED4712, [37] = 0x3F042B20, [44] = 0x479F4760},
    .pageSize = 0x100,
    .sectorSize = 0x1000,
    .ipcmdSerialClkFreq = 1,
    .isUniformBlockSize = 0,
    .blockSize = 0x10000,
    .isNonBlockingMode = 0,
};
#endif /* BOOT_HEADER_ENABLE */