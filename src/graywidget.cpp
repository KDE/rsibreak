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

#include "graywidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include <KWindowSystem>
#include <KDebug>


GrayEffect::GrayEffect( QWidget *parent )
        : BreakBase( parent )
{
    m_grayWidget = new GrayWidget( 0 );
    KWindowSystem::forceActiveWindow( m_grayWidget->winId() );
    KWindowSystem::setOnAllDesktops( m_grayWidget->winId(), true );
    KWindowSystem::setState( m_grayWidget->winId(), NET::KeepAbove );
    KWindowSystem::setState( m_grayWidget->winId(), NET::FullScreen );

    setReadOnly( true );
}

void GrayEffect::activate()
{
    m_grayWidget->show();
    m_grayWidget->update();
    BreakBase::activate();
}

void GrayEffect::deactivate()
{
    m_grayWidget->hide();
    BreakBase::deactivate();
}

GrayWidget::GrayWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup )
{
    QRect rect = QApplication::desktop()->screenGeometry(
                     QApplication::desktop()->primaryScreen() );
    setGeometry( rect );
    setAutoFillBackground( false );
    setWindowOpacity(0.8);
}

bool GrayWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        kDebug();
        QPainter p(this);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(rect(), QColor(0,0,0,180));
    }

    return QWidget::event(event);
}
#include "graywidget.moc"
