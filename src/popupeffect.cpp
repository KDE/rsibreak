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

#include "popupeffect.h"
#include "passivepopup.h"

#include <KLocalizedString>

#include <QLabel>

PopupEffect::PopupEffect( QObject* parent )
        : BreakBase( parent )
{
    m_popup = new PassivePopup( 0 );

    m_label = new QLabel( i18n( "Take a break...." ), m_popup );

    m_popup->setView( m_label );
    m_popup->setTimeout( 0 );
}

PopupEffect::~PopupEffect()
{
    delete m_popup;
}

void PopupEffect::activate()
{
    m_popup->show();
}

void PopupEffect::deactivate()
{
    m_popup->hide();
}

void PopupEffect::setLabel( const QString& text )
{
    // text is a time string like '2 minutes 42 seconds'
    // or '42 seconds', so no plural here
    m_label->setText( i18nc( "%1 is a time string like '2 minutes 42 seconds'", "Take a break for %1", text ) );
}

