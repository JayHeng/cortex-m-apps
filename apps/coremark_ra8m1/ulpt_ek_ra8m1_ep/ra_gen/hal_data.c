/* generated HAL source file - do not edit */
#include "hal_data.h"
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_deep_sw_standby_ctrl;

const lpm_cfg_t g_lpm_deep_sw_standby_cfg =
{ .low_power_mode = LPM_MODE_DEEP, .standby_wake_sources = (lpm_standby_wake_source_t) 0,
#if BSP_FEATURE_LPM_HAS_SNOOZE
    .snooze_cancel_sources      = ,
    .snooze_request_source      = RA_NOT_DEFINED,
#if BSP_FEATURE_LPM_SNZEDCR_MASK > 0
    .snooze_end_sources         =  (lpm_snooze_end_t) 0,
#endif
    .dtc_state_in_snooze        = RA_NOT_DEFINED,
#endif
#if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable         = LPM_OUTPUT_PORT_ENABLE_RETAIN,
#endif
#if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state              = LPM_IO_PORT_NO_CHANGE,
    .power_supply_state         = LPM_POWER_SUPPLY_DEEP_STANDBY_MODE1,
    .deep_standby_cancel_source = LPM_DEEP_STANDBY_CANCEL_SOURCE_ULPT0 |  (lpm_deep_standby_cancel_source_t) 0,
    .deep_standby_cancel_edge   =  (lpm_deep_standby_cancel_edge_t) 0,
#endif
#if BSP_FEATURE_LPM_HAS_PDRAMSCR
    .ram_retention_cfg.ram_retention = (uint8_t) ( 0),
    .ram_retention_cfg.tcm_retention = true,
#endif
#if BSP_FEATURE_LPM_HAS_DPSBYCR_SRKEEP
    .ram_retention_cfg.standby_ram_retention = true,
#endif
#if BSP_FEATURE_LPM_HAS_LDO_CONTROL
    .ldo_standby_cfg.pll1_ldo = false,
    .ldo_standby_cfg.pll2_ldo = false,
    .ldo_standby_cfg.hoco_ldo = false,
#endif
#if BSP_FEATURE_LPM_HAS_FLASH_MODE_SELECT
    .lpm_flash_mode_select = false,
#endif
#if BSP_FEATURE_LPM_HAS_HOCO_STARTUP_SPEED_MODE
    .lpm_hoco_startup_speed = false,
#endif
#if BSP_FEATURE_LPM_HAS_STANDBY_SOSC_SELECT
    .lpm_standby_sosc = false,
#endif
  .p_extend = NULL, };

const lpm_instance_t g_lpm_deep_sw_standby =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm_deep_sw_standby_ctrl, .p_cfg = &g_lpm_deep_sw_standby_cfg };
ulpt_instance_ctrl_t g_timer_periodic_ctrl;
const ulpt_extended_cfg_t g_timer_periodic_extend =
{ .count_source = ULPT_CLOCK_LOCO,
  .ulpto = ULPT_PULSE_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptoa = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptob = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptevi_filter = ULPT_ULPTEVI_FILTER_NONE,
  .enable_function = ULPT_ENABLE_FUNCTION_IGNORED,
  .trigger_edge = ULPT_TRIGGER_EDGE_RISING,
  .event_pin = ULPT_EVENT_PIN_RISING,

};
const timer_cfg_t g_timer_periodic_cfg =
{ .mode = TIMER_MODE_PERIODIC,
  /* Actual period: 1 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x8000,
  .duty_cycle_counts = 0x4000,
  .source_div = (timer_source_div_t) 0,
  .channel = 1,
  .p_callback = periodic_timer_callback,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = &g_timer_periodic_extend,
  .cycle_end_ipl = (2),
#if defined(VECTOR_NUMBER_ULPT1_INT)
    .cycle_end_irq       = VECTOR_NUMBER_ULPT1_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer_periodic =
{ .p_ctrl = &g_timer_periodic_ctrl, .p_cfg = &g_timer_periodic_cfg, .p_api = &g_timer_on_ulpt };
ulpt_instance_ctrl_t g_timer_one_shot_ctrl;
const ulpt_extended_cfg_t g_timer_one_shot_extend =
{ .count_source = ULPT_CLOCK_LOCO,
  .ulpto = ULPT_PULSE_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptoa = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptob = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptevi_filter = ULPT_ULPTEVI_FILTER_NONE,
  .enable_function = ULPT_ENABLE_FUNCTION_IGNORED,
  .trigger_edge = ULPT_TRIGGER_EDGE_RISING,
  .event_pin = ULPT_EVENT_PIN_RISING,

};
const timer_cfg_t g_timer_one_shot_cfg =
{ .mode = TIMER_MODE_ONE_SHOT,
  /* Actual period: 1 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x8000,
  .duty_cycle_counts = 0x4000,
  .source_div = (timer_source_div_t) 0,
  .channel = 0,
  .p_callback = one_shot_timer_callback,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = &g_timer_one_shot_extend,
  .cycle_end_ipl = (2),
#if defined(VECTOR_NUMBER_ULPT0_INT)
    .cycle_end_irq       = VECTOR_NUMBER_ULPT0_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer_one_shot =
{ .p_ctrl = &g_timer_one_shot_ctrl, .p_cfg = &g_timer_one_shot_cfg, .p_api = &g_timer_on_ulpt };
void g_hal_init(void)
{
    g_common_init ();
}
