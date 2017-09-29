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
