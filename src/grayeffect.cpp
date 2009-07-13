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

#include "grayeffect.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include <KWindowSystem>
#include <KDebug>


GrayEffect::GrayEffect( QObject *parent )
        : BreakBase( parent )
{
    for (int i = 0; i < QApplication::desktop()->numScreens(); ++i) {
        GrayWidget* grayWidget = new GrayWidget( 0 );
        m_widgets.append( grayWidget );

        QRect rect = QApplication::desktop()->screenGeometry( i );
        grayWidget->move( rect.topLeft() );
        grayWidget->setGeometry( rect );

        KWindowSystem::forceActiveWindow( grayWidget->winId() );
        KWindowSystem::setState( grayWidget->winId(), NET::KeepAbove );
        KWindowSystem::setOnAllDesktops( grayWidget->winId(), true );
        KWindowSystem::setState( grayWidget->winId(), NET::FullScreen );

        kDebug() << "Created widget for screen" << i
                << "Position:" << rect.topLeft();
    }
    setReadOnly( true );
}

GrayEffect::~GrayEffect()
{
    qDeleteAll( m_widgets );
}

void GrayEffect::activate()
{
    foreach( GrayWidget* widget, m_widgets) {
        widget->show();
        widget->update();
    }
    BreakBase::activate();
}

void GrayEffect::deactivate()
{
    foreach( GrayWidget* widget, m_widgets)
        widget->hide();
    BreakBase::deactivate();
}

void GrayEffect::setLevel( int val )
{
    foreach( GrayWidget* widget, m_widgets)
        widget->setLevel( val );
}

GrayWidget::GrayWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup )
{
    setAutoFillBackground( false );
}

bool GrayWidget::event( QEvent *event )
{
    if ( event->type() == QEvent::Paint ) {
        kDebug();
        QPainter p( this );
        p.setCompositionMode( QPainter::CompositionMode_Source );
        p.fillRect( rect(), QColor( 0,0,0,180 ) );
    }
    return QWidget::event( event );
}

void GrayWidget::setLevel( int val )
{
    double level = 0;
    if ( val > 0 )
        level = ( double )val / 100;

    kDebug() << "New Value" << level;
    setWindowOpacity( level );
    update();
}

#include "grayeffect.moc"
