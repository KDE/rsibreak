/* This file is part of the KDE project
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>
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

#include "rsiwidget.h"
#include "rsiwidgetadaptor.h"
#include "grayeffect.h"
#include "popupeffect.h"
#include "plasmaeffect.h"
#include "slideshoweffect.h"
#include "rsitimer_dpms.h"
#include "rsitimer.h"
#include "rsidock.h"
#include "rsirelaxpopup.h"
#include "rsitooltip.h"
#include "rsiglobals.h"

#include <config-rsibreak.h>

#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>

#include <KLocale>
#include <KApplication>
#include <KMessageBox>
#include <KIconLoader>
#include <KNotification>
#include <KSystemTrayIcon>
#include <KTemporaryFile>
#include <KConfigGroup>
#include <KDebug>

#include <time.h>
#include <math.h>

RSIObject::RSIObject( QWidget *parent )
        : QObject( parent ), m_effect( 0 ),
        m_useImages( false ), m_usePlasma( false ), m_usePlasmaRO( false )
{
    // Keep these 3 lines _above_ the messagebox, so the text actually is right.
    m_tray = new RSIDock( parent );
    m_tray->setIcon( KSystemTrayIcon::loadIcon( "rsibreak0" ) );
    m_tray->show();

    m_tooltip = new RSIToolTip( 0, m_tray );
    connect( m_tray, SIGNAL( showToolTip() ), m_tooltip, SLOT( showToolTip() ) );

    m_relaxpopup = new RSIRelaxPopup( 0, m_tray );
    connect( m_relaxpopup, SIGNAL( lock() ), SLOT( slotLock() ) );

    connect( m_tray, SIGNAL( configChanged( bool ) ), SLOT( readConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), RSIGlobals::instance(),
             SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), m_relaxpopup,
             SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_tooltip,
             SLOT( setSuspended( bool ) ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_relaxpopup, SLOT( hide() ) );

    srand( time( NULL ) );

    readConfig();

    setIcon( 0 );

    QTimer::singleShot( 1000, this, SLOT( slotWelcome() ) );
}

RSIObject::~RSIObject()
{
    delete m_tray;
    delete m_effect;
    delete RSIGlobals::instance();
}

QPixmap RSIObject::takeScreenshot( RSIDock* tray )
{
    // taken from Akregator http://lxr.kde.org/source/KDE/kdepim/akregator/src/trayicon.cpp#70
    //     Copyright (C) 2004 Stanislav Karchebny <Stanislav.Karchebny@kdemail.net>

    const QRect rect = tray->geometry();
    const QPoint g = rect.topLeft();
    int desktopWidth  = kapp->desktop()->width();
    int desktopHeight = kapp->desktop()->height();
    int tw = rect.width();
    int th = rect.height();
    int w = desktopWidth / 4;
    int h = desktopHeight / 9;
    int x = g.x() + tw/2 - w/2; // Center the rectange in the systray icon
    int y = g.y() + th/2 - h/2;
    if ( x < 0 )
        x = 0; // Move the rectangle to stay in the desktop limits
    if ( y < 0 )
        y = 0;
    if ( x + w > desktopWidth )
        x = desktopWidth - w;
    if ( y + h > desktopHeight )
        y = desktopHeight - h;

    // Grab the desktop and draw a circle around the icon:
    QPixmap shot = QPixmap::grabWindow( QApplication::desktop()->winId(), x, y, w, h );
    QPainter painter( &shot );
    painter.setRenderHint( QPainter::Antialiasing );
    const int MARGINS = 6;
    const int WIDTH   = 3;
    int ax = g.x() - x - MARGINS -1;
    int ay = g.y() - y - MARGINS -1;
    painter.setPen( QPen( Qt::red/*KApplication::palette().active().highlight()*/, WIDTH ) );
    painter.drawArc( ax, ay, tw + 2*MARGINS, th + 2*MARGINS, 0, 16*360 );
    painter.end();

    // Paint the border
    const int BORDER = 1;
    QPixmap finalShot( w + 2*BORDER, h + 2*BORDER );
    finalShot.fill( KApplication::palette().color( QPalette::Foreground ) );
    painter.begin( &finalShot );
    painter.drawPixmap( BORDER, BORDER, shot );
    painter.end();
    return finalShot;
    // end taken....
}

