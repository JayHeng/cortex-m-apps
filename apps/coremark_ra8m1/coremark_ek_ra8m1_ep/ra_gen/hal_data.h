/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_ulpt.h"
#include "r_timer_api.h"
#include "r_sci_b_uart.h"
#include "r_uart_api.h"
#include "r_lpm.h"
#include "r_lpm_api.h"
FSP_HEADER
/** ULPT Timer Instance */
extern const timer_instance_t g_timer_cancel_lpm;

/** Access the ULPT instance using these structures when calling API functions directly (::p_api is not used). */
extern ulpt_instance_ctrl_t g_timer_cancel_lpm_ctrl;
extern const timer_cfg_t g_timer_cancel_lpm_cfg;

#ifndef timer_cancel_lpm_callback
void timer_cancel_lpm_callback(timer_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t g_uart;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_uart_instance_ctrl_t g_uart_ctrl;
extern const uart_cfg_t g_uart_cfg;
extern const sci_b_uart_extended_cfg_t g_uart_cfg_extend;

#ifndef uart_callback
void uart_callback(uart_callback_args_t *p_args);
#endif
/** lpm Instance */
extern const lpm_instance_t g_lpm_deep_sw_standby;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm_deep_sw_standby_ctrl;
extern const lpm_cfg_t g_lpm_deep_sw_standby_cfg;
/** lpm Instance */
extern const lpm_instance_t g_lpm_sw_standby;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm_sw_standby_ctrl;
extern const lpm_cfg_t g_lpm_sw_standby_cfg;
/** lpm Instance */
extern const lpm_instance_t g_lpm_deep_sleep;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm_deep_sleep_ctrl;
extern const lpm_cfg_t g_lpm_deep_sleep_cfg;
/** lpm Instance */
extern const lpm_instance_t g_lpm_sleep;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm_sleep_ctrl;
extern const lpm_cfg_t g_lpm_sleep_cfg;
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
