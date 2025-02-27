/* generated HAL source file - do not edit */
#include "hal_data.h"
ulpt_instance_ctrl_t g_timer_cancel_lpm_ctrl;
const ulpt_extended_cfg_t g_timer_cancel_lpm_extend =
{ .count_source = ULPT_CLOCK_LOCO,
  .ulpto = ULPT_PULSE_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptoa = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptoab_settings_b.ulptob = ULPT_MATCH_PIN_CFG_DISABLED,
  .ulptevi_filter = ULPT_ULPTEVI_FILTER_NONE,
  .enable_function = ULPT_ENABLE_FUNCTION_IGNORED,
  .trigger_edge = ULPT_TRIGGER_EDGE_RISING,
  .event_pin = ULPT_EVENT_PIN_RISING,

};
const timer_cfg_t g_timer_cancel_lpm_cfg =
{ .mode = TIMER_MODE_PERIODIC,
/* Actual period: 10 seconds. Actual duty: 50%. */.period_counts = (uint32_t) 0x50000,
  .duty_cycle_counts = 0x28000, .source_div = (timer_source_div_t) 0, .channel = 0, .p_callback =
          timer_cancel_lpm_callback,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = &g_timer_cancel_lpm_extend,
  .cycle_end_ipl = (12),
#if defined(VECTOR_NUMBER_ULPT0_INT)
    .cycle_end_irq       = VECTOR_NUMBER_ULPT0_INT,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer_cancel_lpm =
{ .p_ctrl = &g_timer_cancel_lpm_ctrl, .p_cfg = &g_timer_cancel_lpm_cfg, .p_api = &g_timer_on_ulpt };
sci_b_uart_instance_ctrl_t g_uart_ctrl;

sci_b_baud_setting_t g_uart_baud_setting =
        {
        /* Baud rate calculated with 0.160% error. */.baudrate_bits_b.abcse = 0,
          .baudrate_bits_b.abcs = 0, .baudrate_bits_b.bgdm = 1, .baudrate_bits_b.cks = 0, .baudrate_bits_b.brr = 64, .baudrate_bits_b.mddr =
                  (uint8_t) 256,
          .baudrate_bits_b.brme = false };

/** UART extended configuration for UARTonSCI HAL driver */
const sci_b_uart_extended_cfg_t g_uart_cfg_extend =
{ .clock = SCI_B_UART_CLOCK_INT, .rx_edge_start = SCI_B_UART_START_BIT_FALLING_EDGE, .noise_cancel =
          SCI_B_UART_NOISE_CANCELLATION_DISABLE,
  .rx_fifo_trigger = SCI_B_UART_RX_FIFO_TRIGGER_MAX, .p_baud_setting = &g_uart_baud_setting, .flow_control =
          SCI_B_UART_FLOW_CONTROL_RTS,
#if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
  .flow_control_pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
  .rs485_setting =
  { .enable = SCI_B_UART_RS485_DISABLE,
    .polarity = SCI_B_UART_RS485_DE_POLARITY_HIGH,
    .assertion_time = 1,
    .negation_time = 1, } };

/** UART interface configuration */
const uart_cfg_t g_uart_cfg =
{ .channel = 9, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          uart_callback,
  .p_context = NULL, .p_extend = &g_uart_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (12),
  .txi_ipl = (12), .tei_ipl = (12), .eri_ipl = (12),
#if defined(VECTOR_NUMBER_SCI9_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI9_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI9_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI9_TEI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI9_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI9_ERI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart =
{ .p_ctrl = &g_uart_ctrl, .p_cfg = &g_uart_cfg, .p_api = &g_uart_on_sci_b };
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_deep_sw_standby_ctrl;

const lpm_cfg_t g_lpm_deep_sw_standby_cfg =
        { .low_power_mode = LPM_MODE_DEEP,
          .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ13 | LPM_STANDBY_WAKE_SOURCE_ULP0U
                  | (lpm_standby_wake_source_t) 0,
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
    .deep_standby_cancel_source = LPM_DEEP_STANDBY_CANCEL_SOURCE_IRQ13 | LPM_DEEP_STANDBY_CANCEL_SOURCE_ULPT0 |  (lpm_deep_standby_cancel_source_t) 0,
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
    .ldo_standby_cfg.pll1_ldo = true,
    .ldo_standby_cfg.pll2_ldo = true,
    .ldo_standby_cfg.hoco_ldo = true,
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
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_sw_standby_ctrl;

const lpm_cfg_t g_lpm_sw_standby_cfg =
        { .low_power_mode = LPM_MODE_STANDBY,
          .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ13 | LPM_STANDBY_WAKE_SOURCE_ULP0U
                  | (lpm_standby_wake_source_t) 0,
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
    .deep_standby_cancel_source =  (lpm_deep_standby_cancel_source_t) 0,
    .deep_standby_cancel_edge   =  (lpm_deep_standby_cancel_edge_t) 0,
#endif
#if BSP_FEATURE_LPM_HAS_PDRAMSCR
    .ram_retention_cfg.ram_retention = (uint8_t) ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) |  0),
    .ram_retention_cfg.tcm_retention = true,
#endif
#if BSP_FEATURE_LPM_HAS_DPSBYCR_SRKEEP
    .ram_retention_cfg.standby_ram_retention = true,
#endif
#if BSP_FEATURE_LPM_HAS_LDO_CONTROL
    .ldo_standby_cfg.pll1_ldo = true,
    .ldo_standby_cfg.pll2_ldo = true,
    .ldo_standby_cfg.hoco_ldo = true,
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

const lpm_instance_t g_lpm_sw_standby =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm_sw_standby_ctrl, .p_cfg = &g_lpm_sw_standby_cfg };
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_deep_sleep_ctrl;

const lpm_cfg_t g_lpm_deep_sleep_cfg =
{ .low_power_mode = LPM_MODE_DEEP_SLEEP, .standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ13
        | LPM_STANDBY_WAKE_SOURCE_ULP0U | (lpm_standby_wake_source_t) 0,
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
    .deep_standby_cancel_source =  (lpm_deep_standby_cancel_source_t) 0,
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
    .ldo_standby_cfg.pll1_ldo = true,
    .ldo_standby_cfg.pll2_ldo = true,
    .ldo_standby_cfg.hoco_ldo = true,
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

const lpm_instance_t g_lpm_deep_sleep =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm_deep_sleep_ctrl, .p_cfg = &g_lpm_deep_sleep_cfg };
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_sleep_ctrl;

const lpm_cfg_t g_lpm_sleep_cfg =
{ .low_power_mode = LPM_MODE_SLEEP, .standby_wake_sources = (lpm_standby_wake_source_t) 0,
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
    .deep_standby_cancel_source =  (lpm_deep_standby_cancel_source_t) 0,
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

const lpm_instance_t g_lpm_sleep =
{ .p_api = &g_lpm_on_lpm, .p_ctrl = &g_lpm_sleep_ctrl, .p_cfg = &g_lpm_sleep_cfg };
void g_hal_init(void)
{
    g_common_init ();
}
