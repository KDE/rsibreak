/*
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2005-2007,2010 Tom Albers <toma@kde.org>
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

#include "rsirelaxpopup.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QProgressBar>

#include <KColorScheme>
#include <KVBox>
#include <KLocale>
#include <KDebug>
#include <KConfigGroup>
#include <KIconLoader>

RSIRelaxPopup::RSIRelaxPopup( QWidget *parent )
        : QWidget( parent )
{
    m_popup = new PassivePopup( parent );

    KVBox *vbox = new KVBox( m_popup );
    vbox->setSpacing( 5 );
    m_message = new QLabel( vbox );

    KHBox *hbox = new KHBox( vbox );
    hbox->setSpacing( 15 );

    m_progress = new QProgressBar( hbox );
    m_progress->setFormat( "%v" );
    m_progress->setRange( 0, 0 );

    m_skipbutton = new QPushButton( SmallIcon( "dialog-cancel" ), i18n( "Skip Break" ), hbox );
    m_skipbutton->setToolTip( i18n( "Skip this break" ) );
    connect( m_skipbutton, SIGNAL( clicked() ), SIGNAL( skip() ) );

    m_postponebutton = new QPushButton( SmallIcon( "go-next" ), i18n( "Postpone Break" ), hbox );
    m_postponebutton->setToolTip( i18n( "Postpone this break" ) );
    connect( m_postponebutton, SIGNAL( clicked() ), SIGNAL( postpone()) );
    
    m_lockbutton = new QPushButton( SmallIcon( "system-lock-screen" ), i18n( "Lock Screen" ), hbox );
    m_lockbutton->setToolTip( i18n( "Lock the session" ) );
    connect( m_lockbutton, SIGNAL( clicked() ), SIGNAL( lock() ) );

    m_popup->setTimeout( 0 ); // no auto close
    m_popup->setView( vbox );
    readSettings();
}

RSIRelaxPopup::~RSIRelaxPopup() {}

void RSIRelaxPopup::relax( int n, bool bigBreakNext )
{
    if ( !m_usePopup )
        return;

    /*
      Counts how many times a request for relax resets
      due to detected activity.
    */
    static int resetcount = 0;

    /*
        If n increases compared to the last call,
        we want a new request for a relax moment.
    */
    if ( n >= m_progress->value() ) {
        m_progress->setRange( 0, n );
        resetcount += 1;
        if ( n > m_progress->value() )
            flash();
        else if ( resetcount % 4 == 0 ) // flash regulary when the user keeps working
            flash();
    }

    if ( n > 0 ) {
        QString text = i18n( "Please relax for %1",
                             KGlobal::locale()->prettyFormatDuration( n * 1000 ) );

        if ( bigBreakNext )
            text.append( '\n' + i18n( "Note: next break is a big break" ) );

        m_message->setText( text );
        m_progress->setValue( n );

        if ( resetcount == 1 )
            m_popup->show();
    } else {
        m_popup->setVisible( false );
        resetcount = 0;
    }
}

void RSIRelaxPopup::flash()
{
    if ( !m_useFlash )
        return;

    QTimer::singleShot( 500, this, SLOT( unflash() ) );
    QPalette normal;
    normal.setColor( QPalette::Inactive, QPalette::WindowText,
                     KColorScheme( QPalette::Active, KColorScheme::Selection ).background().color() );
    normal.setColor( QPalette::Inactive, QPalette::Window,
                     KColorScheme( QPalette::Active, KColorScheme::Selection ).foreground().color() );
    m_popup->setPalette( normal );
}

void RSIRelaxPopup::unflash()
{
    QPalette normal;
    m_popup->setPalette( normal );
}

void RSIRelaxPopup::slotReadConfig()
{
    readSettings();
}

void RSIRelaxPopup::readSettings()
{
    KConfigGroup config = KGlobal::config()->group( "Popup Settings" );
    m_usePopup = config.readEntry( "UsePopup", true );
    m_useFlash = config.readEntry( "UseFlash", true );
}

void RSIRelaxPopup::setSkipButtonHidden( bool b )
{
    m_skipbutton->setHidden( b );
}

void RSIRelaxPopup::setLockButtonHidden( bool b )
{
    m_lockbutton->setHidden( b );
}

void RSIRelaxPopup::setPostponeButtonHidden( bool b )
{
    m_postponebutton->setHidden( b );
}
#include "rsirelaxpopup.moc"
