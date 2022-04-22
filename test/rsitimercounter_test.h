/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_RSITIMERCOUNTER_TEST_H
#define RSIBREAK_RSITIMERCOUNTER_TEST_H

#include <QtTest>

class RSITimerCounterTest : public QObject
{
private:
    Q_OBJECT

private slots:
    void normalCountdown();
    void thresholdReached();
    void mixedCountdown();
};

#endif // RSIBREAK_RSITIMERCOUNTER_TEST_H
