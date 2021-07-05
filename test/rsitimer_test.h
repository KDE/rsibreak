/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_RSITIMER_TEST_H
#define RSIBREAK_RSITIMER_TEST_H

#include <QtTest>

class RSITimerTest: public QObject
{
    Q_OBJECT
    QVector<int> m_intervals;

public:
    RSITimerTest();

private slots:
    void triggerSimpleTinyBreak();
    void triggerComplexTinyBreak();
    void testSuspended();
    void triggerSimpleBigBreak();
    void postponeBreak();
    void screenLock();
    void skipBreak();
    void noPopupBreak();
    void regularBreaks();
};

#endif //RSIBREAK_RSITIMER_TEST_H
