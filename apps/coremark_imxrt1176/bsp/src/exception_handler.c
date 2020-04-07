/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "core_portme.h"
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
    //ee_printf("pc = %x\n", frame->pc);
    //ee_printf("psr = %x\n", frame->psr);
    //ee_printf("r0 = %x\n", frame->r0);
    //ee_printf("r1 = %x\n", frame->r1);
    //ee_printf("r2 = %x\n", frame->r2);
    //ee_printf("r3 = %x\n", frame->r3);
    //ee_printf("r12 = %x\n", frame->r12);
    //ee_printf("lr = %x\n", frame->lr);
    memcpy(&s_stackFrame, frame, sizeof(stack_frame_t));
    memset(&s_scbFlag, 0, sizeof(scb_flag_t));

#if (__CORTEX_M >= 3)
    // Show Fault Flags
    //ee_printf("SCB->HFSR=%x\n", SCB->HFSR);

    // If it is a force HardFault, usually, it means that the HardFault was escalated from other core exceptions
    if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
    {
        //ee_printf("SCB->CFSR=%x\n", SCB->CFSR);
        if (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk)
        {
            //ee_printf("HardFault was escalated from MemUsageFault\n");
            s_scbFlag.mmfsr = SCB->CFSR & 0xFF;
            s_scbFlag.abfsr = SCB->ABFSR;
            //MemManage_Handler();
        }
        if (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk)
        {
            //ee_printf("HardFault was escalated from BusFault\n");
            s_scbFlag.bfsr = (SCB->CFSR >> 8) & 0xFF;
            //BusFault_Handler();
        }
        if (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk)
        {
            //ee_printf("HardFault was escalated from UsageFault\n");
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
    ee_printf("SCB->CFSR[MMFSR]=%x\n", mmfsr);

    if (mmfsr & 0x01)
    {
        ee_printf("Instruction access violation flag is asserted\n");
    }
    if (mmfsr & 0x02)
    {
        ee_printf("IDtat access violation flag is asserted\n");
    }
    if (mmfsr & 0x04)
    {
        ee_printf("Instruction access violation flag is asserted\n");
    }
    if (mmfsr & 0x80)
    {
        ee_printf("SCB->MMFAR=%x\n", SCB->MMFAR);
    }

#if (__CORTEX_M == 7)
    uint32_t abfsr = SCB->ABFSR;
    ee_printf("SCB->ABFSR=%x\n", abfsr);

    if (abfsr & 0x01)
    {
        ee_printf("Async fault on ITCM interface\n");
    }
    if (abfsr & 0x02)
    {
        ee_printf("Async fault on DTCM interface\n");
    }
    if (abfsr & 0x04)
    {
        ee_printf("Async fault on AHBP interface\n");
    }
    if (abfsr & 0x08)
    {
        ee_printf("Async fault on AXIM interface\n");
    }
    if (abfsr & 0x10)
    {
        ee_printf("Async fault on EPPB interface\n");
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
    ee_printf("SCB->CFSR[BFSR]=%x\n", bfsr);

    if (bfsr & 0x01)
    {
        ee_printf("Instruction Bus Error\n");
    }
    if (bfsr & 0x02)
    {
        ee_printf("Precise Data Bus Error\n");
    }
    if (bfsr & 0x04)
    {
        ee_printf("Imprecise Data Bus Error\n");
    }
    if (bfsr & 0x80)
    {
        ee_printf("SCB->BFAR=%x\n", SCB->BFAR);
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
    ee_printf("SCB->CFSR[UFSR]=%x\n", ufsr);

    if (ufsr & 0x01)
    {
        ee_printf("Undefined instruction\n");
    }
    if (ufsr & 0x02)
    {
        ee_printf("Invalid State\n");
    }
    if (ufsr & 0x04)
    {
        ee_printf("Invalid PC load UsageFault\n");
    }
    if (ufsr & 0x08)
    {
        ee_printf("No coprocessor\n");
    }
    if (ufsr & 0x100)
    {
        ee_printf("Unaliged access UsageFault\n");
    }

    if (SCB->SHCSR & SCB_SHCSR_USGFAULTACT_Msk)
    {
        while (1);
    }
}
#endif // #if (__CORTEX_M >= 3)

