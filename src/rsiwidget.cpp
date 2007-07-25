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

#include <qpushbutton.h>
#include <qdesktopwidget.h>
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
#include <qbitmap.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <Q3CString>
#include <QPixmap>
#include <QHideEvent>
#include <QMouseEvent>
#include <Q3VBoxLayout>
#include <QCloseEvent>
#include <QPixmap>

#include <config.h>

#include <kwindowsystem.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kimageeffect.h>
#include <ksystemtrayicon.h>
#include <kglobal.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "rsitimer.h"
#include "rsidock.h"
#include "rsirelaxpopup.h"
#include "rsitooltip.h"
#include "rsiglobals.h"

RSIWidget::RSIWidget( QWidget *parent, const char *name )
    : QWidget( parent, name, Qt::WType_Popup)
    , m_currentY( 0 )
    , m_useImages( false )
    , m_searchRecursive( false )
{
    // Keep these 3 lines _above_ the messagebox, so the text actually is right.
    m_tray = new RSIDock(this,"Tray Item");
    m_tray->setIcon( KSystemTrayIcon::loadIcon( "rsibreak0" ) );
    m_tray->show();

    if (KMessageBox::shouldBeShownContinue("dont_show_welcome_again_for_001"))
    {
        takeScreenshotOfTrayIcon();
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

        // we don't need to show the blurp about the changes in 0.5 for new users.
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
                                      "you want a long break.\nPlease review "
                                      "your current settings.\n\n"),
                                 i18n("Welcome"),
                                 "dont_show_welcome_again_for_050");

    //TODO:
    //setBackgroundMode( QWidget::NoBackground );
    QRect rect = QApplication::desktop()->screenGeometry(
                        QApplication::desktop()->primaryScreen() );
    setGeometry( rect );

    m_tooltip = new RSIToolTip( this, m_tray  );
    connect( m_tray, SIGNAL( showToolTip() ), m_tooltip, SLOT( showToolTip() ) );

    m_relaxpopup = new RSIRelaxPopup(this, m_tray);
    m_relaxpopup->show();
    connect( m_relaxpopup, SIGNAL( lock() ), SLOT( slotLock() ) );

    connect( m_tray, SIGNAL( quitSelected() ), kapp, SLOT( quit() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), SLOT( readConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), RSIGlobals::instance(), SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( configChanged( bool ) ), m_relaxpopup, SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_tooltip, SLOT( setSuspended( bool ) ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_relaxpopup, SLOT( hide() ) );

    srand ( time(NULL) );

    Q3BoxLayout *topLayout = new Q3VBoxLayout( this, 5);

    m_countDown = new RSILabel(this);
    /* m_countDown->setAlignment( AlignRight ); */
    m_countDown->setBackgroundMode( Qt::NoBackground );
    m_countDown->setBackgroundOrigin(QWidget::ParentOrigin);
    m_countDown->hide();
    topLayout->addWidget(m_countDown);

    topLayout->addStretch(5);

    Q3BoxLayout *buttonRow = new Q3HBoxLayout( topLayout );
    m_miniButton = new QPushButton( i18n("Skip"), this );
    buttonRow->addWidget( m_miniButton );
    m_lockButton = new QPushButton( i18n("Lock desktop"), this );
    buttonRow->addWidget( m_lockButton );
    buttonRow->addStretch(10);

    // TODO PORT
    // m_accel = new KAccel(this);

    // This connects only get used when there the user locks and
    // unlocks before the break is over. The lock releases the mouse
    // and keyboard, and that way the event methods are not called.
    connect(m_lockButton, SIGNAL( clicked() ), SLOT( slotLock() ) );

    m_timer_slide = new QTimer(this);
    connect(m_timer_slide, SIGNAL(timeout()),  SLOT(slotNewSlide()));

    m_grab = new QTimer(this);
    connect(m_grab, SIGNAL(timeout()),  SLOT(slotGrab()));

    readConfig();

    setIcon( 0 );

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

void RSIWidget::slotShowWhereIAm()
{
    takeScreenshotOfTrayIcon();
    KMessageBox::information(this,
           i18n("<p>RSIBreak is already running<p><p>It is located here:")
           + "<p><center><img source=\"systray_shot\"></center></p><p>",
           i18n("Already Running"));
}

void RSIWidget::takeScreenshotOfTrayIcon()
{

  /* TODO: PORT
        // Process the events else the icon will not be there and the screenie will fail!
        kapp->processEvents();

        // ********************************************************************************
        // This block is copied from Konversation - KonversationMainWindow::queryClose()
        // The part about the border is copied from  KSystemTray::displayCloseMessage()
        //
        // Compute size and position of the pixmap to be grabbed:
        QPoint g = m_tray->mapToGlobal( m_tray-> pos() );
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

        // Grab the desktop and draw a circle around the icon:
        QPixmap shot = QPixmap::grabWindow( qt_xrootwin(),  x,  y,  w,  h );
        QPainter painter( &shot );
        const int MARGINS = 6;
        const int WIDTH   = 3;
        int ax = g.x() - x - MARGINS -1;
        int ay = g.y() - y - MARGINS -1;
        painter.setPen(  QPen( Qt::red,  WIDTH ) );
        painter.drawArc( ax,  ay,  tw + 2*MARGINS,  th + 2*MARGINS,  0,  16*360 );
        painter.end();

        // Then, we add a border around the image to make it more visible:
        QPixmap finalShot(w + 2, h + 2);
        finalShot.fill(KApplication::palette().active().foreground());
        painter.begin(&finalShot);
        painter.drawPixmap(1, 1, shot);
        painter.end();

        // Associate source to image and show the dialog:
        Q3MimeSourceFactory::defaultFactory()->setPixmap( "systray_shot", finalShot );

        // End copied block
        // ********************************************************************************
  */
}

void RSIWidget::minimize( bool newImage )
{
    // stop the m_grab, if the break is ESCaped during the
    // first second, slotGrab is called after the widget
    // is minimized again.
    m_grab->stop();

    m_timer_slide->stop();
    releaseKeyboard();
    releaseMouse();
    hide();
    if (newImage)
        loadImage();
}

void RSIWidget::maximize()
{
    /* TODO: setBackgroundMode( QWidget::NoBackground ); */
    show(); // Keep it above the KWindowSystem calls.
    KWindowSystem::forceActiveWindow(winId());
    KWindowSystem::setOnAllDesktops(winId(),true);
    KWindowSystem::setState(winId(), NET::KeepAbove);
    KWindowSystem::setState(winId(), NET::FullScreen);

    // prevent that users accidently press this button while
    // they were writing text when the break appears
    m_miniButton->clearFocus();
    m_lockButton->clearFocus();

    // Small delay for grabbing keyboard and mouse, because
    // it will not grab when widget not visible
    m_grab->start(1000, true);

    // If there are no images found, we gray the screen and wait....
    m_countDown->hide();
    if (m_files.count() == 0 || !m_useImages)
    {
        m_backgroundimage.resize(QApplication::desktop()->geometry().width(),
                             QApplication::desktop()->geometry().height());
        setGeometry( QApplication::desktop()->geometry() );
        m_currentY=0;
        QTimer::singleShot( 10, this, SLOT( slotGrayEffect() ) );
    }
    else
    {
        m_backgroundimage.resize(QApplication::desktop()->width(),
                             QApplication::desktop()->height());
        setGeometry( QApplication::desktop()->screenGeometry( this ) );
        if (m_slideInterval>0)
            m_timer_slide->start( m_slideInterval*1000 );

        bitBlt( this, 0, 0, &m_backgroundimage );
    }
}

void RSIWidget::loadImage()
{
    if (m_files.count() == 0)
        return;

    // Base the size on the size of the screen, for xinerama.
    QRect size = QApplication::desktop()->screenGeometry(
                        QApplication::desktop()->primaryScreen() );

    // Do not accept images whose surface is more than 3 times smaller than
    // screen
    int min_image_surface = size.width() * size.height() / 3;
    QImage image;

    while (true)
    {
        // reset if all images are shown
        if (m_files_done.count() == m_files.count())
            m_files_done.clear();

        // get a not yet used image
        int j;
        QString name;
        do
        {
            j = (int) (m_files.count() * (rand() / (RAND_MAX + 1.0)));
            name = m_files[ j ];
        } while (m_files_done.findIndex( name ) != -1);

        // load image
        kDebug() << "Loading: " << name <<
                        "( " << j << " / "  << m_files.count() << " ) " << endl;
        image.load( name );

        // Check size
        if ( image.width() * image.height() >= min_image_surface ) {
            // Image is big enough, leave while loop
            m_files_done.append( name );
            break;
        }
        else
        {
            // Too small, remove from list
            m_files.remove( name );
            if (m_files.count() == 0)
            {
                // Couldn't find any image big enough, leave function
                return;
            }
        }
    }

    QImage m = image.smoothScale( size.width(), size.height(),
                                  Qt::KeepAspectRatioByExpanding);

    if (m.isNull())
        return;

    if (!m_backgroundimage.convertFromImage(m))
        kWarning() << "Failed to set new background image" << endl;

}


void RSIWidget::findImagesInFolder(const QString& folder)
{
    if ( folder.isNull() )
        return;

    QDir dir( folder);

    if ( !dir.exists() || !dir.isReadable() )
    {
        kWarning() << "Folder does not exist or is not readable: "
                << folder << endl;
        return;
    }

    // TODO: make an automated filter, maybe with QImageIO.
    QString ext("*.png *.jpg *.jpeg *.tif *.tiff *.gif *.bmp *.xpm *.ppm *.pnm *.xcf *.pcx");
    dir.setNameFilter(ext + ' ' + ext.toUpper());
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks );
    dir.setMatchAllDirs ( true );

    const QFileInfoList list = dir.entryInfoList();
    for (int i=0; i<list.count(); ++i)
    {
        QFileInfo fi = list.at(i);
        if ( fi.isFile())
            m_files.append(fi.filePath());
        else if (fi.isDir() && m_searchRecursive &&
                 fi.fileName() != "." &&  fi.fileName() != "..")
            findImagesInFolder(fi.absoluteFilePath());
    }
}

