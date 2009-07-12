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
#include <QWidget>
#include <QDBusInterface>

PlasmaEffect::PlasmaEffect( QObject* parent )
        : BreakBase( parent )
{
}

void PlasmaEffect::activate()
{
#if KDE_IS_VERSION(4,3,60)
    kDebug() << "New style";
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "showDashboard" ), true );
#else
    kDebug() << "Old style";
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "toggleDashboard" ) );
#endif
    BreakBase::activate();
}

void PlasmaEffect::deactivate()
{
#if KDE_IS_VERSION(4,3,60)
    kDebug() << "New style";
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "showDashboard" ), false );
#else
    kDebug() << "Old style";
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "toggleDashboard" ) );
#endif
    BreakBase::deactivate();
}

#include "plasmaeffect.moc"
