/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "dhry_portme.h"
/*${header:end}*/

typedef struct _stack_frame
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} stack_frame_t;

static stack_frame_t s_stackFrame;

typedef struct _scb_flag
{
    uint32_t mmfsr;
    uint32_t abfsr;
    uint32_t bfsr;
    uint32_t ufsr;
} scb_flag_t;

static scb_flag_t s_scbFlag;

void hardfault_handler(stack_frame_t *frame)
{
    // Show call stack of HardFault
    //PRINTF("pc = %x\n", frame->pc);
    //PRINTF("psr = %x\n", frame->psr);
    //PRINTF("r0 = %x\n", frame->r0);
    //PRINTF("r1 = %x\n", frame->r1);
    //PRINTF("r2 = %x\n", frame->r2);
    //PRINTF("r3 = %x\n", frame->r3);
    //PRINTF("r12 = %x\n", frame->r12);
    //PRINTF("lr = %x\n", frame->lr);
    memcpy(&s_stackFrame, frame, sizeof(stack_frame_t));
    memset(&s_scbFlag, 0, sizeof(scb_flag_t));

#if (__CORTEX_M >= 3)
    // Show Fault Flags
    //PRINTF("SCB->HFSR=%x\n", SCB->HFSR);

    // If it is a force HardFault, usually, it means that the HardFault was escalated from other core exceptions
    if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
    {
        //PRINTF("SCB->CFSR=%x\n", SCB->CFSR);
        if (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk)
        {
            //PRINTF("HardFault was escalated from MemUsageFault\n");
            s_scbFlag.mmfsr = SCB->CFSR & 0xFF;
            s_scbFlag.abfsr = SCB->ABFSR;
            //MemManage_Handler();
        }
        if (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk)
        {
            //PRINTF("HardFault was escalated from BusFault\n");
            s_scbFlag.bfsr = (SCB->CFSR >> 8) & 0xFF;
            //BusFault_Handler();
        }
        if (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk)
        {
            //PRINTF("HardFault was escalated from UsageFault\n");
            s_scbFlag.ufsr = (SCB->CFSR >> 16) & 0xFFFF;
            //UsageFault_Handler();
        }
    }
#endif // #if (__CORTEX_M >= 3)

    while (1);
}

/* Functions below only apply to MCUs that belongs to ARM Cortex-M3 or later family */
#if (__CORTEX_M >= 3)
//! @brief MPU Fault Handler
void MemManage_Handler(void)
{
    uint32_t mmfsr = SCB->CFSR & 0xFF;
    PRINTF("SCB->CFSR[MMFSR]=%x\n", mmfsr);

    if (mmfsr & 0x01)
    {
        PRINTF("Instruction access violation flag is asserted\n");
    }
    if (mmfsr & 0x02)
    {
        PRINTF("IDtat access violation flag is asserted\n");
    }
    if (mmfsr & 0x04)
    {
        PRINTF("Instruction access violation flag is asserted\n");
    }
    if (mmfsr & 0x80)
    {
        PRINTF("SCB->MMFAR=%x\n", SCB->MMFAR);
    }

#if (__CORTEX_M == 7)
    uint32_t abfsr = SCB->ABFSR;
    PRINTF("SCB->ABFSR=%x\n", abfsr);

    if (abfsr & 0x01)
    {
        PRINTF("Async fault on ITCM interface\n");
    }
    if (abfsr & 0x02)
    {
        PRINTF("Async fault on DTCM interface\n");
    }
    if (abfsr & 0x04)
    {
        PRINTF("Async fault on AHBP interface\n");
    }
    if (abfsr & 0x08)
    {
        PRINTF("Async fault on AXIM interface\n");
    }
    if (abfsr & 0x10)
    {
        PRINTF("Async fault on EPPB interface\n");
    }
#endif

    if (SCB->SHCSR & SCB_SHCSR_MEMFAULTACT_Msk)
    {
        while (1);
    }
}

//! @brief Bus Fault Handler
void BusFault_Handler(void)
{
    uint32_t bfsr = (SCB->CFSR >> 8) & 0xFF;
    PRINTF("SCB->CFSR[BFSR]=%x\n", bfsr);

    if (bfsr & 0x01)
    {
        PRINTF("Instruction Bus Error\n");
    }
    if (bfsr & 0x02)
    {
        PRINTF("Precise Data Bus Error\n");
    }
    if (bfsr & 0x04)
    {
        PRINTF("Imprecise Data Bus Error\n");
    }
    if (bfsr & 0x80)
    {
        PRINTF("SCB->BFAR=%x\n", SCB->BFAR);
    }

    if (SCB->SHCSR & SCB_SHCSR_BUSFAULTACT_Msk)
    {
        while (1);
    }
}

//! @brief Usage Fault Handler
void UsageFault_Handler(void)
{
    uint32_t ufsr = (SCB->CFSR >> 16) & 0xFFFF;
    PRINTF("SCB->CFSR[UFSR]=%x\n", ufsr);

    if (ufsr & 0x01)
    {
        PRINTF("Undefined instruction\n");
    }
    if (ufsr & 0x02)
    {
        PRINTF("Invalid State\n");
    }
    if (ufsr & 0x04)
    {
        PRINTF("Invalid PC load UsageFault\n");
    }
    if (ufsr & 0x08)
    {
        PRINTF("No coprocessor\n");
    }
    if (ufsr & 0x100)
    {
        PRINTF("Unaliged access UsageFault\n");
    }

    if (SCB->SHCSR & SCB_SHCSR_USGFAULTACT_Msk)
    {
        while (1);
    }
}
#endif // #if (__CORTEX_M >= 3)

