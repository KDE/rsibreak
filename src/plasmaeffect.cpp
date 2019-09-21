/*
   Copyright (C) 2009-2010 Tom Albers <toma@kde.org>

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

#include <QApplication>
#include <QDebug>
#include <QDBusInterface>
#include <QScreen>

PlasmaEffect::PlasmaEffect( QObject* parent )
        : BreakBase( parent )
{
    // Make all other screens gray...
    slotGray();

    connect( qApp, &QGuiApplication::screenAdded, this, &PlasmaEffect::slotGray );
    connect( qApp, &QGuiApplication::screenRemoved, this, &PlasmaEffect::slotGray );
}

void PlasmaEffect::slotGray()
{
    // Make all other screens gray...
    setGrayEffectOnAllScreens( true );
    excludeGrayEffectOnScreen( QGuiApplication::primaryScreen() );
}

void PlasmaEffect::activate()
{
    QDBusInterface dbus( "org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell" );
    QDBusMessage reply = dbus.call( QLatin1String( "setDashboardShown" ), true );
    BreakBase::activate();

    if ( reply.type() == QDBusMessage::ErrorMessage ) {
        qWarning() << reply.errorMessage() << reply.errorName();
    }
}

void PlasmaEffect::deactivate()
{
    QDBusInterface dbus( "org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell" );
    QDBusMessage reply = dbus.call( QLatin1String( "setDashboardShown" ), false );

    if ( reply.type() == QDBusMessage::ErrorMessage ) {
        qWarning() << reply.errorMessage() << reply.errorName();
    }

    BreakBase::deactivate();
}
