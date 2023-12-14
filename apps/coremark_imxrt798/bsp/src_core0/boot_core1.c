/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "clock_config.h"
#include "fsl_cache.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define CORE1_BOOT_ADDRESS 0x20580000

#if defined(__ICCARM__)
extern unsigned char core1_image_start[];
#define CORE1_IMAGE_START core1_image_start
#endif

#define CORE1_IMAGE_FLUSH_CACHE XCACHE_CleanInvalidateCacheByRange

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void APP_BootCore1(void);
#ifdef CORE1_IMAGE_COPY_TO_RAM
uint32_t get_core1_image_size(void);
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t get_core1_image_size()
{
    uint32_t image_size;
#if defined(__ICCARM__)
#pragma section = "__core1_image"
    image_size = (uint32_t)__section_end("__core1_image") - (uint32_t)&core1_image_start;
#endif
    return image_size;
}

void APP_BootCore1(void)
{
    /*Glikey write enable, GLIKEY4*/
    *((uint32_t *)0x40062c00) = 0x00060000; /* clear bit18 */
    *((uint32_t *)0x40062c00) = 0x00020001; /* index 1 */
    *((uint32_t *)0x40062c00) = 0x00010001; /* step1 */
    *((uint32_t *)0x40062c04) = 0x00290000; /* step2 */
    *((uint32_t *)0x40062c00) = 0x00020001; /* step3 */
    *((uint32_t *)0x40062c04) = 0x00280000; /* step4 */
    *((uint32_t *)0x40062c00) = 0x00000001; /* write enable */
    
    /* Boot source for Core 1 from RAM , PT18, 0x20580000. */
    SYSCON3->CPU1_NSVTOR = (0x00580000 >> 7);
    SYSCON3->CPU1_SVTOR  = (0x00580000 >> 7);

    /* Enable cpu1 clock. */
    CLOCK_EnableClock(kCLOCK_Cpu1);

    /* Clear reset*/
    RESET_ClearPeripheralReset(kCPU1_RST_SHIFT_RSTn);

    /* Release cpu wait*/
    SYSCON3->CPU_STATUS &= ~SYSCON3_CPU_STATUS_CPU_WAIT_MASK;
}

/*!
 * @brief Function to copy core1 image to execution address.
 */
void APP_CopyCore1Image(void)
{
    /* Calculate size of the image  - not required on MCUXpresso IDE. MCUXpresso copies the secondary core
       image to the target memory during startup automatically */
    uint32_t core1_image_size = get_core1_image_size();

    PRINTF("Copy Secondary core image to address: 0x%x, size: %d\r\n", CORE1_BOOT_ADDRESS, core1_image_size);

    /* Copy Secondary core application from FLASH to the target memory. */
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)CORE1_BOOT_ADDRESS, core1_image_size);
#endif
#ifdef CORE1_IMAGE_FLUSH_CACHE
    CORE1_IMAGE_FLUSH_CACHE(CORE1_BOOT_ADDRESS, core1_image_size);
#endif
    memcpy((void *)CORE1_BOOT_ADDRESS, (void *)CORE1_IMAGE_START, core1_image_size);
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanInvalidateDCache_by_Addr((void *)CORE1_BOOT_ADDRESS, core1_image_size);
#endif
#ifdef CORE1_IMAGE_FLUSH_CACHE
    CORE1_IMAGE_FLUSH_CACHE(CORE1_BOOT_ADDRESS, core1_image_size);
#endif
}

