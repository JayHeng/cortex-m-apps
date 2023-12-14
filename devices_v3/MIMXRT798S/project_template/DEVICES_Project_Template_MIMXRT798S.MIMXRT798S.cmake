# Add set(CONFIG_USE_DEVICES_Project_Template_MIMXRT798S true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

if(CONFIG_USE_driver_lpflexcomm AND CONFIG_USE_driver_lpuart AND CONFIG_USE_driver_lpc_iopctl AND CONFIG_USE_driver_gpio AND CONFIG_USE_driver_cache_xcache AND CONFIG_USE_driver_reset AND (CONFIG_DEVICE_ID STREQUAL MIMXRT798S) AND CONFIG_USE_device_MIMXRT798S_startup AND CONFIG_USE_driver_common AND CONFIG_USE_driver_clock AND CONFIG_USE_driver_power AND ((CONFIG_USE_utility_debug_console AND CONFIG_USE_utility_assert AND CONFIG_USE_component_serial_manager) OR (CONFIG_USE_utility_debug_console_lite AND CONFIG_USE_utility_assert_lite)))

add_config_file(${CMAKE_CURRENT_LIST_DIR}/board.h "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/board.c "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/clock_config.h "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/clock_config.c "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/pin_mux.h "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/pin_mux.c "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/peripherals.h "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)
add_config_file(${CMAKE_CURRENT_LIST_DIR}/peripherals.c "" DEVICES_Project_Template_MIMXRT798S.MIMXRT798S)

target_include_directories(${MCUX_SDK_PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/.
)

else()

message(SEND_ERROR "DEVICES_Project_Template_MIMXRT798S.MIMXRT798S dependency does not meet, please check ${CMAKE_CURRENT_LIST_FILE}.")

endif()
