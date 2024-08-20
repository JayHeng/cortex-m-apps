# Add set(CONFIG_USE_RTE_Device true) in config.cmake to use this component

include_guard(GLOBAL)
message("${CMAKE_CURRENT_LIST_FILE} component is included.")

add_config_file(${CMAKE_CURRENT_LIST_DIR}/RTE_Device.h ${CMAKE_CURRENT_LIST_DIR}/. RTE_Device.MIMXRT798S)

