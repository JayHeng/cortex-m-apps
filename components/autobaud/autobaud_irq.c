/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "autobaud.h"
#include "microseconds.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
enum _autobaud_counts
{
    //! 0x5A 字节对应的下降沿个数
    kFirstByteRequiredFallingEdges = 4,
    //! 0xA6 字节对应的下降沿个数
    kSecondByteRequiredFallingEdges = 3,
    //! 0x5A 字节（从起始位到停止位）第一个下降沿到最后一个下降沿之间的实际bit数
    kNumberOfBitsForFirstByteMeasured = 8,
    //! 0xA6 字节（从起始位到停止位）第一个下降沿到最后一个下降沿之间的实际bit数
    kNumberOfBitsForSecondByteMeasured = 7,
    //! 两个下降沿之间允许的最大超时(us)
    kMaximumTimeBetweenFallingEdges = 80000,
    //! 对实际检测出的波特率值做对齐处理，以便于更好地配置UART模块
    kAutobaudStepSize = 1200
};

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////
//! @brief 管脚下降沿跳变回调函数
static void pin_transition_callback(void);
//! @brief 使能GPIO管脚中断
extern void enable_autobaud_pin_irq(pin_irq_callback_t func);
//! @brief 关闭GPIO管脚中断
extern void disable_autobaud_pin_irq(void);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

//!< 已检测到的下降沿个数
static uint32_t s_transitionCount;
//!< 0x5A 字节检测期间内对应计数值
static uint64_t s_firstByteTotalTicks;
//!< 0xA6 字节检测期间内对应计数值
static uint64_t s_secondByteTotalTicks;
//!< 上一次下降沿发生时系统计数值
static uint64_t s_lastToggleTicks;
//!< 下降沿之间最大超时对应计数值
static uint64_t s_ticksBetweenFailure;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void autobaud_init(void)
{
    s_transitionCount = 0;
    s_firstByteTotalTicks = 0;
    s_secondByteTotalTicks = 0;
    s_lastToggleTicks = 0;
    // 计算出下降沿之间最大超时对应计数值
    s_ticksBetweenFailure = microseconds_convert_to_ticks(kMaximumTimeBetweenFallingEdges);
    // 使能GPIO管脚中断，并注册中断处理回调函数
    enable_autobaud_pin_irq(pin_transition_callback);
}

void autobaud_deinit(void)
{
    // 关闭GPIO管脚中断
    disable_autobaud_pin_irq();
}

bool autobaud_get_rate(uint32_t *rate)
{
    if (s_transitionCount == (kFirstByteRequiredFallingEdges + kSecondByteRequiredFallingEdges))
    {
        // 计算出实际检测到的波特率值
        uint32_t calculatedBaud =
            (microseconds_get_clock() * (kNumberOfBitsForFirstByteMeasured + kNumberOfBitsForSecondByteMeasured)) /
            (uint32_t)(s_firstByteTotalTicks + s_secondByteTotalTicks);

        // 对实际检测出的波特率值做对齐处理
        // 公式：rounded = stepSize * (value/stepSize + .5)
        *rate = ((((calculatedBaud * 10) / kAutobaudStepSize) + 5) / 10) * kAutobaudStepSize;

        return true;
    }
    else
    {
        return false;
    }
}

void pin_transition_callback(void)
{
    // 获取当前系统计数值
    uint64_t ticks = microseconds_get_ticks();
    // 计数这次检测到的下降沿
    s_transitionCount++;

    // 如果本次下降沿与上次下降沿之间间隔过长，则从头开始检测
    uint64_t delta = ticks - s_lastToggleTicks;
    if (delta > s_ticksBetweenFailure)
    {
        s_transitionCount = 1;
    }

    switch (s_transitionCount)
    {
        case 1:
            // 0x5A 字节检测时间起点
            s_firstByteTotalTicks = ticks;
            break;

        case kFirstByteRequiredFallingEdges:
            // 得到 0x5A 字节检测期间内对应计数值
            s_firstByteTotalTicks = ticks - s_firstByteTotalTicks;
            break;

        case (kFirstByteRequiredFallingEdges + 1):
            // 0xA6 字节检测时间起点
            s_secondByteTotalTicks = ticks;
            break;

        case (kFirstByteRequiredFallingEdges + kSecondByteRequiredFallingEdges):
            // 得到 0xA6 字节检测期间内对应计数值
            s_secondByteTotalTicks = ticks - s_secondByteTotalTicks;
            // 关闭GPIO管脚中断
            disable_autobaud_pin_irq();
            break;
    }

    // 记录本次下降沿发生时系统计数值
    s_lastToggleTicks = ticks;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