void RSIObject::slotWelcome()
{
    if ( KMessageBox::shouldBeShownContinue( "dont_show_welcome_again_for_001" ) ) {
        QString tempfile = takeScreenshotOfTrayIcon();
        KMessageBox::information( 0, i18n( "<p>Welcome to RSIBreak</p><p>"
                                           "In your tray you can now see RSIBreak:</p>" )
                                  + "<p><center><img source=\"" + tempfile + "\"></center></p>"
                                  + i18n( "<p>When you right-click on that you will see a menu, from which "
                                          "you can go to the configuration for example.</p><p>When you want to "
                                          "know when the next break is, hover over the icon.</p><p>Use RSIBreak "
                                          "wisely.</p>" ), i18n( "Welcome" ), "dont_show_welcome_again_for_001" );
    }
}

void RSIObject::showWhereIAm()
{
    QString tempfile = takeScreenshotOfTrayIcon();
    KMessageBox::information( 0,
                              i18n( "<p>RSIBreak is already running</p><p>It is located here:</p>" )
                              + "<p><center><img source=\"" + tempfile + "\"></center></p><p>",
                              i18n( "Already Running" ) );
}

QString RSIObject::takeScreenshotOfTrayIcon()
{
    // Process the events else the icon will not be there and the screenie will fail!
    kapp->processEvents();

    //TODO: find the tray window.
    QPixmap screenshot = takeScreenshot( m_tray );

    QString filename;
    KTemporaryFile* tmpfile = new KTemporaryFile;
    tmpfile->setAutoRemove( false );
    if ( tmpfile->open() ) {
        filename = tmpfile->fileName();
        screenshot.save( tmpfile, "png" );
        tmpfile->close();
    }
    return filename;
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
    kDebug();
    m_effect->deactivate();

    QDBusInterface lock( "org.freedesktop.ScreenSaver", "/ScreenSaver",
                         "org.freedesktop.ScreenSaver" );
    lock.call( "Lock" );
}

