/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2005-2007 Tom Albers <tomalbers@kde.nl>

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

#include "passivepopup.h"

#include <QMouseEvent>

#include <KSystemTrayIcon>
#include <KDebug>

PassivePopup::PassivePopup( QWidget *parent )
        : KPassivePopup( parent ) {}

void PassivePopup::show( const QSystemTrayIcon* icon )
{
    /* To place it properly near the icon, first show it off-screen, then
       move it to the correct position. If you know something better, please,
       please let me know */
    KPassivePopup::show( icon->geometry().bottomRight() );
    moveNear( icon->geometry() );
}

void PassivePopup::mouseReleaseEvent( QMouseEvent * event )
{
    kDebug() << k_funcinfo;
    event->accept();
    /* eat this! */
}
