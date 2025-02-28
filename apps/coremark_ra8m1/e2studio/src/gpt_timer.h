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
#define INITIAL_VALUE            ('\0')
#define TIMER_UNITS_MILLISECONDS  (1000U)        /* timer unit in millisecond */
#define CLOCK_TYPE_SPECIFIER      (1ULL)         /* type specifier */

#define TIMES_MAX_GPT           (3U)

/* MACRO for select the LED to blink in GPT periodic mode */
#define LED_USE_GPT            (0U)

/* GPT Timer Pin for boards */
#define TIMER_PIN           (GPT_IO_PIN_GTIOCB)

#if defined (BOARD_RA2A1_EK) || defined (BOARD_RA4W1_EK)
#define GPT_MAX_PERIOD_COUNT     (0XFFFF)        /* Max Period Count for 16-bit Timer*/
#else
#define GPT_MAX_PERIOD_COUNT     (0XFFFFFFFF)    /* Max Period Count for 32-bit Timer*/
#endif

/* Function declaration */
fsp_err_t hw_module_gpt_init(void);
fsp_err_t gpt_set_period(void);
fsp_err_t gpt_periodic_operation(void);
uint32_t gpt_get_current_counter(void);
fsp_err_t init_gpt_timer(timer_ctrl_t * const p_timer_ctl, timer_cfg_t const * const p_timer_cfg);
fsp_err_t start_gpt_timer (timer_ctrl_t * const p_timer_ctl);
void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);

#endif /* GPT_TIMER_H_ */
