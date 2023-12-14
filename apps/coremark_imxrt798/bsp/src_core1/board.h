/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"
#include "fsl_reset.h"
#include "fsl_gpio.h"
#if defined(MIMXRT798S_cm33_core0_SERIES)
#include "fsl_xspi.h"
#include "fsl_power.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "MIMXRT700-EVK"

/*! @brief The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE kSerialPort_Uart
#if defined(MIMXRT798S_cm33_core0_SERIES)
#define BOARD_DEBUG_UART_BASEADDR     (uint32_t) LPUART0
#define BOARD_DEBUG_UART_INSTANCE     0U
#define BOARD_DEBUG_UART_CLK_FREQ     CLOCK_GetLPFlexCommClkFreq(0)
#define BOARD_DEBUG_UART_CLK_ATTACH   kFCCLK0_to_FLEXCOMM0
#define BOARD_DEBUG_UART_FCCLK_DIV    kCLOCK_DivFcclk0Clk
#define BOARD_DEBUG_UART_FCCLK_ATTACH kOSC_CLK_to_FCCLK0
#define BOARD_UART_IRQ_HANDLER        LP_FLEXCOMM0_IRQHandler
#define BOARD_UART_IRQ                LP_FLEXCOMM0_IRQn
#elif defined(MIMXRT798S_cm33_core1_SERIES)
#if USE_EVK_BOARD
#define BOARD_DEBUG_UART_BASEADDR   (uint32_t) LPUART19
#define BOARD_DEBUG_UART_INSTANCE   19U
#define BOARD_DEBUG_UART_CLK_FREQ   CLOCK_GetLPFlexCommClkFreq(19)
#define BOARD_DEBUG_UART_CLK_ATTACH kSENSE_BASE_to_FLEXCOMM19
#define BOARD_DEBUG_UART_CLK_DIV    kCLOCK_DivLPFlexComm19Clk
#define BOARD_UART_IRQ_HANDLER      LP_FLEXCOMM19_IRQHandler
#define BOARD_UART_IRQ              LP_FLEXCOMM19_IRQn
#else
#define BOARD_DEBUG_UART_BASEADDR   (uint32_t) LPUART18
#define BOARD_DEBUG_UART_INSTANCE   18U
#define BOARD_DEBUG_UART_CLK_FREQ   CLOCK_GetLPFlexCommClkFreq(18)
#define BOARD_DEBUG_UART_CLK_ATTACH kSENSE_BASE_to_FLEXCOMM18
#define BOARD_DEBUG_UART_CLK_DIV    kCLOCK_DivLPFlexComm18Clk
#define BOARD_UART_IRQ_HANDLER      LP_FLEXCOMM18_IRQHandler
#define BOARD_UART_IRQ              LP_FLEXCOMM18_IRQn
#endif /* USE_EVK_BOARD*/
#else
#error "Unsupported core!"
#endif

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE 115200
#endif /* BOARD_DEBUG_UART_BAUDRATE */

/* Board RGB LED color mapping */
#define LOGIC_LED_ON  0U
#define LOGIC_LED_OFF 1U
#ifndef BOARD_LED_GREEN_GPIO
#define BOARD_LED_GREEN_GPIO GPIO0
#endif
#ifndef BOARD_LED_GREEN_GPIO_PIN
#define BOARD_LED_GREEN_GPIO_PIN 18U
#endif
#ifndef BOARD_LED_BLUE_GPIO
#define BOARD_LED_BLUE_GPIO GPIO0
#endif
#ifndef BOARD_LED_BLUE_GPIO_PIN
#define BOARD_LED_BLUE_GPIO_PIN 17U
#endif

#ifndef BOARD_LED_RED_GPIO
#define BOARD_LED_RED_GPIO GPIO8
#endif
#ifndef BOARD_LED_RED_GPIO_PIN
#define BOARD_LED_RED_GPIO_PIN 6U
#endif

#define LED_RED_INIT(output)                                           \
    GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, output); \
    BOARD_LED_RED_GPIO->PDDR |= (1U << BOARD_LED_RED_GPIO_PIN)                         /*!< Enable target LED_RED */
