/*
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
