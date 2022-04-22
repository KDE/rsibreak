/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_NOTIFICATOR_H
#define RSIBREAK_NOTIFICATOR_H

#include <QDebug>
#include <QObject>

class Notificator : public QObject
{
    Q_OBJECT

public slots:

    void onShortTimerReset();

    void onTimersReset();

    void onStartLongBreak();

    void onEndLongBreak();

    void onStartShortBreak();

    void onEndShortBreak();
};

#endif // RSIBREAK_NOTIFICATOR_H
