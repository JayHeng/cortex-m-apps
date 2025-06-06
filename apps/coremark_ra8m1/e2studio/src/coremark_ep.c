/***********************************************************************************************************************
 * File Name    : coremark_ep.c
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020-2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "coremark_ep.h"
#include "ulpt_ep.h"
#include "gpt_timer.h"

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/

/**********************************************************************************************************************
* Private function declarations
**********************************************************************************************************************/
static fsp_err_t coremark_ep_startup(void);
static void coremark_ep_main_process(void);

#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
static bool standby_sram_write(uint32_t offset_addr, uint8_t * p_write_buf, uint32_t len);
#endif

extern int coremark_main(void);

uint32_t s_timerClockSourceInHz = 0;

/**********************************************************************************************************************
* Function implementations
**********************************************************************************************************************/

//BSP_PLACE_IN_SECTION(".itcm_data")uint32_t itcm_var = 0;
void bsp_itcm_test(void)
{
    volatile uint64_t counter_start, counter_end;
    counter_start = gpt_get_current_counter();
    for(uint32_t i=0; i<0x3000; i++)
    {
        memcpy((void *)0xC000, (void *)0x8000, 8*1024);
    }
    counter_end = gpt_get_current_counter();
    ee_printf("counter value before ITCM test = %llu\r\n", counter_start);
    ee_printf("counter value before ITCM test = %llu\r\n", counter_end);
    ee_printf("ITCM memcpy cost time (us) = %llu\r\n", (counter_end-counter_start)/(s_timerClockSourceInHz/1000000));
}

//BSP_PLACE_IN_SECTION(".dtcm_data")uint32_t dtcm_var = 0;
void bsp_dtcm_test(void)
{
    volatile uint64_t counter_start, counter_end;
    counter_start = gpt_get_current_counter();
    for(uint32_t i=0; i<0x3000; i++)
    {
        memcpy((void *)0x2000C000, (void *)0x20008000, 8*1024);
    }
    counter_end = gpt_get_current_counter();
    ee_printf("counter value before DTCM test = %llu\r\n", counter_start);
    ee_printf("counter value before DTCM test = %llu\r\n", counter_end);
    ee_printf("DTCM memcpy cost time (us) = %llu\r\n", (counter_end-counter_start)/(s_timerClockSourceInHz/1000000));
}

void coremark_timer_track(void)
{
    static volatile uint64_t counter0, counter1;
    counter0 = gpt_get_current_counter();
    ee_printf("counter value before run = %llu\r\n", counter0);
    counter0 = gpt_get_current_counter();
    ee_printf("counter value before run = %llu\r\n", counter0);
    counter0 = gpt_get_current_counter();
    ee_printf("counter value before run = %llu\r\n", counter0);
    ee_printf("----------------------------------\r\n");
}

void coremark_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Perform EP startup procedure */
    err = coremark_ep_startup();
    if (err != FSP_SUCCESS)
    {
        return;
    }

    /* Perform EP main procedure */
    coremark_timer_track();
    //bsp_itcm_test();
    //bsp_dtcm_test();
    coremark_main();
    while (true)
    {
    }
}

static fsp_err_t coremark_ep_startup(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize SCI UART module first to print log to terminal */
    err = uart_init();
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* In case of normal reset */
    {
        /* Print EP information */
        uart_ep_info_print();

        /* Store fixed data into standby SRAM */
#if (BSP_FEATURE_LPM_HAS_DEEP_STANDBY) && defined(SSRAM_START) && defined(SSRAM_LENGTH)
        if (false == standby_sram_write(APP_SSRAM_OFFSET, (uint8_t *)APP_SSRAM_DATA, APP_SSRAM_LEN))
        {
            TERMINAL_ERR_RETURN(FSP_ERR_INVALID_DATA,"standby_sram_write failed\r\n");
        }

        /* Print stored successful message */
        TERMINAL_PRINT("\r\nStored fixed data into standby SRAM\r\n");
#endif
    }
    
    // SCB->CCR[BP], SCB->CCR[LOB] have been set in SystemInit()
    uint32_t scb_ccr = SCB->CCR;
    ee_printf("Cortex-M85 SCB->CCR = 0x%x\r\n", scb_ccr);

    uint32_t cpu_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_CPUCLK);
    ee_printf("Cortex-M85 freq_hz = %d\r\n", cpu_freq_hz);
    //printf("Cortex-M85 freq_hz = %d\r\n", cpu_freq_hz);
    /* Get the source clock frequency (in Hz) */
    s_timerClockSourceInHz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD);
    s_timerClockSourceInHz >>= (uint32_t)(g_timer_gpt_periodic_cfg.source_div);
    ee_printf("PCLKD freq_hz = %d\r\n", s_timerClockSourceInHz);

    /* Initialize necessary hardware modules  */
    //err = hw_module_ulpt_init();
    err = hw_module_gpt_init();
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Start periodic timer */
    err = start_gpt_timer(&g_timer_gpt_periodic_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* The user selects the period for both timers */
    //err = ulpt_set_period();
    err = gpt_set_period();
    return err;
}

static void coremark_ep_main_process(void)
{
    /* Prints LPM operation start message */
    TERMINAL_PRINT("\r\nCoremark operating is started\r\n");
    //printf("\r\nCoremark operating is started\r\n");

    /* Example project loop*/
    while (true)
    {
    	//fsp_err_t err = ulpt_periodic_operation();
    	fsp_err_t err = gpt_periodic_operation();
        if (err != FSP_SUCCESS)
        {
            return;
        }
    }
}

#if defined(SSRAM_START) && defined(SSRAM_LENGTH)
/*******************************************************************************************************************//**
 *  @brief      This function will perform write data to Standby SRAM
 *  @param[in]  offset_addr   offset address at Standby SRAM sections
 *  @param[in]  p_souce       pointer to source data to be written
 *  @param[in]  len           write data length
 *  @retval     true          successful operation
 **********************************************************************************************************************/
static bool standby_sram_write(uint32_t offset_addr, uint8_t * p_source, uint32_t len)
{
    uint8_t * p_ssram = (uint8_t *)SSRAM_START + offset_addr;

    /* Over Standby SRAM address range */
    if (SSRAM_LENGTH <= offset_addr + len)
    {
        return false;
    }

    /* Write to Standby SRAM */
    memcpy(p_ssram, p_source, len);
    return true;
}
#endif

