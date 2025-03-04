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
#include "ulpt_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup r_gpt_ep
 * @{
 **********************************************************************************************************************/

extern bsp_leds_t g_bsp_leds;
volatile uint32_t s_timerGptHighCounter = 0;
static volatile bool g_periodic_timer_gpt_flag = false;

/*******************************************************************************************************************//**
 * @brief This function is callback for periodic timer.
 * @param[in]  p_args
 * @retval     None
 **********************************************************************************************************************/
void periodic_timer_gpt_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
    	g_periodic_timer_gpt_flag = true;
    	s_timerGptHighCounter++;
    }
}

/*****************************************************************************************************************
 * @brief       Initialize GPT timer.
 * @param[in]   p_timer_ctl     Timer instance control structure
 * @param[in]   p_timer_cfg     Timer instance Configuration structure
 * @param[in]   timer_mode      Mode of GPT Timer
 * @retval      FSP_SUCCESS     Upon successful open of timer.
 * @retval      Any Other Error code apart from FSP_SUCCES on Unsuccessful open .
 ****************************************************************************************************************/
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize GPT Timer */
    err = R_GPT_Open(p_timer_ctl, p_timer_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function initializes the necessary hardware for the example project.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t hw_module_gpt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM driver */
    err = R_LPM_Open(&g_lpm_deep_sw_standby_ctrl, &g_lpm_deep_sw_standby_cfg);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

	/*Initialize Periodic Timer */
	err = init_gpt_timer(&g_timer_gpt_periodic_ctrl, &g_timer_gpt_periodic_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets the period value from the RTT and sets this period value for the one-shot timer.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t gpt_set_period(void)
{
    fsp_err_t err = FSP_SUCCESS;
	uint32_t gpt_desired_period_ms          = 1000;
	uint64_t period_counts                  = GPT_MAX_PERIOD_COUNT;
	uint32_t pclkd_freq_hz                  = RESET_VALUE;

	/* Convert period to PCLK counts so it can be set in hardware. */
	//period_counts = (uint64_t)((gpt_desired_period_ms * (pclkd_freq_hz * CLOCK_TYPE_SPECIFIER))  / TIMER_UNITS_MILLISECONDS);

	/* Validate Period Count based on user input (time period in ms) */
	if(GPT_MAX_PERIOD_COUNT < period_counts)
	{
		return FSP_ERR_ASSERTION;
	}
	else
	{
		/* Period Set API set the desired period counts on the on-board LED */
		err = R_GPT_PeriodSet(&g_timer_gpt_periodic_ctrl, (uint32_t)period_counts);
		if (FSP_SUCCESS != err)
		{
			/*Close Periodic Timer instance */
			deinit_gpt_timer(&g_timer_gpt_periodic_ctrl);
			return err;
		}
	}

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets the period value from the RTT and sets this period value for the one-shot timer.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
uint32_t gpt_get_current_counter(void)
{
	fsp_err_t err = FSP_SUCCESS;
    timer_status_t    timer_status;

    /* Get the clock frequency of the periodic timer */
    R_GPT_StatusGet(&g_timer_gpt_periodic_ctrl, &timer_status);
	if (FSP_SUCCESS != err)
	{
		return 0;
	}

    return timer_status.counter;
}

/*******************************************************************************************************************//**
 * @brief       This function demonstrates a periodic timer operation.
 * @param[in]   None
 * @retval      FSP_SUCCESS Upon successful operation
 * @retval      Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t gpt_periodic_operation(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     count_value = RESET_VALUE;
    led_power_t led_state   = (led_power_t)RESET_VALUE;

    /* Start periodic timer */
    err = start_gpt_timer(&g_timer_gpt_periodic_ctrl);
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    /* Wait until GPT timer underflow three times*/
    while (TIMES_MAX_GPT > count_value)
    {
        if (true == g_periodic_timer_gpt_flag)
        {
        	g_periodic_timer_gpt_flag = false;
            count_value ++;

            if (LED_USE_GPT < g_bsp_leds.led_count)
            {
                led_state = led_state ^ (led_power_t)BSP_IO_LEVEL_HIGH;
                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_USE_GPT], (bsp_io_level_t)led_state);
            }
        }
    }

    /* Stop GPT in periodic mode */
    err = R_GPT_Stop(&g_timer_gpt_periodic_ctrl);

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

}

/*******************************************************************************************************************//**
 * @} (end addtogroup r_gpt_ep)
 **********************************************************************************************************************/
