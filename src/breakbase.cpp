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

#include "breakbase.h"
#include "breakcontrol.h"

#include <KDebug>
#include <KWindowSystem>

#include <QKeyEvent>

BreakBase::BreakBase( QWidget* parent )
        : QObject( parent ), m_readOnly( false ), m_disableShortcut( false )
{
    m_parent = parent;
    m_parent->installEventFilter( this );
    m_breakControl = new BreakControl( parent, Qt::Popup );
    m_breakControl->hide();
    m_breakControl->installEventFilter( this );
    connect( m_breakControl, SIGNAL( skip() ), SIGNAL( skip() ) );
    connect( m_breakControl, SIGNAL( lock() ), SIGNAL( lock() ) );
}

void BreakBase::activate()
{
    m_breakControl->show();
    m_breakControl->setFocus();

    KWindowSystem::forceActiveWindow( m_breakControl->winId() );
    KWindowSystem::setOnAllDesktops( m_breakControl->winId(), true );
    KWindowSystem::setState( m_breakControl->winId(), NET::KeepAbove );
    KWindowSystem::setState( m_breakControl->winId(), NET::FullScreen );

    m_breakControl->grabKeyboard();
    m_breakControl->grabMouse();
}

void BreakBase::deactivate()
{
    m_breakControl->releaseMouse();
    m_breakControl->releaseKeyboard();
    m_breakControl->hide();
}

bool BreakBase::eventFilter( QObject *obj, QEvent *event )
{
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        kDebug() << "Ate key press" << keyEvent->key();
        if ( !m_disableShortcut && keyEvent->key() == Qt::Key_Escape ) {
            kDebug() << "Escape";
            emit skip();
        }
        return true;
    } else if ( m_readOnly &&
                ( event->type() == QEvent::MouseButtonPress ||
                  event->type() == QEvent::MouseButtonDblClick )
              ) {
        kDebug() << "Ate mouse click event";
        return true;
    } else {
        return QObject::eventFilter( obj, event );
    }
}

void BreakBase::setReadOnly( bool ro )
{
    m_readOnly = ro;
}

bool BreakBase::readOnly() const
{
    return m_readOnly;
}

void BreakBase::setLabel( const QString& text )
{
    m_breakControl->setText( text );
}

void BreakBase::showMinimize( bool show )
{
    m_breakControl->showMinimize( show );
}

void BreakBase::disableShortcut( bool disable )
{
    m_disableShortcut = disable;
}

#include "breakbase.moc"
