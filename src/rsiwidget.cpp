/*
   Copyright (C) 2005-2006,2009-2010 Tom Albers <toma@kde.org>
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
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

#include "rsiwidget.h"
#include "rsiwidgetadaptor.h"
#include "grayeffect.h"
#include "popupeffect.h"
#include "plasmaeffect.h"
#include "slideshoweffect.h"
#include "rsitimer.h"
#include "rsidock.h"
#include "rsirelaxpopup.h"
#include "rsiglobals.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>

#include <KLocalizedString>
#include <KMessageBox>
#include <KIconLoader>
#include <KNotification>
#include <QDBusInterface>
#include <QTemporaryFile>
#include <KConfigGroup>
#include <KSharedConfig>

#include <time.h>
#include <math.h>
#include <KFormat>

RSIObject::RSIObject( QWidget *parent )
        : QObject( parent ), m_effect( 0 ),
        m_useImages( false ), m_usePlasma( false ), m_usePlasmaRO( false )
{
    // Keep these 2 lines _above_ the messagebox, so the text actually is right.
    m_tray = new RSIDock( this );
    m_tray->setIconByName( "rsibreak0" );

    new RsiwidgetAdaptor( this );
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject( "/rsibreak", this );

    m_relaxpopup = new RSIRelaxPopup( 0 );
    connect(m_relaxpopup, &RSIRelaxPopup::lock, this, &RSIObject::slotLock);

    connect(m_tray, &RSIDock::configChanged, this, &RSIObject::readConfig);
    connect(m_tray, &RSIDock::configChanged, RSIGlobals::instance(), &RSIGlobals::slotReadConfig );
    connect(m_tray, &RSIDock::configChanged, m_relaxpopup, &RSIRelaxPopup::slotReadConfig);
    connect(m_tray, &RSIDock::suspend, m_relaxpopup, &RSIRelaxPopup::hide);

    srand( time( NULL ) );

    readConfig();

    setIcon( 0 );

    QTimer::singleShot( 2000, this, &RSIObject::slotWelcome );
}

RSIObject::~RSIObject()
{
    delete m_effect;
    delete RSIGlobals::instance();
}

void RSIObject::slotWelcome()
{
    if ( KMessageBox::shouldBeShownContinue( "dont_show_welcome_again_for_001" ) ) {
        KMessageBox::information( 0, i18n( "<p>Welcome to RSIBreak</p>\n<p>"
                                           "In your tray you can now see RSIBreak.</p>\n" )
                                  + i18n( "<p>When you right-click on that you will see a menu, from which "
                                          "you can go to the configuration for example.</p>\n<p>When you want to "
                                          "know when the next break is, hover over the icon.</p>\n<p>Use RSIBreak "
                                          "wisely.</p>" ), i18n( "Welcome" ), "dont_show_welcome_again_for_001" );
    }
}

void RSIObject::minimize()
{
    m_effect->deactivate();
}

void RSIObject::maximize()
{
    m_effect->activate();
}

void RSIObject::slotLock()
{
    m_effect->deactivate();
    m_timer->slotRestart();

    QDBusInterface lock( "org.freedesktop.ScreenSaver", "/ScreenSaver",
                         "org.freedesktop.ScreenSaver" );
    lock.call( "Lock" );
}

void RSIObject::setCounters( int timeleft )
{
    if ( timeleft > 0 ) {
        m_effect->setLabel( KFormat().formatSpelloutDuration( timeleft * 1000 ) );
    } else if ( m_timer->isSuspended() ) {
        m_effect->setLabel( i18n( "Suspended" ) );
    } else {
        m_effect->setLabel( QString() );
    }
}

void RSIObject::updateIdleAvg( double idleAvg )
{
    if ( idleAvg == 0.0 )
        setIcon( 0 );
    else if ( idleAvg > 0 && idleAvg < 30 )
        setIcon( 1 );
    else if ( idleAvg >= 30 && idleAvg < 60 )
        setIcon( 2 );
    else if ( idleAvg >= 60 && idleAvg < 90 )
        setIcon( 3 );
    else
        setIcon( 4 );
}

void RSIObject::setIcon( int level )
{
    QString newIcon = "rsibreak" +
                      ( m_timer->isSuspended() ? QString( "x" ) : QString::number( level ) );

    if ( newIcon != m_currentIcon ) {
        m_tray->setIconByName( newIcon );
        m_tray->setToolTipIconByName( newIcon );
        m_currentIcon = newIcon;
    }
}

// ------------------- Popup for skipping break ------------- //

void RSIObject::tinyBreakSkipped()
{
    if ( !m_showTimerReset )
        return;

    KNotification::event( "short timer reset",
                          i18n( "Timer for the short break has now been reset" ),
                          KIconLoader::global()->loadIcon( "rsibreak0", KIconLoader::Desktop ) );
}

void RSIObject::bigBreakSkipped()
{
    if ( !m_showTimerReset )
        return;

    KNotification::event( "timers reset",
                          i18n( "The timers have now been reset" ),
                          KIconLoader::global()->loadIcon( "rsibreak0", KIconLoader::Desktop ) );
}

//--------------------------- CONFIG ----------------------------//

void RSIObject::startTimer( bool idle )
{

    static bool first = true;

    if ( !first ) {
        qDebug() << "Current Timer: " << m_timer->metaObject()->className()
        << " wanted: " << ( idle ? "RSITimer" : "RSITimerNoIdle" ) << endl;

        if ( !idle && !strcmp( m_timer->metaObject()->className(), "RSITimerNoIdle" ) )
            return;

        if ( idle && !strcmp( m_timer->metaObject()->className(), "RSITimer" ) )
            return;

        qDebug() << "Switching timers";

        m_timer->exit();
        m_timer->deleteLater();
//        m_accel->remove("minimize");
    }

    if ( idle )
        m_timer = new RSITimer( this );
    else
        m_timer = new RSITimerNoIdle( this );

    m_timer->start();

    connect(m_timer, &RSITimer::breakNow, this, &RSIObject::maximize, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::updateWidget, this, &RSIObject::setCounters, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::updateToolTip, m_tray, &RSIDock::setCounters, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::updateIdleAvg, this, &RSIObject::updateIdleAvg, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::minimize, this, &RSIObject::minimize,  Qt::QueuedConnection );
    connect(m_timer, &RSITimer::relax, m_relaxpopup, &RSIRelaxPopup::relax, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::tinyBreakSkipped, this, &RSIObject::tinyBreakSkipped, Qt::QueuedConnection );
    connect(m_timer, &RSITimer::bigBreakSkipped, this, &RSIObject::bigBreakSkipped, Qt::QueuedConnection );

    connect(m_tray, &RSIDock::configChanged, m_timer, &RSITimer::slotReadConfig);
    connect(m_tray, &RSIDock::dialogEntered, m_timer, &RSITimer::slotStop);
    connect(m_tray, &RSIDock::dialogLeft, m_timer, &RSITimer::slotStart);
    connect(m_tray, &RSIDock::suspend, m_timer, &RSITimer::slotSuspended);

    connect(m_relaxpopup, &RSIRelaxPopup::skip, m_timer, &RSITimer::skipBreak);
    connect(m_relaxpopup, &RSIRelaxPopup::postpone, m_timer, &RSITimer::postponeBreak);

    first = false;
}

void RSIObject::readConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    m_showTimerReset = config.readEntry( "ShowTimerReset", false );

    m_relaxpopup->setSkipButtonHidden(
        config.readEntry( "HideMinimizeButton", false ) );
    m_relaxpopup->setLockButtonHidden(
        config.readEntry( "HideLockButton", false ) );
    m_relaxpopup->setPostponeButtonHidden(
        config.readEntry( "HidePostponeButton", false ) );

    m_usePlasma = config.readEntry( "UsePlasma", false );
    m_usePlasmaRO = config.readEntry( "UsePlasmaReadOnly", false );

    m_useImages = config.readEntry( "ShowImages", false );
    int slideInterval = config.readEntry( "SlideInterval", 10 );
    bool recursive =  config.readEntry( "SearchRecursiveCheck", false );
    bool showSmallImages = config.readEntry( "ShowSmallImagesCheck", true );
    QString path = config.readEntry( "ImageFolder" );

    bool timertype = config.readEntry( "UseNoIdleTimer", false );
    startTimer( !timertype );

    int effect =  config.readEntry( "Effect", 0 );

    delete m_effect;
    switch ( effect ) {
    case Plasma: {
        m_effect = new PlasmaEffect( 0 );
        m_effect->setReadOnly( m_usePlasmaRO );
        break;
    }
    case SlideShow: {
        SlideEffect* slide = new SlideEffect( 0 );
        slide->reset( path, recursive, showSmallImages, slideInterval );
        if ( slide->hasImages() )
            m_effect = slide;
        else {
            delete slide;
            m_effect = new GrayEffect( 0 );
        }
        break;
    }
    case Popup: {
        PopupEffect* effect = new PopupEffect( 0 );
        m_effect = effect;
        break;
    }
    case SimpleGray:
    default: {
        GrayEffect* effect = new GrayEffect( 0 );
        effect->setLevel( config.readEntry( "Graylevel", 80 ) );
        m_effect = effect;
        break;
    }
    }
    connect(m_effect, &BreakBase::skip, m_timer, &RSITimer::skipBreak);
    connect(m_effect, &BreakBase::lock, this, &RSIObject::slotLock);
    connect(m_effect, &BreakBase::postpone, m_timer, &RSITimer::postponeBreak);

    m_effect->showMinimize( !config.readEntry( "HideMinimizeButton", false ) );
    m_effect->showLock( !config.readEntry( "HideLockButton", false ) );
    m_effect->showPostpone( !config.readEntry( "HidePostponeButton", false ) );
    m_effect->disableShortcut( config.readEntry( "DisableAccel", false ) );
}
