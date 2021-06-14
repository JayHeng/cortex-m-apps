/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "fsl_device_registers.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "autobaud.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
//! @brief UART autobaud port irq configurations
#define GPIO_IRQC_INTERRUPT_ENABLED_PRIORITY 1
#define GPIO_IRQC_INTERRUPT_RESTORED_PRIORITY 0

/*====================== LPUART IOMUXC Definitions ===========================*/
#define UART1_RX_IOMUXC_MUX_FUNC    IOMUXC_GPIO_09_LPUART1_RXD
#define UART1_RX_IOMUXC_MUX_GPIO    IOMUXC_GPIO_09_GPIOMUX_IO09
#define UART1_RX_GPIO_BASE          GPIO1
#define UART1_RX_GPIO_PIN_NUM       9 // gpiomux.io[9], sel gpio1/gpio2 via iomuxc_gpr26
#define UART1_RX_GPIO_IRQn          GPIO1_Combined_0_15_IRQn
#define UART1_RX_GPIO_IRQHandler    GPIO1_Combined_0_15_IRQHandler

#define UART1_TX_IOMUXC_MUX_FUNC    IOMUXC_GPIO_10_LPUART1_TXD
#define UART1_TX_GPIO_PIN_NUM       10 // gpiomux.io[10], sel gpio1/gpio2 via iomuxc_gpr26

#define LPUART1_PAD_CTRL            (IOMUXC_SW_PAD_CTL_PAD_SRE(0) | IOMUXC_SW_PAD_CTL_PAD_DSE(6) | IOMUXC_SW_PAD_CTL_PAD_SPEED(2) | \
                                     IOMUXC_SW_PAD_CTL_PAD_PKE(1) | IOMUXC_SW_PAD_CTL_PAD_PUE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(1))
#define UART1_PULLUP_PAD_CTRL       (IOMUXC_SW_PAD_CTL_PAD_PKE(1) | IOMUXC_SW_PAD_CTL_PAD_PUS(2))

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//! this is to store the function pointer for calling back to the function that wants
//! the UART RX instance pin that triggered the interrupt. This only supports 1 pin
//! for UART0 because UART1 is on PORTC which does not support interrupts :(

static pin_irq_callback_t s_pin_irq_func;

/*******************************************************************************
 * Code
 ******************************************************************************/
static inline void IOMUXC_SetUartAutoBaudPinMode(uint32_t muxRegister,
                                                 uint32_t muxMode,
                                                 uint32_t inputRegister,
                                                 uint32_t inputDaisy,
                                                 uint32_t configRegister,
                                                 GPIO_Type *gpioBase,
                                                 uint32_t pin)
{
    // Configure the UART RX pin to GPIO mode
    IOMUXC_SetPinMux(muxRegister, muxMode, inputRegister, inputDaisy, configRegister, 0);

    // Pull-up resistor enabled.
    *((volatile uint32_t *)configRegister) |= UART1_PULLUP_PAD_CTRL;

    // Configure UART RX pin to digital input mode.
    gpioBase->GDIR &= (uint32_t) ~(1 << pin);
}

static inline void IOMUXC_SetUartPinMode(
    uint32_t muxRegister, uint32_t muxMode, uint32_t inputRegister, uint32_t inputDaisy, uint32_t configRegister)
{
    IOMUXC_SetPinMux(muxRegister, muxMode, inputRegister, inputDaisy, configRegister, 0);
    *(volatile uint32_t *)configRegister = LPUART1_PAD_CTRL;
};

/*!
 * @brief Configure pinmux for uart module.
 *
 * This function only support switching default or gpio or fixed-ALTx mode on fixed pins
 * (Although there are many ALTx-pinmux configuration choices on various pins for the same
 * peripheral module)
 */
void uart_pinmux_config(bool setGpio)
{
    if (setGpio)
    {
        IOMUXC_SetUartAutoBaudPinMode(UART1_RX_IOMUXC_MUX_GPIO, UART1_RX_GPIO_BASE, UART1_RX_GPIO_PIN_NUM);
    }
    else
    {
        // Enable pins for UART1.
        IOMUXC_SetUartPinMode(UART1_RX_IOMUXC_MUX_FUNC);
        IOMUXC_SetUartPinMode(UART1_TX_IOMUXC_MUX_FUNC);
    }
}

//! @brief this is going to be used for autobaud IRQ handling for UART1
void UART1_RX_GPIO_IRQHandler(void)
{
    uint32_t interrupt_flag = (1U << UART1_RX_GPIO_PIN_NUM);
    // Check if the pin for UART1 is what triggered the RX PORT interrupt
    if ((GPIO_GetPinsInterruptFlags(UART1_RX_GPIO_BASE) & interrupt_flag) && s_pin_irq_func)
    {
        s_pin_irq_func();
        GPIO_ClearPinsInterruptFlags(UART1_RX_GPIO_BASE, interrupt_flag);
    }
}

void enable_autobaud_pin_irq(pin_irq_callback_t func)
{
    s_pin_irq_func = func;
    // Only look for a falling edge for our interrupts
    GPIO_SetPinInterruptConfig(UART1_RX_GPIO_BASE, UART1_RX_GPIO_PIN_NUM, kGPIO_IntFallingEdge);
    UART1_RX_GPIO_BASE->IMR |= (1U << UART1_RX_GPIO_PIN_NUM);
    NVIC_SetPriority(UART1_RX_GPIO_IRQn, GPIO_IRQC_INTERRUPT_ENABLED_PRIORITY);
    NVIC_EnableIRQ(UART1_RX_GPIO_IRQn);
}

void disable_autobaud_pin_irq(void)
{
    NVIC_DisableIRQ(UART1_RX_GPIO_IRQn);
    NVIC_SetPriority(UART1_RX_GPIO_IRQn, GPIO_IRQC_INTERRUPT_RESTORED_PRIORITY);
    GPIO_SetPinInterruptConfig(UART1_RX_GPIO_BASE, UART1_RX_GPIO_PIN_NUM, kGPIO_NoIntmode);
    s_pin_irq_func = 0;
}

#define AUTOBAUD_PIN_DEBOUNCE_READ_COUNT (20U)

uint32_t read_autobaud_pin(void)
{
    // Sample the pin a number of times
    uint32_t readCount = 0;
    for (uint32_t i = 0; i < AUTOBAUD_PIN_DEBOUNCE_READ_COUNT; i++)
    {
        readCount += GPIO_PinRead(UART1_RX_GPIO_BASE, UART1_RX_GPIO_PIN_NUM);
    }

    // Most of measurements are the final value
    return (readCount < (AUTOBAUD_PIN_DEBOUNCE_READ_COUNT / 2)) ? 0 : 1;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