// This slot is copied from KDE's logout screen.
// from various authors found in:
// branches/KDE/3.5/kdebase/ksmserver/shutdowndlg.cpp
void RSIWidget::slotGrayEffect()
{
  /* TODO PORT 
    if ( m_currentY >= height() ) {
        m_countDown->show();
        if ( backgroundMode() == QWidget::NoBackground ) {
            setBackgroundMode( QWidget::NoBackground );
            setBackgroundPixmap( m_backgroundimage );
        }
        return;
    }

    QPixmap pixmap;
    pixmap = QPixmap::grabWindow( qt_xrootwin(), 0, m_currentY, width(), 10 );
    QImage image = pixmap.convertToImage();
    KImageEffect::blend( Qt::black, image, 0.4 );
    KImageEffect::toGray( image, true );
    pixmap.convertFromImage( image );
    bitBlt( this, 0, m_currentY, &pixmap );
    bitBlt( &m_backgroundimage, 0, m_currentY, &pixmap );
    m_currentY += 10;
    QTimer::singleShot( 1, this, SLOT( slotGrayEffect() ) );
    */
}

// -------------------------- SLOTS ------------------------//

void RSIWidget::slotNewSlide()
{
    if (m_files.count() == 1 || !m_useImages)
        return;

    loadImage();
    bitBlt( this, 0, 0, &m_backgroundimage );
}

