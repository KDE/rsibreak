/*
    SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2005-2007, 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsirelaxpopup.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QProgressBar>
#include <QIcon>

#include <KColorScheme>
#include <QVBoxLayout>
#include <KLocalizedString>
#include <KConfigGroup>
#include <QHBoxLayout>
#include <KFormat>

RSIRelaxPopup::RSIRelaxPopup( QWidget *parent )
        : QObject( parent )
        , m_wasShown( false )
{
    m_popup = new PassivePopup( parent );

    QWidget *vbox = new QWidget( m_popup );
    QVBoxLayout *vboxVBoxLayout = new QVBoxLayout(vbox);
    vboxVBoxLayout->setContentsMargins(0, 0, 0, 0);
    vboxVBoxLayout->setSpacing( 5 );
    m_message = new QLabel( vbox );
    vboxVBoxLayout->addWidget(m_message);

    QWidget *hbox = new QWidget( vbox );
    QHBoxLayout *hboxHBoxLayout = new QHBoxLayout(hbox);
    hboxHBoxLayout->setContentsMargins(0, 0, 0, 0);
    vboxVBoxLayout->addWidget(hbox);
    hboxHBoxLayout->setSpacing( 15 );

    m_progress = new QProgressBar( hbox );
    hboxHBoxLayout->addWidget(m_progress);
    m_progress->setFormat( "%v" );
    m_progress->setRange( 0, 0 );

    m_skipbutton = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), i18n( "Skip Break" ), hbox );
    hboxHBoxLayout->addWidget(m_skipbutton);
    m_skipbutton->setToolTip( i18n( "Skip this break" ) );
    connect(m_skipbutton, &QPushButton::clicked, this, &RSIRelaxPopup::skip);

    m_postponebutton = new QPushButton( QIcon::fromTheme( "go-next" ), i18n( "Postpone Break" ), hbox );
    hboxHBoxLayout->addWidget(m_postponebutton);
    m_postponebutton->setToolTip( i18n( "Postpone this break" ) );
    connect(m_postponebutton, &QPushButton::clicked, this, &RSIRelaxPopup::postpone);
    
    m_lockbutton = new QPushButton( QIcon::fromTheme( "system-lock-screen" ), i18n( "Lock Screen" ), hbox );
    hboxHBoxLayout->addWidget(m_lockbutton);
    m_lockbutton->setToolTip( i18n( "Lock the session" ) );
    connect(m_lockbutton, &QPushButton::clicked, this, &RSIRelaxPopup::lock);

    m_popup->setTimeout( 0 ); // no auto close
    m_popup->setView( vbox );
    readSettings();
}

RSIRelaxPopup::~RSIRelaxPopup() {}

void RSIRelaxPopup::relax( int n, bool bigBreakNext )
{
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
        else if ( resetcount % 4 == 0 ) // flash regularly when the user keeps working
            flash();
    }

    if ( n > 0 ) {
        QString text = i18n( "Please relax for %1",
                             KFormat().formatSpelloutDuration( n * 1000 ) );

        if ( bigBreakNext )
            text.append( '\n' + i18n( "Note: next break is a big break" ) );

        m_message->setText( text );
        m_progress->setValue( n );

        if ( resetcount == 1 )
            m_popup->show();
    } else {
        m_popup->setVisible( false );
        resetcount = 0;
        m_wasShown = false;
    }
}

void RSIRelaxPopup::flash()
{
    if ( !m_useFlash )
        return;

    QTimer::singleShot( 500, this, &RSIRelaxPopup::unflash );
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
    KConfigGroup config = KSharedConfig::openConfig()->group( "Popup Settings" );
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

void RSIRelaxPopup::setSuspended( bool suspended )
{
    if ( suspended ) {
        m_wasShown = m_popup->isVisible();
        m_popup->hide();
    } else if (m_wasShown) {
        m_popup->show();
    }
}
