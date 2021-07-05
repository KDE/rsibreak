/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <memory>
#include <QTest>

#include "rsitimer_test.h"
#include "rsitimercounter_test.h"

int main( int argc, char *argv[] )
{
    std::unique_ptr<QApplication> app { new QApplication( argc, argv ) };

    std::vector<std::unique_ptr<QObject>> tests;
    tests.emplace_back( new RSITimerCounterTest() );
    tests.emplace_back( new RSITimerTest() );

    int status = 0;
    for ( auto& test : tests ) {
        status |= QTest::qExec( test.get(), argc, argv );
    }

    return status;
}
