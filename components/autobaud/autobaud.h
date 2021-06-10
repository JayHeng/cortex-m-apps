/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _autobaud_h
#define _autobaud_h

#include "fsl_common.h"

//! @addtogroup autobaud
//! @{

/*******************************************************************************
 * Definitions
 ******************************************************************************/

//! @brief Callback function invoked for a pin change interrupt.
typedef void (*pin_irq_callback_t)(void);

//! @brief Pinmux types.
typedef enum _pinmux_types
{
    kPinmuxType_Default         = 0,
    kPinmuxType_PollForActivity = 1,
    kPinmuxType_Peripheral      = 2,
} pinmux_type_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

//! @name Autobaud
//@{

//! @brief If completed sets the rate after calculation as 9600, 19200, etc.
//         If not yet completed returns failure
//!
//! @retval #kStatus_Success The baud rate has been calculated successfully.
//! @retval #kStatus_Fail Not enough edges have been received yet to determine the baud rate.
status_t autobaud_get_rate(uint32_t *rate);

//! @brief Initializes autobaud detection
void autobaud_init(void);

//! @brief De-initializes autobaud detection
void autobaud_deinit(void);

//@}

#if defined(__cplusplus)
}
#endif

//! @}

#endif // _autobaud_h
