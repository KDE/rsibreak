/*
    SPDX-License-Identifier: GPL-2.0-or-later
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
