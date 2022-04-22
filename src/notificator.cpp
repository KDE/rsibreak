/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "notificator.h"

#include <KLocalizedString>
#include <KNotification>

void Notificator::onShortTimerReset()
{
    KNotification::event("short timer reset", QString(), i18n("Timer for the short break has now been reset"), QStringLiteral("rsibreak0"));
}

void Notificator::onTimersReset()
{
    KNotification::event("timers reset", QString(), i18n("The timers have now been reset"), QStringLiteral("rsibreak0"));
}

void Notificator::onStartLongBreak()
{
    KNotification::event("start long break", i18n("Start of a long break"));
}

void Notificator::onEndLongBreak()
{
    KNotification::event("end long break", i18n("End of a long break"));
}

void Notificator::onStartShortBreak()
{
    KNotification::event("start short break", i18n("Start of a short break"));
}

void Notificator::onEndShortBreak()
{
    KNotification::event("end short break", i18n("End of a short break"));
}
