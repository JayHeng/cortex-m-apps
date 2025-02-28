/***********************************************************************************************************************
 * File Name    : gpt_timer.c
 * Description  : Contains function definition.
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "gpt_timer.h"

/*******************************************************************************************************************//**
 * @addtogroup r_gpt_ep
 * @{
 **********************************************************************************************************************/

/* Store Timer open state*/
uint8_t g_timer_open_state = RESET_VALUE;

/*****************************************************************************************************************
 * @brief       Initialize GPT timer.
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @param[in]   p_timer_cfg     Timer instance Configuration structure
 * @param[in]   timer_mode      Mode of GPT Timer
 * @retval      FSP_SUCCESS     Upon successful open of timer.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful open .
 ****************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT Timer */
    err = R_GPT_Open(p_timer_ctl, p_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    if(PERIODIC_MODE_TIMER == timer_mode)
    {
        g_timer_open_state = PERIODIC_MODE;
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Start GPT timers in periodic, one shot, PWM mode.
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @retval      FSP_SUCCESS     Upon successful start of timer.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful start .
 ****************************************************************************************************************/
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Starts GPT timer */
    err = R_GPT_Start(p_timer_ctl);
    return err;
}

/*****************************************************************************************************************
 * @brief      Close the GPT HAL driver.
 * @param[in]  p_timer_ctl     Timer instance control structure
 * @retval     None
 ****************************************************************************************************************/
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl)
{
    R_GPT_Close(p_timer_ctl);

    /* Reset open state of timer*/
    g_timer_open_state = RESET_VALUE;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_gpt_ep)
 **********************************************************************************************************************/
