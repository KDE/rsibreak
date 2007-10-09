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
#include "grayouteffect.h"
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
    setAttribute( Qt::WA_PaintOnScreen );
    QRect rect = QApplication::desktop()->screenGeometry(
                     QApplication::desktop()->primaryScreen() );
    setGeometry( rect );

    m_gray = 0;

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

void GrayWidget::slotGrayEffect()
{
    m_complete = takeScreenshot( QX11Info::appScreen() );

    repaint(); //before show

    show();

    KWindowSystem::forceActiveWindow( winId() );
    KWindowSystem::setOnAllDesktops( winId(), true );
    KWindowSystem::setState( winId(), NET::KeepAbove );
    KWindowSystem::setState( winId(), NET::FullScreen );

    m_gray = new GrayOutEffect(this, &m_complete);
    connect(m_gray, SIGNAL(ready()), m_dialog, SLOT(showDialog()));
    m_gray->start();
}

void GrayWidget::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap( 0, 0, m_complete );
}

#include "graywidget.moc"
