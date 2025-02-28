/***********************************************************************************************************************
 * File Name    : gpt_timer.h
 * Description  : Contains Macros and function declarations.
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
#ifndef GPT_TIMER_H_
#define GPT_TIMER_H_

/* Macros definitions */
#define GPT_MAX_PERCENT          (100U)          /* Max Duty Cycle percentage */
#define BUF_SIZE                 (16U)           /* Size of buffer for RTT input data */
#define PERIODIC_MODE_TIMER      (1U)            /* To perform GPT Timer in Periodic mode */
#define PWM_MODE_TIMER           (2U)            /* To perform GPT Timer in PWM mode */
#define ONE_SHOT_MODE_TIMER      (3U)            /* To perform GPT Timer in ONE-SHOT mode */
#define INITIAL_VALUE            ('\0')
#define TIMER_UNITS_MILLISECONDS  (1000U)        /* timer unit in millisecond */
#define CLOCK_TYPE_SPECIFIER      (1ULL)         /* type specifier */

/* GPT Timer Pin for boards */
#define TIMER_PIN           (GPT_IO_PIN_GTIOCB)

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA4W1_EK)
#define GPT_MAX_PERIOD_COUNT     (0XFFFF)        /* Max Period Count for 16-bit Timer*/
#else
#define GPT_MAX_PERIOD_COUNT     (0XFFFFFFFF)    /* Max Period Count for 32-bit Timer*/
#endif

#define PERIODIC_MODE	   (1U)                  /* To check status of GPT Timer in Periodic mode */
#define PWM_MODE	       (2U)                  /* To check status of GPT Timer in PWM mode */
#define ONE_SHOT_MODE      (3U)                  /* To check status of GPT Timer in oNE-SHOT mode */

#define EP_INFO    "\r\nThe project initializes GPT module in Periodic, PWM or One-shot mode based on user input " \
		"from the displayed menu options." \
		"\r\nIn periodic mode, user can enter the time period within the permitted ranges to change "\
		"the frequency of the user LED." \
		"\r\nIn PWM mode, user can enter the duty cycle within the specified range to adjust the "\
		"intensity of the user LED." \
		"\r\nIn ONE SHOT mode, Output will be displayed on JlinkRTTViewer when timer expires.\r\n "

/* Function declaration */
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg, uint8_t timer_mode);
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl);
fsp_err_t set_timer_duty_cycle(uint8_t duty_cycle_percent);
uint32_t  process_input_data(void);
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
void print_timer_menu(void);

#endif /* GPT_TIMER_H_ */
