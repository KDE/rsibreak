/* ============================================================
 * Copyright (C) 2005-2007,2009 by Tom Albers <toma@kde.org>
 * Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
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
    KNotifyConfigWidget* notify = new KNotifyConfigWidget( this );
    notify->setApplication( "rsibreak" );
    l->addWidget( notify );
}

SetupNotifications::~SetupNotifications()
{
}

#include "setupnotifications.moc"
