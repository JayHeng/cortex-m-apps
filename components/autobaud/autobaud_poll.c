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
    //! the number of edge transitions being counted
    kRequiredEdges = 14,
    //! the number of bits being measured for the baud rate
    //! for this sequence of kFramingPacketStartByte(0x5A) + kFramingPacketType_Ping(0xA6) there are
    //! 20 bits total however the final rising edge is 1 bit before the stop bit so the final two bits
    //! will not be measured since this is all edge transition based. Additionally we will not start the
    //! timer until after the first rising transition from the start bit which is 2 bytes into the sequence
    //! so there are a total of:
    //! 20 - 1 (stop bit) - 1 (final rising bit) - 2 (bits to first rising edge) = 16
    //! bits we are measuring the time for
    kNumberOfBitsMeasured = 16,
    //! 250 milliseconds = 250000 microseconds
    kAutobaudDetectDelay = 250000
};

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

extern uint32_t read_autobaud_pin(void);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

static uint32_t s_initialEdge;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void autobaud_init(void)
{
    do
    {
        s_initialEdge = read_autobaud_pin();
    }
    while (s_initialEdge != 1);
}

void autobaud_deinit(void)
{

}

bool autobaud_get_rate(uint32_t *rate)
{
    uint32_t currentEdge = read_autobaud_pin();

    if (currentEdge != s_initialEdge)
    {
        // If we get a transition on a UART then one of the UART peripherals
        // is going to be the active peripheral so we can block and wait for autobaud
        // to finish here
        uint32_t previousEdge = currentEdge;
        // Keep track of any time outs
        bool expired = false;
        // Stores the running timer value
        uint64_t currentTicks;
        // Keeps a starting point for timeout reference and calculation
        uint64_t startTicks = microseconds_get_ticks();
        const uint64_t ticksTimeout = microseconds_convert_to_ticks(kAutobaudDetectDelay);

        // When we get to this point we know that we are active but are somewhere in the start
        // bit trough, due to other peripheral detection it is not guaranteed to be at the very start
        // of the transition so now that we are only spinning in here we can get an exact start time
        // at the next transition
        while (1)
        {
            currentTicks = microseconds_get_ticks();
            currentEdge = read_autobaud_pin();

            // Check for the second transition
            if (currentEdge != previousEdge)
            {
                break;
            }

            if ((currentTicks - startTicks) > ticksTimeout)
            {
                expired = true;
                break;
            }
        }
        previousEdge = currentEdge;

        // Now we have gotten another transition so store the time of the previous transition. Two transitions
        // have occurred now so up the counter again
        startTicks = currentTicks;
        // We have had two transitions at the point
        uint32_t transitionCount = 2;

        // keep counting our edge transitions until the required number is met
        while (transitionCount < kRequiredEdges)
        {
            currentEdge = read_autobaud_pin();

            if (currentEdge != previousEdge)
            {
                transitionCount++;
                previousEdge = currentEdge;
            }

            currentTicks = microseconds_get_ticks();

            if ((currentTicks - startTicks) > ticksTimeout)
            {
                expired = true;
                break;
            }
        }

        if (!expired)
        {
            *rate = (microseconds_get_clock() * kNumberOfBitsMeasured) / (uint32_t)(currentTicks - startTicks);

            return true;
        }
        else
        {
            // The timer has expired meaning it has been too long since an
            // edge has been detected, reset detection
            s_initialEdge = currentEdge;
            return false;
        }
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
