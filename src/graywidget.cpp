/* This file is part of the KDE project
   Copyright (C) 2005-2007 Tom Albers <tomalbers@kde.nl>
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#include "graywidget.h"
#include "rsitimer_dpms.h"
#include "boxdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QPainter>
#include <QTimer>

#include <KWindowSystem>
#include <KDebug>

#include <qimageblitz.h>

GrayWidget::GrayWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup ), m_currentY( 0 ), m_first( true )
{
    // full screen
    setAttribute( Qt::WA_NoSystemBackground );
    QRect rect = QApplication::desktop()->screenGeometry(
                     QApplication::desktop()->primaryScreen() );
    setGeometry( rect );

    m_dialog = new BoxDialog( this, Qt::Popup );
}

GrayWidget::~GrayWidget() {}

void GrayWidget::reset()
{
    hide();
    m_currentY = 0;
    m_first = true;
    m_dialog->reject();
}

// This slot and the paint event is partly copied from KDE's logout screen.
// from various authors found in:
// /KDE/4/kdebase/workspace/ksmserver/shutdowndlg.cpp
void GrayWidget::slotGrayEffect()
{
    if ( m_currentY >= height() ) {
        m_dialog->showDialog();
        return;
    }
    if ( m_first ) {
        m_first = false;
        m_complete = takeScreenshot( QX11Info::appScreen() ).toImage();

        show();

        KWindowSystem::forceActiveWindow( winId() );
        KWindowSystem::setOnAllDesktops( winId(), true );
        KWindowSystem::setState( winId(), NET::KeepAbove );
        KWindowSystem::setState( winId(), NET::FullScreen );

    }
    repaint();
}

void GrayWidget::paintEvent( QPaintEvent* )
{
    kDebug() << m_currentY << m_first;

    if ( m_currentY >= height() )
        return;

    // this part we want to process...
    QImage change( width(), 15, QImage::Format_RGB32 );
    change = m_complete.copy( 0, m_currentY, width(), 15 );
    change = Blitz::fade( change, 0.4, Qt::black );
    Blitz::grayscale( change, true );

    QPainter painter( this );
    painter.drawImage( 0, m_currentY, change );

    if ( m_currentY == 0 )
        painter.drawImage( 0, 15, m_complete.copy( 0, 15, width(), height() - 15 ) );

    m_currentY += 15;
    QTimer::singleShot( 10, this, SLOT( slotGrayEffect() ) );
}

#include "graywidget.moc"
