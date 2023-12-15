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
#include "board.h"
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
uint32_t get_core1_image_size(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
   
static void sram_unsecure(void){
//pre_main: AHB Secure control configure....
  *((uint32_t *)0x4017cc00) = 0x00060000;//clear bit18
  *((uint32_t *)0x4017cc00) = 0x00020001;
  *((uint32_t *)0x4017cc00) = 0x00010001;
  *((uint32_t *)0x4017cc04) = 0x00290000;
  *((uint32_t *)0x4017cc00) = 0x00020001;
  *((uint32_t *)0x4017cc04) = 0x00280000;
  *((uint32_t *)0x4017cc00) = 0x00000001;
  *((uint32_t *)0x4017cff8) = 0x0000865a;
  *((uint32_t *)0x4017cffc) = 0x0000865a;
  *((uint32_t *)0x40220c00) = 0x00060000;//clear bit18
  *((uint32_t *)0x40220c00) = 0x00020001;
  *((uint32_t *)0x40220c00) = 0x00010001;
  *((uint32_t *)0x40220c04) = 0x00290000;
  *((uint32_t *)0x40220c00) = 0x00020001;
  *((uint32_t *)0x40220c04) = 0x00280000;
  *((uint32_t *)0x40220c00) = 0x00000001;
  *((uint32_t *)0x40220ff8) = 0x0000865a;
  *((uint32_t *)0x40220ffc) = 0x0000865a;
  *((uint32_t *)0x4017cc00) = 0x00060000;//clear bit18
  *((uint32_t *)0x4017cc00) = 0x00020007;
  *((uint32_t *)0x4017cc00) = 0x00010007;
  *((uint32_t *)0x4017cc04) = 0x00290000;
  *((uint32_t *)0x4017cc00) = 0x00020007;
  *((uint32_t *)0x4017cc04) = 0x00280000;
  *((uint32_t *)0x4017cc00) = 0x00000007;
  //arbiter0: enable compute access ....
  *((uint32_t *)0x4017cf80) = 0xffffffff;
  *((uint32_t *)0x4017cf84) = 0xffffffff;
  *((uint32_t *)0x4017cf88) = 0xffffffff;
  *((uint32_t *)0x4017cf8c) = 0xffffffff;
  *((uint32_t *)0x4017cf90) = 0xffffffff;
  *((uint32_t *)0x40220c00) = 0x00060000;//clear bit18
  *((uint32_t *)0x40220c00) = 0x00020009;
  *((uint32_t *)0x40220c00) = 0x00010009;
  *((uint32_t *)0x40220c04) = 0x00290000;
  *((uint32_t *)0x40220c00) = 0x00020009;
  *((uint32_t *)0x40220c04) = 0x00280000;
  *((uint32_t *)0x40220c00) = 0x00000009;
  //arbiter1: enable compute access ....
  *((uint32_t *)0x40220f80) = 0xffffffff;
  *((uint32_t *)0x40220f84) = 0xffffffff;
  *((uint32_t *)0x40220f88) = 0xffffffff;
  *((uint32_t *)0x40220f8c) = 0xffffffff;
  *((uint32_t *)0x40220f90) = 0xffffffff;
  *((uint32_t *)0x40220f94) = 0xffffffff;
  *((uint32_t *)0x40220c00) = 0x00060000;//clear bit18
  *((uint32_t *)0x40220c00) = 0x00020008;
  *((uint32_t *)0x40220c00) = 0x00010008;
  *((uint32_t *)0x40220c04) = 0x00290000;
  *((uint32_t *)0x40220c00) = 0x00020008;
  *((uint32_t *)0x40220c04) = 0x00280000;
  *((uint32_t *)0x40220c00) = 0x00000008;
  //common apb: enable compute domain master access ....

  *((uint32_t *)0x40220fa0) = 0xffffffff;
  *((uint32_t *)0x40220fb0) = 0xffffffff; //Ye add: Access enable for COMPUTE domain masters to common AIPS
  *((uint32_t *)0x40220fa4) = 0xffffffff;
  *((uint32_t *)0x40220fb4) = 0xffffffff; //Ye add: Access enable for SENSE domain masters to common AIPS
  *((uint32_t *)0x40220c00) = 0x00060000;//clear bit18
  *((uint32_t *)0x40220c00) = 0x00020007;
  *((uint32_t *)0x40220c00) = 0x00010007;
  *((uint32_t *)0x40220c04) = 0x00290000;
  *((uint32_t *)0x40220c00) = 0x00020007;
  *((uint32_t *)0x40220c04) = 0x00280000;
  *((uint32_t *)0x40220c00) = 0x00000007;
  *((uint32_t *)0x40220fb0) = 0xffffffff;
  *((uint32_t *)0x40220fb4) = 0xffffffff;
  
  /* AHB_SECURE set sram0 secure level to SecurePriv */
  *((uint32_t *)0x4017c110) = 0x33333333;
  *((uint32_t *)0x4017c114) = 0x33333333;
  *((uint32_t *)0x4017c118) = 0x33333333;
  *((uint32_t *)0x4017c11c) = 0x33333333;
  
  
    /* AHB_SECURE enable compute access to sram0~sram21*/
  *((uint32_t *)0x5017cc00) = 0x00060000;//clear bit18
  *((uint32_t *)0x5017cc00) = 0x00020007;
  *((uint32_t *)0x5017cc00) = 0x00010007;
  *((uint32_t *)0x5017cc04) = 0x00290000;
  *((uint32_t *)0x5017cc00) = 0x00020007;
  *((uint32_t *)0x5017cc04) = 0x00280000;
  *((uint32_t *)0x5017cc00) = 0x00000007; 
}

static void PMC_ReleasePower()
{
#if defined(MIMXRT798S_cm33_core0_SERIES)
//    PMC0->PDRUNCFG0 &= ~0x740UL;
    /* Powerup all SRAM Partition */
    PMC0->PDRUNCFG2 = 0xC0000000;
    PMC0->PDRUNCFG3 = 0xC0000000;
    POWER_ApplyPD();
#endif
#if 0
    SLEEPCON0->RUNCFG = 0;
#else
    POWER_DisablePD(kPDRUNCFG_SHUT_RAM1_CLK);
    POWER_DisablePD(kPDRUNCFG_SHUT_SENSES_MAINCLK);
    POWER_DisablePD(kPDRUNCFG_SHUT_SENSEP_MAINCLK);
#endif
   
}

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
    sram_unsecure();
    PMC_ReleasePower();

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

    //PRINTF("Copy Secondary core image to address: 0x%x, size: %d\r\n", CORE1_BOOT_ADDRESS, core1_image_size);

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

