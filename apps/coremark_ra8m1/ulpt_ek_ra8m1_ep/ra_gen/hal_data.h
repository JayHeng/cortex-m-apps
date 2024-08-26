/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_lpm.h"
#include "r_lpm_api.h"
#include "r_ulpt.h"
#include "r_timer_api.h"
FSP_HEADER
/** lpm Instance */
extern const lpm_instance_t g_lpm_deep_sw_standby;

/** Access the LPM instance using these structures when calling API functions directly (::p_api is not used). */
extern lpm_instance_ctrl_t g_lpm_deep_sw_standby_ctrl;
extern const lpm_cfg_t g_lpm_deep_sw_standby_cfg;
/** ULPT Timer Instance */
extern const timer_instance_t g_timer_periodic;

/** Access the ULPT instance using these structures when calling API functions directly (::p_api is not used). */
extern ulpt_instance_ctrl_t g_timer_periodic_ctrl;
extern const timer_cfg_t g_timer_periodic_cfg;

#ifndef periodic_timer_callback
void periodic_timer_callback(timer_callback_args_t *p_args);
#endif
/** ULPT Timer Instance */
extern const timer_instance_t g_timer_one_shot;

/** Access the ULPT instance using these structures when calling API functions directly (::p_api is not used). */
extern ulpt_instance_ctrl_t g_timer_one_shot_ctrl;
extern const timer_cfg_t g_timer_one_shot_cfg;

#ifndef one_shot_timer_callback
void one_shot_timer_callback(timer_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
