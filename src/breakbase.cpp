/*
   Copyright (C) 2009 Tom Albers <toma@kde.org>
   Copyright (C) 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

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

#include "breakbase.h"
#include "breakcontrol.h"

#include <KWindowSystem>

#include <QApplication>
#include <QDebug>
#include <QObject>
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>


BreakBase::BreakBase( QObject* parent )
        : QObject( parent ),  m_grayEffectOnAllScreens( 0 ), m_readOnly( false ),
        m_disableShortcut( false ), m_grayEffectOnAllScreensActivated( false )
{
    m_breakControl = new BreakControl( 0, Qt::Popup );
    m_breakControl->hide();
    m_breakControl->installEventFilter( this );
    connect(m_breakControl, &BreakControl::skip, this, &BreakBase::skip);
    connect(m_breakControl, &BreakControl::lock, this, &BreakBase::lock);
    connect(m_breakControl, &BreakControl::postpone, this, &BreakBase::postpone);
}

BreakBase::~BreakBase()
{
    delete m_grayEffectOnAllScreens;
    delete m_breakControl;
}

void BreakBase::activate()
{
    if ( m_grayEffectOnAllScreensActivated )
        m_grayEffectOnAllScreens->activate();

    m_breakControl->show();
    m_breakControl->setFocus();

    KWindowSystem::forceActiveWindow( m_breakControl->winId() );
    KWindowSystem::setOnAllDesktops( m_breakControl->winId(), true );
    KWindowSystem::setState( m_breakControl->winId(), NET::KeepAbove );
    KWindowSystem::setState( m_breakControl->winId(), NET::FullScreen );

    m_breakControl->grabKeyboard();
    m_breakControl->grabMouse();
}

void BreakBase::deactivate()
{
    if ( m_grayEffectOnAllScreensActivated )
        m_grayEffectOnAllScreens->deactivate();

    m_breakControl->releaseMouse();
    m_breakControl->releaseKeyboard();
    m_breakControl->hide();
}

bool BreakBase::eventFilter( QObject *obj, QEvent *event )
{
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>( event );
        qDebug() << "Ate key press" << keyEvent->key();
        if ( !m_disableShortcut && keyEvent->key() == Qt::Key_Escape ) {
            qDebug() << "Escape";
            emit skip();
        }
        return true;
    } else if ( m_readOnly &&
                ( event->type() == QEvent::MouseButtonPress ||
                  event->type() == QEvent::MouseButtonDblClick )
              ) {
        qDebug() << "Ate mouse click event";
        return true;
    } else {
        return QObject::eventFilter( obj, event );
    }
}

void BreakBase::setReadOnly( bool ro )
{
    m_readOnly = ro;
}

bool BreakBase::readOnly() const
{
    return m_readOnly;
}

void BreakBase::setLabel( const QString& text )
{
    m_breakControl->setText( text );
}

void BreakBase::showMinimize( bool show )
{
    m_breakControl->showMinimize( show );
}

void BreakBase::showLock( bool show )
{
    m_breakControl->showLock( show );
}

void BreakBase::showPostpone( bool show )
{
    m_breakControl->showPostpone( show );
}

void BreakBase::disableShortcut( bool disable )
{
    m_disableShortcut = disable;
}

void BreakBase::setGrayEffectOnAllScreens( bool on )
{
    m_grayEffectOnAllScreensActivated = on;
    delete m_grayEffectOnAllScreens;
    if ( on ) {
        m_grayEffectOnAllScreens = new GrayEffectOnAllScreens();
        m_grayEffectOnAllScreens->setLevel( 70 );
    }
}

void BreakBase::setGrayEffectLevel( int level )
{
    m_grayEffectOnAllScreens->setLevel( level );
}

void BreakBase::excludeGrayEffectOnScreen( QScreen *screen )
{
    m_grayEffectOnAllScreens->disable( screen );
}


// ------------------------ GrayEffectOnAllScreens -------------//

GrayEffectOnAllScreens::GrayEffectOnAllScreens()
{
    for ( QScreen *screen : QGuiApplication::screens() ) {
        GrayWidget* grayWidget = new GrayWidget( 0 );
        m_widgets.insert( screen, grayWidget );

        const QRect rect = screen->geometry();
        grayWidget->move( rect.topLeft() );
        grayWidget->setGeometry( rect );

        KWindowSystem::forceActiveWindow( grayWidget->winId() );
        KWindowSystem::setState( grayWidget->winId(), NET::KeepAbove );
        KWindowSystem::setOnAllDesktops( grayWidget->winId(), true );
        KWindowSystem::setState( grayWidget->winId(), NET::FullScreen );

        qDebug() << "Created widget for screen" << screen << "Position:" << rect.topLeft();
    }
}

GrayEffectOnAllScreens::~GrayEffectOnAllScreens()
{
    qDeleteAll( m_widgets.values() );
}

void GrayEffectOnAllScreens::disable( QScreen *screen )
{
    qDebug() << "Removing widget from screen" << screen;
    if ( !m_widgets.contains( screen ) )
        return;

    delete m_widgets.take( screen );
}

void GrayEffectOnAllScreens::activate()
{
    foreach( GrayWidget* widget, m_widgets ) {
        widget->show();
        widget->update();
    }
}

void GrayEffectOnAllScreens::deactivate()
{
    foreach( GrayWidget* widget, m_widgets ) {
        widget->hide();
    }
}

void GrayEffectOnAllScreens::setLevel( int val )
{
    foreach( GrayWidget* widget, m_widgets ) {
        widget->setLevel( val );
    }
}


//-------------------- GrayWidget ----------------------------//


GrayWidget::GrayWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup )
{
    setAutoFillBackground( false );
}

bool GrayWidget::event( QEvent *event )
{
    if ( event->type() == QEvent::Paint ) {
        qDebug() << "GrayWidget::event";
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

    qDebug() << "New Value" << level;
    setWindowOpacity( level );
    update();
}
