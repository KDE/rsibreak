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

#include "breakbase.h"
#include "breakcontrol.h"

#include <KDebug>
#include <KWindowSystem>

#include <QApplication>
#include <QDesktopWidget>
#include <QObject>
#include <QPainter>
#include <QKeyEvent>


BreakBase::BreakBase( QObject* parent )
        : QObject( parent ),  m_grayEffectOnAllScreens( 0 ), m_readOnly( false ),
        m_disableShortcut( false ), m_grayEffectOnAllScreensActivated( false )
{
    m_parent = parent;
    m_breakControl = new BreakControl( 0, Qt::Popup );
    m_breakControl->hide();
    m_breakControl->installEventFilter( this );
    connect( m_breakControl, SIGNAL( skip() ), SIGNAL( skip() ) );
    connect( m_breakControl, SIGNAL( lock() ), SIGNAL( lock() ) );
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
        kDebug() << "Ate key press" << keyEvent->key();
        if ( !m_disableShortcut && keyEvent->key() == Qt::Key_Escape ) {
            kDebug() << "Escape";
            emit skip();
        }
        return true;
    } else if ( m_readOnly &&
                ( event->type() == QEvent::MouseButtonPress ||
                  event->type() == QEvent::MouseButtonDblClick )
              ) {
        kDebug() << "Ate mouse click event";
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

void BreakBase::disableShortcut( bool disable )
{
    m_disableShortcut = disable;
}

void BreakBase::setGrayEffectOnAllScreens( bool on )
{
    if (m_grayEffectOnAllScreensActivated == on )
        return;

    m_grayEffectOnAllScreensActivated = on;
    delete m_grayEffectOnAllScreens;
    m_grayEffectOnAllScreens = new GrayEffectOnAllScreens();
    m_grayEffectOnAllScreens->setLevel( 70 );
}

void BreakBase::setGrayEffectLevel( int level )
{
    m_grayEffectOnAllScreens->setLevel( level );
}

void BreakBase::excludeGrayEffectOnScreen( int screen )
{
    m_grayEffectOnAllScreens->disable( screen );
}


// ------------------------ GrayEffectOnAllScreens -------------//

GrayEffectOnAllScreens::GrayEffectOnAllScreens()
{
    for (int i = 0; i < QApplication::desktop()->numScreens(); ++i) {
        GrayWidget* grayWidget = new GrayWidget( 0 );
        m_widgets.insert(i, grayWidget );

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
}

GrayEffectOnAllScreens::~GrayEffectOnAllScreens()
{
    qDeleteAll( m_widgets.values() );
}

void GrayEffectOnAllScreens::disable( int screen )
{
    kDebug() << "Removing widget from screen" << screen;
    if ( !m_widgets.contains( screen ) )
        return;

    delete m_widgets.value( screen );
    m_widgets.remove( screen );
}

void GrayEffectOnAllScreens::activate()
{
    foreach( GrayWidget* widget, m_widgets.values() ) {
        widget->show();
        widget->update();
    }
}

void GrayEffectOnAllScreens::deactivate()
{
    foreach( GrayWidget* widget, m_widgets.values() )
        widget->hide();
}

void GrayEffectOnAllScreens::setLevel( int val )
{
    foreach( GrayWidget* widget, m_widgets.values() )
        widget->setLevel( val );
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

#include "breakbase.moc"
