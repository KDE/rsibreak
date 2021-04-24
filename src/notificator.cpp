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

#include "notificator.h"

#include <KNotification>
#include <KLocalizedString>

void Notificator::onShortTimerReset()
{
    KNotification::event( "short timer reset",
                          QString(),
                          i18n( "Timer for the short break has now been reset" ),
                          QStringLiteral("rsibreak0"));

}

void Notificator::onTimersReset()
{
    KNotification::event( "timers reset",
                          QString(),
                          i18n( "The timers have now been reset" ),
                          QStringLiteral("rsibreak0"));
}

void Notificator::onStartLongBreak()
{
    KNotification::event( "start long break", i18n( "Start of a long break" ) );
}

void Notificator::onEndLongBreak()
{
    KNotification::event( "end long break", i18n( "End of a long break" ) );
}

void Notificator::onStartShortBreak()
{
    KNotification::event( "start short break", i18n( "Start of a short break" ) );
}

void Notificator::onEndShortBreak()
{
    KNotification::event( "end short break", i18n( "End of a short break" ) );
}
