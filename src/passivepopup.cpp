/*
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2005-2007 Tom Albers <toma@kde.org>

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

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>

static const int MARGIN = 30;

PassivePopup::PassivePopup( QWidget *parent )
        : KPassivePopup( parent ) {}

void PassivePopup::show()
{
    // Hardcoded to show at bottom-center for now
    QRect screenRect = QApplication::desktop()->availableGeometry();
    int posX = screenRect.left() + (screenRect.width() - sizeHint().width()) / 2;
    int posY = screenRect.bottom() - sizeHint().height() - MARGIN;
    KPassivePopup::show( QPoint( posX, posY ) );
}

void PassivePopup::mouseReleaseEvent( QMouseEvent * event )
{
    event->accept();
    /* eat this! */
}
