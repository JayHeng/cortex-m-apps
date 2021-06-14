/*
 * Copyright (c) 2021 NXP
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
//! @brief 读取GPIO管脚输入电平
extern uint32_t read_autobaud_pin(void);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void autobaud_init(void)
{
    // 确保电平为高（空闲态）
    while (read_autobaud_pin() != 1);
}

void autobaud_deinit(void)
{

}

bool autobaud_get_rate(uint32_t *rate)
{
    // 仅当电平为低（非空闲态）时才开始识别
    uint32_t currentEdge = read_autobaud_pin();
    if (currentEdge != 1)
    {
        uint64_t ticks = microseconds_get_ticks();
        uint32_t transitionCount = 1;
        // 0x5A 字节检测时间起点
        uint64_t firstByteTotalTicks = ticks;
        uint64_t secondByteTotalTicks;
        uint64_t lastToggleTicks = ticks;
        // 计算出下降沿之间最大超时对应计数值
        uint64_t ticksBetweenFailure = microseconds_convert_to_ticks(kMaximumTimeBetweenFallingEdges);
        uint32_t previousEdge = currentEdge;
        while (transitionCount < kFirstByteRequiredFallingEdges + kSecondByteRequiredFallingEdges)
        {
            // 获取当前系统计数值
            ticks = microseconds_get_ticks();
            // 检查是否有电平翻转
            currentEdge = read_autobaud_pin();
            if (currentEdge != previousEdge)
            {
                // 仅当电平翻转是下降沿时
                if (previousEdge == 1)
                {
                    // 计数这次检测到的下降沿
                    transitionCount++;
                    // 记录本次下降沿发生时系统计数值
                    lastToggleTicks = ticks;
                    // 得到 0x5A 字节检测期间内对应计数值
                    if (transitionCount == kFirstByteRequiredFallingEdges)
                    {
                        firstByteTotalTicks = ticks - firstByteTotalTicks;
                    }
                    // 0xA6 字节检测时间起点
                    else if (transitionCount == kFirstByteRequiredFallingEdges + 1)
                    {
                        secondByteTotalTicks = ticks;
                    }
                    // 得到 0xA6 字节检测期间内对应计数值
                    else if (transitionCount == kFirstByteRequiredFallingEdges + kSecondByteRequiredFallingEdges)
                    {
                        secondByteTotalTicks = ticks - secondByteTotalTicks;
                    }
                }
                previousEdge = currentEdge;
            }
            // 如果本次下降沿与上次下降沿之间间隔过长，则从头开始检测
            if ((ticks - lastToggleTicks) > ticksBetweenFailure)
            {
                return false;
            }
        }

        // 计算出实际检测到的波特率值
        uint32_t calculatedBaud =
            (microseconds_get_clock() * (kNumberOfBitsForFirstByteMeasured + kNumberOfBitsForSecondByteMeasured)) /
            (uint32_t)(firstByteTotalTicks + secondByteTotalTicks);
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

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////