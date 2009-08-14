/*
   Copyright (C) 2009 Tom Albers <toma@kde.org>

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

#include "plasmaeffect.h"
#include "kdeversion.h"

#include <KDebug>

#include <QApplication>
#include <QDesktopWidget>
#include <QDBusInterface>

PlasmaEffect::PlasmaEffect( QObject* parent )
        : BreakBase( parent )
{
    // Make all other screens gray...
    setGrayEffectOnAllScreens( true );
    excludeGrayEffectOnScreen( QApplication::desktop()->primaryScreen() );
}

void PlasmaEffect::activate()
{
    // Before 4.3 it was org.kde.plasma, after that it became org.kde.plasma-desktop
#if KDE_IS_VERSION(4,3,0)
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
#else
    QDBusInterface dbus( "org.kde.plasma", "/App" );
#endif

    // Before 4.4 there was no showDashboard( bool ), only a toggle....
#if KDE_IS_VERSION(4,3,60)
    QDBusMessage reply = dbus.call( QLatin1String( "showDashboard" ), true );
#else
    kDebug() << "Old style";
    QDBusMessage reply = dbus.call( QLatin1String( "toggleDashboard" ) );
#endif
    BreakBase::activate();

    if ( reply.type() == QDBusMessage::ErrorMessage ) {
        kDebug() << reply.errorMessage() << reply.errorName();
    }
}

void PlasmaEffect::deactivate()
{
    // Before 4.3 it was org.kde.plasma, after that it became org.kde.plasma-desktop
#if KDE_IS_VERSION(4,3,0)
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
#else
    QDBusInterface dbus( "org.kde.plasma", "/App" );
#endif

#if KDE_IS_VERSION(4,3,60)
    QDBusMessage reply = dbus.call( QLatin1String( "showDashboard" ), false );
#else
    kDebug() << "Old style";
    QDBusMessage reply = dbus.call( QLatin1String( "toggleDashboard" ) );
#endif

    if ( reply.type() == QDBusMessage::ErrorMessage ) {
        kDebug() << reply.errorMessage() << reply.errorName();
    }

    BreakBase::deactivate();
}

#include "plasmaeffect.moc"
