/* This file is part of the KDE project
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>

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

#include <qpushbutton.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qcursor.h>
#include <qpainter.h>

#include "config.h"

#include <kwin.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaccel.h>
#include <kdebug.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kpixmap.h>
#include <kimageeffect.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "rsiwidget.h"
#include "rsitimer.h"
#include "rsidock.h"
#include "rsirelaxpopup.h"
#include "rsitooltip.h"
#include "rsiglobals.h"

RSIWidget::RSIWidget( QWidget *parent, const char *name )
    : QWidget( parent, name, WType_Popup),
     m_currentY( 0 )
{

    // Keep these 3 lines _above_ the messagebox, so the text actually is right.
    m_tray = new RSIDock(this,"Tray Item");
    m_tray->setPixmap( KSystemTray::loadIcon("rsibreak0") );
    m_tray->show();

    if (KMessageBox::shouldBeShownContinue("dont_show_welcome_again_for_001"))
    {
        // Process the events else the icon will not be there and the screenie will fail!
        kapp->processEvents();

        // ********************************************************************************
        // This block is copied from Konversation - KonversationMainWindow::queryClose()
        // The part about the border is copied from  KSystemTray::displayCloseMessage()
        //
        // Compute size and position of the pixmap to be grabbed:
        QPoint g = m_tray->mapToGlobal( m_tray-> pos() );
        kdDebug() << "TRAY: "<< m_tray->pos() << g << m_tray->isVisible() << endl;
        int desktopWidth  = kapp->desktop()->width();
        int desktopHeight = kapp->desktop()->height();
        int tw = m_tray->width();
        int th = m_tray->height();
        int w = desktopWidth / 4;
        int h = desktopHeight / 9;
        int x = g.x() + tw/2 - w/2;               // Center the rectange in the systray icon
        int y = g.y() + th/2 - h/2;
        if ( x < 0 )                 x = 0;       // Move the rectangle to stay in the desktop limits
        if ( y < 0 )                 y = 0;
        if ( x + w > desktopWidth )  x = desktopWidth - w;
        if ( y + h > desktopHeight ) y = desktopHeight - h;

        // Grab the desktop and draw a circle arround the icon:
        QPixmap shot = QPixmap::grabWindow( qt_xrootwin(),  x,  y,  w,  h );
        QPainter painter( &shot );
        const int MARGINS = 6;
        const int WIDTH   = 3;
        int ax = g.x() - x - MARGINS -1;
        int ay = g.y() - y - MARGINS -1;
        painter.setPen(  QPen( Qt::red,  WIDTH ) );
        painter.drawArc( ax,  ay,  tw + 2*MARGINS,  th + 2*MARGINS,  0,  16*360 );
        painter.end();

        // Then, we add a border arround the image to make it more visible:
        QPixmap finalShot(w + 2, h + 2);
        finalShot.fill(KApplication::palette().active().foreground());
        painter.begin(&finalShot);
        painter.drawPixmap(1, 1, shot);
        painter.end();

        // Associate source to image and show the dialog:
        QMimeSourceFactory::defaultFactory()->setPixmap( "systray_shot", finalShot );

        // End copied block
        // ********************************************************************************

        KMessageBox::information(parent,
                             i18n("<p>Welcome to RSIBreak<p><p>"
                                  "In your tray you can now see RSIBreak: ")
                             + "<p><center><img source=\"systray_shot\"></center></p><p>"
                             + i18n("When you right-click on that you will see "
                                  "a menu, from which you can go to the "
                                  "configuration for example.<p>When you want to "
                                  "know when the next break is, hover "
                                  "over the icon.<p>Use RSIBreak wisely."),
                             i18n("Welcome"),
                             "dont_show_welcome_again_for_001");

        // we dont need to show the blurp about the changes in 0.5 for new users.
        KMessageBox::saveDontShowAgainContinue("dont_show_welcome_again_for_050");
    }
    else
        KMessageBox::information(parent,
                                 i18n("Changes in RSIBreak 0.5.0\n\n"
                                      "In this version we have changed the way "
                                      "the timers work. Instead of indicating "
                                      "the number of short breaks you want to "
                                      "take before taking a long break, you "
                                      "can now setup how often, in minutes, "
                                      "you want a lomg break.\nPlease review "
                                      "your current settings.\n\n"),
                                 i18n("Welcome"),
                                 "dont_show_welcome_again_for_050");

    m_backgroundimage.resize(QApplication::desktop()->width(),
                             QApplication::desktop()->height());
    setBackgroundMode( QWidget::NoBackground );
    setGeometry( QApplication::desktop()->geometry() );
    move(0,0);

    m_tooltip = new RSIToolTip( m_tray, "Tooltip" );
    connect( m_tray, SIGNAL( showToolTip() ), m_tooltip, SLOT( show() ) );
    connect( m_tray, SIGNAL( hideToolTip() ), m_tooltip, SLOT( hide() ) );

    m_relaxpopup = new RSIRelaxPopup(m_tray);
    connect( m_relaxpopup, SIGNAL( lock() ), SLOT( slotLock() ) );

    m_timer = new RSITimer(this,"Timer");
    connect( m_timer, SIGNAL( breakNow() ), SLOT( maximize() ) );
    connect( m_timer, SIGNAL( updateWidget( int ) ),
             SLOT( setCounters( int ) ) );
    connect( m_timer, SIGNAL( updateToolTip( int, int ) ),
             m_tooltip, SLOT( setCounters( int, int ) ) );
    connect( m_timer, SIGNAL( updateIdleAvg( double ) ), SLOT( updateIdleAvg( double ) ) );
    connect( m_timer, SIGNAL( minimize( bool ) ), SLOT( minimize( bool ) ) );
    connect( m_timer, SIGNAL( relax( int ) ), m_relaxpopup, SLOT( relax( int ) ) );
    connect( m_timer, SIGNAL( relax( int ) ), m_tooltip, SLOT( hide() ) );
    connect( m_timer, SIGNAL( relax( int ) ), m_tray, SLOT( relaxEntered( int ) ) );

    connect( m_timer, SIGNAL( tinyBreakSkipped() ), SLOT( tinyBreakSkipped() ) );
    connect( m_timer, SIGNAL( bigBreakSkipped() ), SLOT( bigBreakSkipped() ) );
    connect( m_timer, SIGNAL( skipBreakEnded() ), SLOT( skipBreakEnded() ) );

    connect( m_tray, SIGNAL( quitSelected() ), kapp, SLOT( quit() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), SLOT( readConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), RSIGlobals::instance(), SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), m_timer, SLOT( slotReadConfig( bool ) ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), m_relaxpopup, SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( dialogEntered() ), m_timer, SLOT( slotStopNoImage( ) ) );
    connect( m_tray, SIGNAL( dialogLeft() ), m_timer, SLOT( slotStartNoImage() ) );
    connect( m_tray, SIGNAL( breakRequest() ), m_timer, SLOT( slotRequestBreak() ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_tooltip, SLOT( setSuspended( bool ) ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_timer, SLOT( slotSuspended( bool ) ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_relaxpopup, SLOT( hide() ) );

    m_dcopIface = new DCOPIface(this, "actions");
    connect( m_dcopIface, SIGNAL( signalSuspend( bool) ),
             m_tooltip, SLOT( setSuspended( bool ) ) );
    connect( m_dcopIface, SIGNAL( signalSuspend( bool) ),
             m_timer, SLOT( slotSuspended( bool ) ) );
    connect( m_dcopIface, SIGNAL( signalSuspend( bool) ),
             m_relaxpopup, SLOT( setVisible( bool ) ) );
    connect( m_dcopIface, SIGNAL( signalDoTinyBreak() ),
             m_timer, SLOT( slotRequestTinyBreak() ) );
    connect( m_dcopIface, SIGNAL( signalDoBigBreak() ),
             m_timer, SLOT( slotRequestBigBreak() ) );

    setIcon( 0 );
    srand ( time(NULL) );

    QBoxLayout *topLayout = new QVBoxLayout( this, 5);

    m_countDown = new QLabel(this);
    m_countDown->setAlignment( AlignCenter );
    m_countDown->setBackgroundOrigin(QWidget::ParentOrigin);
    topLayout->addWidget(m_countDown);

    topLayout->addStretch(5);

    QBoxLayout *buttonRow = new QHBoxLayout( topLayout );
    m_miniButton = new QPushButton( i18n("Skip"), this );
    buttonRow->addWidget( m_miniButton );
    m_lockButton = new QPushButton( i18n("Lock desktop"), this );
    buttonRow->addWidget( m_lockButton );
    buttonRow->addStretch(10);

    // These two connects only get used when there the user locks and
    // unlocks before the break is over. The lock releases the mouse
    // and keyboard, and that way the event methods are not called.
    connect(m_miniButton, SIGNAL( clicked() ), m_timer, SLOT( skipBreak() ) );
    connect(m_lockButton, SIGNAL( clicked() ), SLOT( slotLock() ) );

    m_accel = new KAccel(this);
    m_accel->insert("minimize", i18n("Skip"),
                    i18n("Abort a break"),Qt::Key_Escape,
                    m_timer, SLOT( skipBreak() ));


    m_timer_slide = new QTimer(this);
    connect(m_timer_slide, SIGNAL(timeout()),  SLOT(slotNewSlide()));

    m_grab = new QTimer(this);
    connect(m_grab, SIGNAL(timeout()),  SLOT(slotGrab()));

    readConfig();

    // if there are no images found, the break will appear in black.
    // if the text color is black (default) then change that.
    if (m_files.count() == 0 &&
        m_countDown->paletteForegroundColor() == Qt::black)
        m_countDown->setPaletteForegroundColor( Qt::white );
}

RSIWidget::~RSIWidget()
{
    delete RSIGlobals::instance();
}

void RSIWidget::minimize( bool newImage )
{
    kdDebug() << "Entering RSIWidget::Minimize" << endl;

    // stop the m_grab, if the break is ESCaped during the
    // first second, slotGrab is called after the widget
    // is minimized again.
    m_grab->stop();

    m_timer_slide->stop();
    releaseKeyboard();
    releaseMouse();
    hide();
    setBackgroundMode( QWidget::NoBackground );
    if (newImage)
        loadImage();
}

void RSIWidget::maximize()
{
    kdDebug() << "Entering RSIWidget::Maximize" << endl;

    show(); // Keep it above the KWin calls.
    KWin::forceActiveWindow(winId());
    KWin::setOnAllDesktops(winId(),true);
    KWin::setState(winId(), NET::KeepAbove);
    KWin::setState(winId(), NET::FullScreen);

    // prevent that users accidently press this button while
    // they were writing text when the break appears
    m_miniButton->clearFocus();
    m_lockButton->clearFocus();

    // Small delay for grabbing keyboard and mouse, because
    // it will not grab when widget not visible
    m_grab->start(1000, true);

    // If there are no images found, we gray the screen and wait....
    if (m_files.count() == 0 || !m_useImages)
    {
        m_currentY=0;
        QTimer::singleShot( 10, this, SLOT( slotGrayEffect() ) );
    }
    else
    {
        if (m_slideInterval>0)
            m_timer_slide->start( m_slideInterval*1000 );

        bitBlt( this, 0, 0, &m_backgroundimage );
    }
}

void RSIWidget::loadImage()
{
    if (m_files.count() == 0)
        return;

    // reset if all images are shown
    if (m_files_done.count() == m_files.count())
        m_files_done.clear();

    // get a not yet used number
    int j = (int) ((m_files.count()) * (rand() / (RAND_MAX + 1.0)));
    while (m_files_done.findIndex( QString::number(j) ) != -1)
        j = (int) ((m_files.count()) * (rand() / (RAND_MAX + 1.0)));

    // Use that number to load the right image
    m_files_done.append(QString::number(j));

    kdDebug() << "Loading: " << m_files[j] <<
                    "( " << j << " / "  << m_files.count() << " ) " << endl;

    // Base the size on the size of the screen, for xinerama.
    QRect size = QApplication::desktop()->screenGeometry(
                        QApplication::desktop()->primaryScreen() );

    QImage m = QImage( m_files[ j ]).smoothScale( size.width(),
                                                  size.height(),
                                                  QImage::ScaleMax);

    if (m.isNull())
        return;

    if (!m_backgroundimage.convertFromImage(m))
        kdWarning() << "Failed to set new background image" << endl;
}


void RSIWidget::findImagesInFolder(const QString& folder)
{
    kdDebug() << "Looking for pictures in " << folder << endl;

    if ( folder.isNull() )
        return;

    QDir dir( folder);

    if ( !dir.exists() || !dir.isReadable() )
    {
        kdWarning() << "Folder does not exist or is not readable: "
                << folder << endl;
        return;
    }

    // TODO: make an automated filter, maybe with QImageIO.
    QString ext("*.png *.jpg *.jpeg *.tif *.tiff *.gif *.bmp *.xpm *.ppm *.pnm *.xcf *.pcx");
    dir.setNameFilter(ext + ' ' + ext.upper());
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks );
    dir.setMatchAllDirs ( true );

    const QFileInfoList *list = dir.entryInfoList();
    if (!list)
        return;

    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    while ( (fi = it.current()) != 0 )
    {
        if ( fi->isFile())
            m_files.append(fi->filePath());
        else if (fi->isDir() && m_searchRecursive &&
                 fi->fileName() != "." &&  fi->fileName() != "..")
            findImagesInFolder(fi->absFilePath());
        ++it;
    }
}

// This slot is copied from KDE's logout screen.
// from various authors found in:
// branches/KDE/3.5/kdebase/ksmserver/shutdowndlg.cpp
void RSIWidget::slotGrayEffect()
{
    if ( m_currentY >= height() ) {
        if ( backgroundMode() == QWidget::NoBackground ) {
            setBackgroundMode( QWidget::NoBackground );
            setBackgroundPixmap( m_backgroundimage );
        }
        return;
    }

    KPixmap pixmap;
    pixmap = QPixmap::grabWindow( qt_xrootwin(), 0, m_currentY, width(), 10 );
    QImage image = pixmap.convertToImage();
    KImageEffect::blend( Qt::black, image, 0.4 );
    KImageEffect::toGray( image, true );
    pixmap.convertFromImage( image );
    bitBlt( this, 0, m_currentY, &pixmap );
    bitBlt( &m_backgroundimage, 0, m_currentY, &pixmap );
    m_currentY += 10;
    QTimer::singleShot( 1, this, SLOT( slotGrayEffect() ) );
}

// -------------------------- SLOTS ------------------------//

void RSIWidget::slotNewSlide()
{
    kdDebug() << "Entering RSIWidget::slotNewSlide" << endl;

    if (m_files.count() == 1 || !m_useImages)
        return;

    loadImage();
    bitBlt( this, 0, 0, &m_backgroundimage );
}

void RSIWidget::slotLock()
{
    kdDebug() << "Entering RSIWidget::slotLock" << endl;

    m_timer_slide->stop();
    releaseKeyboard();
    releaseMouse();

    QCString appname( "kdesktop" );
    int rsibreak_screen = qt_xscreen();
    if ( rsibreak_screen )
        appname.sprintf("kdesktop-screen-%d", rsibreak_screen );
    kapp->dcopClient()->send(appname, "KScreensaverIface", "lock()", "");
}

void RSIWidget::slotGrab()
{
    grabMouse( QCursor( ArrowCursor ) );
    grabKeyboard();
}

void RSIWidget::setCounters( int timeleft )
{
    if (timeleft > 0)
    {
        int minutes = (int)floor(timeleft/60);
        int seconds  = timeleft-(minutes*60);
        QString cdString;

        if (minutes > 0 && seconds > 0)
        {
            cdString = i18n("minutes:seconds","%1:%2")
                    .arg( QString::number( minutes ))
                    .arg( QString::number( seconds).rightJustify(2,'0'));
        }
        else if ( minutes == 0 && seconds > 0 )
        {
            cdString = QString::number( seconds );
        }
        else if ( minutes >0 && seconds == 0 )
        {
            cdString = i18n("minutes:seconds","%1:00")
                    .arg( minutes );
        }

        m_countDown->setText( cdString );

    }
    else if ( m_timer->isSuspended() )
    {
        m_countDown->setText( i18n("Suspended") );
    }
    else
    {
        m_countDown->setText (QString::null );
    }
}

void RSIWidget::updateIdleAvg( double idleAvg )
{
    if ( idleAvg == 0.0 )
        setIcon( 0 );
    else if ( idleAvg >0 && idleAvg<30 )
        setIcon( 1 );
    else if ( idleAvg >=30 && idleAvg<60 )
        setIcon( 2 );
    else if ( idleAvg >=60 && idleAvg<90 )
        setIcon( 3 );
    else
        setIcon( 4 );
}

void RSIWidget::setIcon(int level)
{
    static QString currentIcon;
    static KIconLoader il;
    QString newIcon = "rsibreak" +
                      ( m_timer->isSuspended() ? QString("x") : QString::number(level) );

    if (newIcon != currentIcon)
    {
        QPixmap dockPixmap = KSystemTray::loadIcon( newIcon );
        QPixmap toolPixmap = il.loadIcon( newIcon, KIcon::Desktop );
        currentIcon = newIcon;
        m_tray->setPixmap( dockPixmap );
        m_tooltip->setPixmap( toolPixmap );
    }
}

// ------------------- Popup for skipping break ------------- //

void RSIWidget::tinyBreakSkipped()
{
    if (!m_showTimerReset)
        return;

    m_tooltip->setText( i18n("Timer for the short break has now been reset"));
    breakSkipped();
}

void RSIWidget::bigBreakSkipped()
{
    if (!m_showTimerReset)
        return;

    m_tooltip->setText( i18n("The timers have now been reset"));
    breakSkipped();
}

void RSIWidget::breakSkipped()
{
    static KIconLoader il;

    disconnect( m_timer, SIGNAL( updateToolTip( int, int ) ),
                m_tooltip, SLOT( setCounters( int, int ) ) );

    m_tooltip->setPixmap( il.loadIcon( "rsibreak0", KIcon::Desktop ) );
    m_tooltip->setTimeout(0); // autoDelete is false, but after the ->show() it still
                              // gets hidden after the timeout. Setting to 0 helps.
    m_tooltip->show();
}

void RSIWidget::skipBreakEnded()
{
    if (!m_showTimerReset)
        return;

    connect( m_timer, SIGNAL( updateToolTip( int, int ) ),
             m_tooltip, SLOT( setCounters( int, int ) ) );
    m_tooltip->hide();
}

// ----------------------------- EVENTS -----------------------//

void RSIWidget::closeEvent( QCloseEvent * )
{
    hide();
}

void RSIWidget::mousePressEvent( QMouseEvent * e )
{
    kdDebug() << e->pos() << " is within " <<  m_miniButton->geometry() << endl;

    if (e->button() != LeftButton)
        return;

    if ( m_miniButton->geometry().contains(e->pos()))
        m_miniButton->setDown( true );

    else if (m_lockButton->geometry().contains(e->pos()))
        m_lockButton->setDown( true );
}

void RSIWidget::mouseReleaseEvent( QMouseEvent * e )
{
    kdDebug() << e->pos() << " is within " <<  m_miniButton->geometry() << endl;
    if ( m_miniButton->geometry().contains(e->pos()))
    {
        m_miniButton->setDown( false );
        m_timer->skipBreak();
    }

    else if (m_lockButton->geometry().contains(e->pos()))
    {
        m_lockButton->setDown( false );
        slotLock();
    }
}

void RSIWidget::keyPressEvent( QKeyEvent * e)
{
    kdDebug() << "Entering RSIWidget::mousePressEvent - Received: "
            << e->key() << " wanted " << m_accel->shortcut("minimize") << endl;

    if (e->key() == m_accel->shortcut("minimize"))
        m_timer->skipBreak();
}
//--------------------------- CONFIG ----------------------------//


void RSIWidget::readConfig()
{
    kdDebug() << "Entering RSIWidget::readConfig" << endl;
    KConfig* config = kapp->config();
    QColor *Black = new QColor(Qt::black);
    QFont *t = new QFont(  QApplication::font().family(), 40, 75, true );

    config->setGroup("General Settings");
    m_countDown->setPaletteForegroundColor(
            config->readColorEntry("CounterColor", Black ) );
    m_miniButton->setHidden(
            config->readBoolEntry("HideMinimizeButton", false));
    m_countDown->setHidden(
            config->readBoolEntry("HideCounter", false));
    m_accel->setEnabled("minimize",
                   !config->readBoolEntry("DisableAccel", false));
    QString shortcut = config->readEntry("MinimizeKey", "Escape");
    m_accel->setShortcut("minimize",KShortcut(shortcut));

    m_countDown->setFont(
            config->readFontEntry("CounterFont", t ) );

    bool uImages = config->readBoolEntry("ShowImages", false);

    bool recursive =
            config->readBoolEntry("SearchRecursiveCheck", false);
    QString path = config->readEntry("ImageFolder");

    if (m_basePath != path || m_searchRecursive != recursive ||
        m_useImages != uImages )
    {
        m_files.clear();
        m_files_done.clear();
        m_basePath = path;
        m_searchRecursive = recursive;
        m_useImages = uImages;
        if (m_useImages)
        {
            findImagesInFolder( path );
            // LoadImage (smoothscale) takes some time, so when booting this seriously
            // drains startup speed. So, deleay this a few seconds...
            QTimer::singleShot(2000, this, SLOT(slotNewSlide()));
        }
    }

    m_slideInterval = config->readNumEntry("SlideInterval", 2);
    m_showTimerReset = config->readBoolEntry("ShowTimerReset", false);

    delete Black;
    delete t;
}

#include "rsiwidget.moc"
