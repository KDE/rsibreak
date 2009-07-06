/*
   Copyright (C) 2009 Tom Albers <tomalbers@kde.nl>

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

#include <KDebug>
#include <QWidget>
#include <QDBusInterface>
#include <QKeyEvent>

PlasmaEffect::PlasmaEffect( QWidget* parent )
        : QObject( parent ), m_parent( parent )
{
    m_parent->installEventFilter( this );
}

void PlasmaEffect::Activate()
{
    if ( m_readOnly ) {
        kDebug() << "grabbing tha keyboard";
        m_parent->grabKeyboard();
    } else
        kDebug() << "not grabbing";

    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "showDashboard"), true );
}

void PlasmaEffect::Deactivate()
{
    kDebug();
    m_parent->releaseKeyboard();
    QDBusInterface dbus( "org.kde.plasma-desktop", "/App" );
    dbus.call( QLatin1String( "showDashboard"), false );
}

void PlasmaEffect::setReadOnly( bool ro )
{
    m_readOnly = ro;
}

bool PlasmaEffect::readOnly()
{
    return m_readOnly;
}

bool PlasmaEffect::eventFilter( QObject *obj, QEvent *event )
{
    kDebug();
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        kDebug( "Ate key press %d", keyEvent->key() );
        if ( keyEvent->key() == Qt::Key_Escape ) {
            kDebug() << "Escape";
        }
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter( obj, event );
    }
}

#include "plasmaeffect.moc"
