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

#include <KDebug>

#include <QKeyEvent>
#include <QLabel>

BreakBase::BreakBase( QWidget* parent )
        : QObject( parent )
{
    m_parent = parent;
    m_parent->installEventFilter( this );
    m_label = new QLabel( parent, Qt::Popup );
    m_label->hide();
    m_label->installEventFilter( this );
}

void BreakBase::activate()
{
    m_label->show();
    m_label->setFocus();
    if ( m_readOnly )
        m_label->grabKeyboard();
}

void BreakBase::deactivate()
{
    kDebug();
    m_label->releaseKeyboard();
    m_label->hide();
}

bool BreakBase::eventFilter( QObject *obj, QEvent *event )
{
    kDebug();
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        kDebug() << "Ate key press" << keyEvent->key();
        if ( keyEvent->key() == Qt::Key_Escape ) {
            kDebug() << "Escape";
            deactivate();
        }
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
    m_label->setText( text );
}

#include "breakbase.moc"
