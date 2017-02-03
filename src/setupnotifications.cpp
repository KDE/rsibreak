/* ============================================================
 * Copyright (C) 2010 by Tom Albers <toma@kde.org>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * ============================================================ */

// Local includes.
#include "setupnotifications.h"

// QT includes.
#include <QVBoxLayout>

// KDE includes.
#include <KNotifyConfigWidget>

SetupNotifications::SetupNotifications( QWidget* parent )
        : QWidget( parent )
{
    QVBoxLayout *l = new QVBoxLayout( this );
    m_notify = new KNotifyConfigWidget( this );
    m_notify->setApplication( "rsibreak" );
    l->addWidget( m_notify );
}

SetupNotifications::~SetupNotifications()
{
}

void SetupNotifications::save()
{
    m_notify->save();
}
