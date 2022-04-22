/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsitimercounter.h"

#include <algorithm>

int RSITimerCounter::tick(const int idleTime)
{
    m_counter++;

    // Not idle for too long, time for a break.
    if (m_counter >= m_delayTicks) {
        reset();
        return m_breakLength;
    }

    // Idle for enough to consider the break has happened.
    if (idleTime >= m_resetThreshold) {
        reset();
        return 0;
    }

    // In-flight, not time for a break yet.
    return 0;
}

bool RSITimerCounter::isReset()
{
    return m_counter == 0;
}

void RSITimerCounter::reset()
{
    m_counter = 0;
}

void RSITimerCounter::postpone(int ticks)
{
    m_counter = std::max(0, m_delayTicks - ticks);
}

int RSITimerCounter::counterLeft() const
{
    return m_delayTicks - m_counter;
}

int RSITimerCounter::getDelayTicks() const
{
    return m_delayTicks;
}
