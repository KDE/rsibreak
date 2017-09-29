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

#include "rsitimercounter_test.h"

#include "rsitimercounter.h"

static constexpr int TEST_DELAY = 15*60;
static constexpr int TEST_THRESHOLD = 40;
static constexpr int TEST_BREAK = 30;

void RSITimerCounterTest::normalCountdown()
{
    RSITimerCounter counter = RSITimerCounter( TEST_DELAY, TEST_BREAK, TEST_THRESHOLD );

    static constexpr int TEST_RUNS = 5;
    for ( int k = 0; k < TEST_RUNS; k++ ) {
        for ( int i = 0; i < TEST_DELAY - 1; i++ ) {
            int breakInterval = counter.tick( 0 );
            QCOMPARE( breakInterval, 0 );
            QCOMPARE( counter.isReset(), false );
        }
        int breakInterval = counter.tick( 0 );
        QCOMPARE( breakInterval, TEST_BREAK );
    }
}

void RSITimerCounterTest::thresholdReached()
{
    RSITimerCounter counter = RSITimerCounter( TEST_DELAY, TEST_BREAK, TEST_THRESHOLD );

    for ( int i = 0; i < TEST_THRESHOLD - 1; i++ ) {
        int breakInterval = counter.tick( i + 1 );
        QCOMPARE( breakInterval, 0 );
    }

    // Idle break detection should trigger.
    int breakInterval = counter.tick( TEST_THRESHOLD );
    QCOMPARE( breakInterval, 0 );
    QCOMPARE( counter.isReset(), true );
}

void RSITimerCounterTest::mixedCountdown()
{
    RSITimerCounter counter = RSITimerCounter( TEST_DELAY, TEST_BREAK, TEST_THRESHOLD );

    int part1 = TEST_DELAY / 2;

    QVERIFY2( TEST_THRESHOLD < TEST_DELAY, "Test parameters make no sense" );

    // First non-idle for part1 seconds.
    for ( int i = 0; i < part1; i++ ) {
        int breakInterval = counter.tick( 0 );
        QVERIFY2( breakInterval == 0, "Break triggered too fast" );
        QCOMPARE( counter.isReset(), false );
    }
    // Then idle for the rest of the period.
    for ( int i = 0; i < TEST_THRESHOLD - 1; i++ ) {
        int breakInterval = counter.tick( i + 1 );
        QVERIFY2( breakInterval == 0, "Break triggered too fast" );
        QCOMPARE( counter.isReset(), false );
    }

    // Idle break detection should trigger.
    int breakInterval = counter.tick( TEST_THRESHOLD );
    QCOMPARE( breakInterval, 0 );
    QVERIFY2( counter.isReset(), QString( "Counter is not reset after %1 ticks" ).arg( TEST_DELAY ).toLatin1() );
}

#include "rsitimercounter_test.moc"
