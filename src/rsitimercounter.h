/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_RSITIMERCOUNTER_H
#define RSIBREAK_RSITIMERCOUNTER_H


class RSITimerCounter
{

private:
    const int m_delayTicks;
    const int m_breakLength;
    const int m_resetThreshold;

    int m_counter;          // counts ticks of user activity.

public:
    RSITimerCounter( const int delay, const int breakLength, const int resetThreshold )
        : m_delayTicks( delay )
        , m_breakLength( breakLength )
        , m_resetThreshold( resetThreshold )
        , m_counter( 0 ) { }

    ~RSITimerCounter() { }

    // Counts `elapsed` ticks.
    // @param idleTime time idle for this tick.
    // @returns non zero if break is due, for the number of ticks to break for.
    int tick( const int idleTime );

    // Resets the counter.
    void reset();

    // @returns ticks left till break for this counter.
    int counterLeft() const;

    // @returns ticks this timer delays for.
    int getDelayTicks() const;

    // @param ticks Postpones the timer by `ticks` ticks.
    void postpone( int ticks );

    // Returns if the timer was just reset.
    bool isReset();
};


#endif //RSIBREAK_RSITIMERCOUNTER_H
