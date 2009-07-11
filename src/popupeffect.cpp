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

#include "popupeffect.h"
#include "passivepopup.h"

#include <KLocale>
#include <KHBox>

#include <QLabel>

PopupEffect::PopupEffect( QObject* parent )
        : BreakBase( parent )
{
    m_popup = new PassivePopup( 0 );

    KHBox* box = new KHBox( m_popup );
    m_label = new QLabel( i18n( "Take a break...." ), box );

    m_popup->setView( box );
    m_popup->setTimeout( 0 );
}

PopupEffect::~PopupEffect()
{
    delete m_popup;
}

void PopupEffect::activate()
{
    m_popup->show( m_tray );
}

void PopupEffect::deactivate()
{
    m_popup->hide();
}

void PopupEffect::setTray( QSystemTrayIcon* tray )
{
    m_tray = tray;
}

void PopupEffect::setLabel( const QString& text )
{
    // text can be 1:33 or 33, so no plural here
    m_label->setText( i18n( "Take a break for %1 seconds", text ) );
}

#include "popupeffect.moc"
