/***********************************************************************************************************************
 * File Name    : ulpt_ep.c
 * Description  : Contains data structures and functions used in ulpt_ep.c.
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "ulpt_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup ulpt_ep
 * @{
 **********************************************************************************************************************/

extern bsp_leds_t g_bsp_leds;

static volatile bool g_periodic_timer_flag = false;

/*******************************************************************************************************************//**
 * @brief This function is callback for periodic timer.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void periodic_timer_ulpt_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        g_periodic_timer_flag = true;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the necessary hardware for the example project.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t hw_module_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM driver */
    err = R_LPM_Open(&g_lpm_deep_sw_standby_ctrl, &g_lpm_deep_sw_standby_cfg);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Initialize ULPT1 in Periodic mode */
    err = R_ULPT_Open(&g_timer_ulpt_periodic_ctrl, &g_timer_ulpt_periodic_cfg);

    return err;
}


/*******************************************************************************************************************//**
 * @brief       This function gets the period value from the RTT and sets this period value for the one-shot timer.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ulpt_set_period(void)
{
    fsp_err_t       err                             = FSP_SUCCESS;
    uint32_t        period_in_second                = PERIOD_1S;
    uint32_t        period_in_raw                   = RESET_VALUE;
    timer_info_t    timer_info                      = {
                                                        .clock_frequency = RESET_VALUE,
                                                        .count_direction = (timer_direction_t) RESET_VALUE,
                                                        .period_counts = RESET_VALUE,
                                                     };

    /* Get the clock frequency of the periodic timer */
    err = R_ULPT_InfoGet(&g_timer_ulpt_periodic_ctrl, &timer_info);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Convert the period value in second to the raw value for a periodic timer*/
    period_in_raw = (uint32_t)((period_in_second * timer_info.clock_frequency ));
    printf("\r\ntimer_info.clock_frequency = %dHz\r\n", timer_info.clock_frequency);

    /* Set a period value for the periodic timer */
    err = R_ULPT_PeriodSet(&g_timer_ulpt_periodic_ctrl, period_in_raw);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function demonstrates a periodic timer operation.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t ulpt_periodic_operation(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     count_value = RESET_VALUE;
    led_power_t led_state   = (led_power_t)RESET_VALUE;

    /* Start periodic timer */
    err = R_ULPT_Start(&g_timer_ulpt_periodic_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Wait until ULPT1 timer underflow three times*/
    while (TIMES_MAX > count_value)
    {
        if (true == g_periodic_timer_flag)
        {
            g_periodic_timer_flag = false;
            count_value ++;

            if (LED_USE < g_bsp_leds.led_count)
            {
                led_state = led_state ^ (led_power_t)BSP_IO_LEVEL_HIGH;
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_USE], (bsp_io_level_t)led_state);
            }
        }
    }

    /* Stop ULPT1 in periodic mode */
    err = R_ULPT_Stop(&g_timer_ulpt_periodic_ctrl);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function de-initialize all opened hardware modules.
 * @param[in]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
void hw_module_deinit(void)
{
    /* Close ULPT1 module */
    if (MODULE_CLOSED != g_timer_ulpt_periodic_ctrl.open)
    {
        R_ULPT_Close(&g_timer_ulpt_periodic_ctrl);
    }

    /* Close LPM driver */
    if(MODULE_CLOSED != g_lpm_deep_sw_standby_ctrl.lpm_open)
    {
        R_LPM_Close(&g_lpm_deep_sw_standby_ctrl);
    }
}



/*******************************************************************************************************************//**
 * @} (end addtogroup ulpt_ep)
 **********************************************************************************************************************/