void RSIWidget::slotLock()
{
    m_timer_slide->stop();
    releaseKeyboard();
    releaseMouse();

    /* TODO PORT
    Q3CString appname( "kdesktop" );
    int rsibreak_screen = qt_xscreen();
    if ( rsibreak_screen )
        appname.sprintf("kdesktop-screen-%d", rsibreak_screen );
    kapp->dcopClient()->send(appname, "KScreensaverIface", "lock()", "");
    */
}

void RSIWidget::slotGrab()
{
    grabMouse( QCursor( Qt::ArrowCursor ) );
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
            cdString = i18nc("minutes:seconds","%1:%2")
                    .arg( QString::number( minutes ))
                    .arg( QString::number( seconds).rightJustified(2,'0'));
        }
        else if ( minutes == 0 && seconds > 0 )
        {
            cdString = QString::number( seconds );
        }
        else if ( minutes >0 && seconds == 0 )
        {
            cdString = i18nc("minutes:seconds","%1:00")
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
        m_countDown->clear();
    }

    if( m_useImages )
    {
      setBackgroundPixmap( m_backgroundimage );
      m_countDown->show();
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
    QString newIcon = "rsibreak" +
                      ( m_timer->isSuspended() ? QString("x") : QString::number(level) );

    if (newIcon != currentIcon)
    {
        QIcon dockIcon = KSystemTrayIcon::loadIcon( newIcon );
        m_tray->setIcon( dockIcon );

        QPixmap toolPixmap = KIconLoader::global()->loadIcon( newIcon, K3Icon::Desktop );
        currentIcon = newIcon;
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
    disconnect( m_timer, SIGNAL( updateToolTip( int, int ) ),
                m_tooltip, SLOT( setCounters( int, int ) ) );

    m_tooltip->setPixmap( KIconLoader::global()->loadIcon( "rsibreak0", K3Icon::Desktop ) );
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
    if (e->button() != Qt::LeftButton)
        return;

    if ( m_miniButton->geometry().contains(e->pos()))
        m_miniButton->setDown( true );

    else if (m_lockButton->geometry().contains(e->pos()))
        m_lockButton->setDown( true );
}

void RSIWidget::mouseReleaseEvent( QMouseEvent * e )
{
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
  /* PORT: 
    if (e->key() == m_accel->shortcut("minimize") && m_accel->isEnabled() )
        m_timer->skipBreak();
  */
}

void RSIWidget::hideEvent( QHideEvent * e)
{
    kDebug() << k_funcinfo << endl;
    m_backgroundimage = QPixmap();
    setBackgroundPixmap(m_backgroundimage);

    QWidget::hideEvent(e);
}

//--------------------------- CONFIG ----------------------------//

void RSIWidget::startTimer( bool idle)
{

    static bool first = true;

    if (!first)
    {
        kDebug() << "Current Timer: " << m_timer->className()
                << " wanted: " << (idle ? "RSITimer" : "RSITimerNoIdle") << endl;

        if (!idle && m_timer->isA("RSITimerNoIdle"))
            return;

        if (idle && m_timer->isA("RSITimer"))
            return;

        kDebug() << "Switching timers" << endl;

        m_timer->deleteLater();
//        m_accel->remove("minimize");
    }

    if (idle)
        m_timer = new RSITimer(this,"Timer");
    else
        m_timer = new RSITimerNoIdle(this,"Timer");

    connect( m_timer, SIGNAL( breakNow() ), SLOT( maximize() ) );
    connect( m_timer, SIGNAL( updateWidget( int ) ),
             SLOT( setCounters( int ) ) );
    connect( m_timer, SIGNAL( updateToolTip( int, int ) ),
             m_tooltip, SLOT( setCounters( int, int ) ) );
    connect( m_timer, SIGNAL( updateIdleAvg( double ) ), SLOT( updateIdleAvg( double ) ) );
    connect( m_timer, SIGNAL( minimize( bool ) ), SLOT( minimize( bool ) ) );
    connect( m_timer, SIGNAL( relax( int, bool ) ), m_relaxpopup, SLOT( relax( int, bool ) ) );
    connect( m_timer, SIGNAL( relax( int, bool ) ), m_tooltip, SLOT( hide() ) );
    connect( m_timer, SIGNAL( tinyBreakSkipped() ), SLOT( tinyBreakSkipped() ) );
    connect( m_timer, SIGNAL( bigBreakSkipped() ), SLOT( bigBreakSkipped() ) );
    connect( m_timer, SIGNAL( skipBreakEnded() ), SLOT( skipBreakEnded() ) );

    connect( m_tray, SIGNAL( configChanged( bool ) ), m_timer, SLOT( slotReadConfig( bool ) ) );
    connect( m_tray, SIGNAL( dialogEntered() ), m_timer, SLOT( slotStopNoImage( ) ) );
    connect( m_tray, SIGNAL( dialogLeft() ), m_timer, SLOT( slotStartNoImage() ) );
    connect( m_tray, SIGNAL( breakRequest() ), m_timer, SLOT( slotRequestBreak() ) );
    connect( m_tray, SIGNAL( debugRequest() ), m_timer, SLOT( slotRequestDebug() ) );
    connect( m_tray, SIGNAL( suspend( bool ) ), m_timer, SLOT( slotSuspended( bool ) ) );

    connect( m_relaxpopup, SIGNAL( skip() ), m_timer, SLOT( skipBreak() ) );

    // This connects only get used when there the user locks and
    // unlocks before the break is over. The lock releases the mouse
    // and keyboard, and that way the event methods are not called.
    connect(m_miniButton, SIGNAL( clicked() ), m_timer, SLOT( skipBreak() ) );

//    m_accel->insert("minimize", i18n("Skip"),
 //                   i18n("Abort a break"),Qt::Key_Escape,
 //                   m_timer, SLOT( skipBreak() ));

    first = false;
}

void RSIWidget::readConfig()
{
    QColor *Black = new QColor(Qt::black);
    QFont *t = new QFont(  QApplication::font().family(), 40, 75, true );

    KConfigGroup config = KGlobal::config()->group("General Settings");
/* todo
        m_countDown->setPaletteForegroundColor(
            config.readEntry("CounterColor", Black ) );
    */
    m_miniButton->setHidden(
            config.readEntry("HideMinimizeButton", false));
    m_relaxpopup->setSkipButtonHidden(
            config.readEntry("HideMinimizeButton", false));
    m_countDown->setHidden(
            config.readEntry("HideCounter", false));
    /* TODO:
    m_countDown->setFont(
            config.readEntry("CounterFont", t ) );
    */

    bool useImages = config.readEntry("ShowImages", false);

    bool recursive =
            config.readEntry("SearchRecursiveCheck", false);
    QString path = config.readEntry("ImageFolder");

    bool timertype = config.readEntry("UseNoIdleTimer", false);
    startTimer(!timertype);

    // Hook in the shortcut after the timer initialisation.
//    m_accel->setEnabled(!config.readEntry("DisableAccel", false));
    QString shortcut = config.readEntry("MinimizeKey", "Escape");
//    m_accel->setShortcut("minimize",KShortcut(shortcut));

    if (m_basePath != path || m_searchRecursive != recursive ||
        m_useImages != useImages )
    {
        m_files.clear();
        m_files_done.clear();
        m_basePath = path;
        m_searchRecursive = recursive;
        m_useImages = useImages;
        if (m_useImages)
        {
            findImagesInFolder( path );
            // LoadImage (smoothscale) takes some time, so when booting this seriously
            // drains startup speed. So, deleay this a few seconds...
            QTimer::singleShot(2000, this, SLOT(slotNewSlide()));
        }
    }

// TODO: see if this is still the case in KDE4
//  Ok. So I have no idea. Without setting the group _again_ we do not get a valid
//  result. Again, by re-setting it I get the value from the config file, and without it not.
    config = KGlobal::config()->group("General Settings");

    m_slideInterval = config.readEntry("SlideInterval", 10);
    m_showTimerReset = config.readEntry("ShowTimerReset", false);

    delete Black;
    delete t;
}





RSILabel::RSILabel( QWidget *parent, const char *name )
: QLabel( parent, name )
{
}

RSILabel::~RSILabel()
{
}

void RSILabel::setText( const QString &str )
{
    QLabel::setText( str );
    updateMask();
}

void RSILabel::updateMask()
{
  /* PORT
    QBitmap b( size() );
    b.fill( color0 );

    QPainter p;
    p.begin( &b, this );
    p.setPen( color1 );
    drawContents( &p );
    p.end();

    setMask( b );
  */
}

RSITimer *RSIWidget::getTimer() const
{
    return m_timer;
}

#include "rsiwidget.moc"