#define LED_RED_ON()  GPIO_PortClear(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN) /*!< Turn on target LED_RED */
#define LED_RED_OFF() GPIO_PortSet(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN)   /*!< Turn off target LED_RED */
#define LED_RED_TOGGLE() \
    GPIO_PortToggle(BOARD_LED_RED_GPIO, 1U << BOARD_LED_RED_GPIO_PIN)                  /*!< Toggle on target LED_RED1 */

#define LED_GREEN_INIT(output)                                             \
    GPIO_PinWrite(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, output); \
    BOARD_LED_GREEN_GPIO->PDDR |= (1U << BOARD_LED_GREEN_GPIO_PIN)        /*!< Enable target LED_GREEN */
#define LED_GREEN_ON() \
    GPIO_PortClear(BOARD_LED_GREEN_GPIO, 1U << BOARD_LED_GREEN_GPIO_PIN)  /*!< Turn on target LED_GREEN */
#define LED_GREEN_OFF() \
    GPIO_PortSet(BOARD_LED_GREEN_GPIO, 1U << BOARD_LED_GREEN_GPIO_PIN)    /*!< Turn off target LED_GREEN */
#define LED_GREEN_TOGGLE() \
    GPIO_PortToggle(BOARD_LED_GREEN_GPIO, 1U << BOARD_LED_GREEN_GPIO_PIN) /*!< Toggle on target LED_GREEN */

#define LED_BLUE_INIT(output)                                            \
    GPIO_PinWrite(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, output); \
    BOARD_LED_BLUE_GPIO->PDDR |= (1U << BOARD_LED_BLUE_GPIO_PIN)        /*!< Enable target LED_BLUE */
#define LED_BLUE_ON()                                                                                \
    GPIO_PortClear(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN)  /*!< Turn on target LED_BLUE \
                                                                         */
#define LED_BLUE_OFF()                                                                                \
    GPIO_PortSet(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN)    /*!< Turn off target LED_BLUE \
                                                                         */
#define LED_BLUE_TOGGLE() \
    GPIO_PortToggle(BOARD_LED_BLUE_GPIO, 1U << BOARD_LED_BLUE_GPIO_PIN) /*!< Toggle on target LED_BLUE */

/* Board SW PIN */
#ifndef BOARD_SW5_GPIO /* User button 1 */
#define BOARD_SW5_GPIO GPIO0
#endif
#ifndef BOARD_SW5_GPIO_PIN
#define BOARD_SW5_GPIO_PIN 9U
#endif
#ifndef BOARD_SW6_GPIO /* User button 3 */
#define BOARD_SW6_GPIO GPIO8
#endif
#ifndef BOARD_SW6_GPIO_PIN
#define BOARD_SW6_GPIO_PIN 5U
#endif
#ifndef BOARD_SW7_GPIO /* User button 2 */
#define BOARD_SW7_GPIO GPIO1
#endif
#ifndef BOARD_SW7_GPIO_PIN
#define BOARD_SW7_GPIO_PIN 3U
#endif

#define BOARD_ACCEL_I2C_BASEADDR LPI2C20

#define BOARD_CODEC_I2C_BASEADDR   LPI2C2
#define BOARD_CODEC_I2C_CLOCK_FREQ CLOCK_GetLPI2cClkFreq(2)
#define BOARD_CODEC_I2C_INSTANCE   2

#define BOARD_PMIC_I2C_BASEADDR   LPI2C15
#define BOARD_PMIC_I2C_CLOCK_FREQ CLOCK_GetLPI2cClkFreq(15)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitDebugConsole(void);
#if defined(MIMXRT798S_cm33_core0_SERIES)
void BOARD_ConfigMPU(void);
status_t BOARD_Init16bitsPsRam(XSPI_Type *base);
#endif
/* Initializes the AHB Secure Controller */
void BOARD_InitAHBSC(void);

#if defined(SDK_I2C_BASED_COMPONENT_USED) && SDK_I2C_BASED_COMPONENT_USED
void BOARD_I2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz);
status_t BOARD_I2C_Send(LPI2C_Type *base,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize);
status_t BOARD_I2C_Receive(LPI2C_Type *base,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize);

void BOARD_PMIC_I2C_Init(void);
status_t BOARD_PMIC_I2C_Send(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
status_t BOARD_PMIC_I2C_Receive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
#endif /* SDK_I2C_BASED_COMPONENT_USED */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