void RSIObject::setCounters( int timeleft )
{
    if ( timeleft > 0 ) {
        int minutes = ( int )floor( timeleft / 60 );
        int seconds = timeleft - ( minutes * 60 );
        QString cdString;

        if ( minutes > 0 && seconds > 0 ) {
            cdString = ki18nc( "minutes:seconds", "%1:%2" )
                       .subs( minutes ).subs( seconds, 2, 10, QChar( '0' ) ).toString();
        } else if ( minutes == 0 && seconds > 0 ) {
            cdString = QString::number( seconds );
        } else if ( minutes > 0 && seconds == 0 ) {
            cdString = i18nc( "minutes:seconds", "%1:00", minutes );
        }

        m_effect->setLabel( cdString );
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
        QIcon dockIcon = KSystemTrayIcon::loadIcon( newIcon );
        m_tray->setIcon( dockIcon );

        QPixmap toolPixmap = KIconLoader::global()->loadIcon( newIcon, KIconLoader::Desktop );
        m_currentIcon = newIcon;
        m_tooltip->setPixmap( toolPixmap );
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
        kDebug() << "Current Timer: " << m_timer->metaObject()->className()
        << " wanted: " << ( idle ? "RSITimer" : "RSITimerNoIdle" ) << endl;

        if ( !idle && !strcmp( m_timer->metaObject()->className(), "RSITimerNoIdle" ) )
            return;

        if ( idle && !strcmp( m_timer->metaObject()->className(), "RSITimer" ) )
            return;

        kDebug() << "Switching timers";

        m_timer->deleteLater();
//        m_accel->remove("minimize");
    }

    if ( idle )
        m_timer = new RSITimer( this );
    else
        m_timer = new RSITimerNoIdle( this );

    m_timer->start();

    connect( m_timer, SIGNAL( breakNow() ), SLOT( maximize() ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( updateWidget( int ) ),
             SLOT( setCounters( int ) ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( updateToolTip( int, int ) ),
             m_tooltip, SLOT( setCounters( int, int ) ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( updateIdleAvg( double ) ), SLOT( updateIdleAvg( double ) ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( minimize() ), SLOT( minimize() ),  Qt::QueuedConnection );
    connect( m_timer, SIGNAL( relax( int, bool ) ), m_relaxpopup, SLOT( relax( int, bool ) ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( relax( int, bool ) ), m_tooltip, SLOT( hide() ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( tinyBreakSkipped() ), SLOT( tinyBreakSkipped() ), Qt::QueuedConnection );
    connect( m_timer, SIGNAL( bigBreakSkipped() ), SLOT( bigBreakSkipped() ), Qt::QueuedConnection );

    connect( m_tray, SIGNAL( configChanged( bool ) ), m_timer, SLOT( slotReadConfig( bool ) ) );
    connect( m_tray, SIGNAL( dialogEntered() ), m_timer, SLOT( slotStop() ) );
    connect( m_tray, SIGNAL( dialogLeft() ), m_timer, SLOT( slotStart() ) );
    connect( m_tray, SIGNAL( breakRequest() ), m_timer, SLOT( slotRequestBreak() ) );
    connect( m_tray, SIGNAL( debugRequest() ), m_timer, SLOT( slotRequestDebug() ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_timer, SLOT( slotSuspended( bool ) ) );

    connect( m_relaxpopup, SIGNAL( skip() ), m_timer, SLOT( skipBreak() ) );

    first = false;
}

void RSIObject::readConfig()
{
    KConfigGroup config = KGlobal::config()->group( "General Settings" );
    m_showTimerReset = config.readEntry( "ShowTimerReset", false );

    m_relaxpopup->setSkipButtonHidden(
        config.readEntry( "HideMinimizeButton", false ) );

    m_usePlasma = config.readEntry( "UsePlasma", false );
    m_usePlasmaRO = config.readEntry( "UsePlasmaReadOnly", false );

    m_useImages = config.readEntry( "ShowImages", false );
    int slideInterval = config.readEntry( "SlideInterval", 10 );
    bool recursive =
        config.readEntry( "SearchRecursiveCheck", false );
    QString path = config.readEntry( "ImageFolder" );

    bool timertype = config.readEntry( "UseNoIdleTimer", false );
    startTimer( !timertype );

    int effect =  config.readEntry( "Effect", 0 );

    delete m_effect;
    switch ( effect ) {
    case Plasma: {
        m_effect = new PlasmaEffect( 0 );
        m_effect ->setReadOnly( m_usePlasmaRO );
        break;
    }
    case SlideShow: {
        SlideEffect* slide = new SlideEffect( 0 );
        slide->reset( path, recursive, slideInterval );
        if ( slide->hasImages() )
            m_effect = slide;
        else
            delete slide;
        m_effect = new GrayEffect( 0 );
        break;
    }
    case Popup: {
        PopupEffect* effect = new PopupEffect( 0 );
        effect->setTray( m_tray );
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
    connect( m_effect, SIGNAL( skip() ), m_timer, SLOT( skipBreak() ) );
    connect( m_effect, SIGNAL( lock() ), this, SLOT( slotLock() ) );

    m_effect->showMinimize( !config.readEntry( "HideMinimizeButton", false ) );
    m_effect->disableShortcut( config.readEntry( "DisableAccel", false ) );
}

#include "rsiwidget.moc"
