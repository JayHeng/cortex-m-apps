/***********************************************************************************************************************
 * File Name    : ulpt_ep.h
 * Description  : Contains declarations of data structures and functions used in ulpt_ep.c.
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

#include "common_utils.h"
#include "uart_terminal.h"

#ifndef ULPT_EP_H_
#define ULPT_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* MACROs for FSP Module status */
#define MODULE_CLOSED       (0U)

/* MACRO to check if the LED count is zero */
#define LED_COUNT_ZERO      (0U)
#define LED_ONE             (0U)

/* MACROs to define time-period value */
#define PERIOD_1S           (1U)
#define PERIOD_2S           (2U)
#define PERIOD_3S           (3U)
#define TIMES_MAX           (3U)

/* MACRO for select the LED to blink in ULPT periodic mode */
#define LED_USE             (0U)

typedef enum e_led_power
{
    LED_ON = BSP_IO_LEVEL_LOW,
    LED_OFF = BSP_IO_LEVEL_HIGH
} led_power_t;

/* function declarations */
fsp_err_t hw_module_init(void);
fsp_err_t ulpt_set_period(void);
fsp_err_t ulpt_periodic_operation(void);
fsp_err_t ulpt_one_shot_operation(void);
void hw_module_deinit(void);

#endif /* ULPT_EP_H_ */
