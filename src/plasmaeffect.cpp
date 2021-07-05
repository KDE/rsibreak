/*
    SPDX-FileCopyrightText: 2009-2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
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
