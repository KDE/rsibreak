/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

   The parts for idle detection is based on
   kdepim's karm idletimedetector.cpp/.h

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
#include <qtooltip.h>
#include <qevent.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qfileinfo.h>

#include "config.h"     // HAVE_LIBXSS
#ifdef HAVE_LIBXSS      // Idle detection.
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 #include <X11/extensions/scrnsaver.h>
 #include <fixx11h.h>
#endif // HAVE_LIBXSS

#include <kwin.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaccel.h>
#include <kmessagebox.h>
#include <kpassivepopup.h>
#include <kdebug.h>
#include <kconfig.h>
#include <dcopclient.h>

#include <stdlib.h>
#include <math.h>

#include "rsiwidget.h"
#include "rsidock.h"

RSIWidget::RSIWidget( QWidget *parent, const char *name )
    : QWidget( parent, name ), m_idleLong( false ), m_targetReached( false ), m_needBreak( false ), m_idleIndex( 0 )
{
    kdDebug() << "Entering RSIWidget" << endl;
    m_tray = new RSIDock(this,"Tray Item");
    m_tray->show();
    connect( m_tray, SIGNAL( quitSelected() ), kapp, SLOT( quit() ) );
    connect( m_tray, SIGNAL( configChanged() ), SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( dialogEntered() ), SLOT( slotStop() ) );
    connect( m_tray, SIGNAL( dialogLeft() ), SLOT( slotStart() ) );
    connect( m_tray, SIGNAL( breakRequest() ), SLOT( slotMaximize() ) );


    m_backgroundimage = new QPixmap(QApplication::desktop()->width(),
                                    QApplication::desktop()->height());


#ifdef HAVE_LIBXSS      // Idle detection.
    int event_base, error_base;
    if(XScreenSaverQueryExtension(qt_xdisplay(), &event_base, &error_base))
        m_idleDetection = true;
#endif

    if (m_idleDetection)
        kdDebug() << "IDLE Detection is possible" << endl;
    else
        kdDebug() << "IDLE Detection is not possible" << endl;


    srand ( time(NULL) );

    KMessageBox::information(parent,
                             i18n("Welcome to RSIBreak\n\n"
                                  "In your tray you can now see a clock. "
                                  "When you right-click on that you will see "
                                  "a menu, from which you can go to the "
                                  "configuration for example.\nWhen you want to "
                                  "know when the next break is, hover "
                                  "over the icon.\n\nUse RSIBreak wisely."),
                             i18n("Welcome"),
                             "dont_show_welcome_again_for_001");
    QBoxLayout *topLayout = new QVBoxLayout( this, 5);

    m_countDown = new QLabel(this);
    m_countDown->setAlignment( AlignCenter );
    m_countDown->setBackgroundOrigin(QWidget::ParentOrigin);
    topLayout->addWidget(m_countDown);

    topLayout->addStretch(5);

    QBoxLayout *buttonRow = new QHBoxLayout( topLayout );

    m_miniButton = new QPushButton( i18n("Minimize"), this );
    buttonRow->addWidget( m_miniButton );
    connect( m_miniButton, SIGNAL( clicked() ), SLOT( slotStart() ) );

    m_accel = new KAccel(this);
    m_accel->insert("minimize", i18n("Minimize"),
                    i18n("Abort a break"),Qt::Key_Escape,
                    this, SLOT( slotStart() ));

    buttonRow->addStretch(10);

    m_timer_max = new QTimer(this);
    connect(m_timer_max, SIGNAL(timeout()), SLOT(slotMaximize()));

    m_timer_min = new QTimer(this);
    connect(m_timer_min, SIGNAL(timeout()),  SLOT(slotMinimize()));

    m_timer_slide = new QTimer(this);
    connect(m_timer_slide, SIGNAL(timeout()),  SLOT(slotNewSlide()));

    m_normalTimer = startTimer( 1000 );

    m_popup = new KPassivePopup(m_tray);
    m_tool = new QLabel(m_popup);
    m_popup->setView(m_tool);

    readConfig();
    startMinimizeTimer();
}

RSIWidget::~RSIWidget()
{
    kdDebug() << "Entering ~RSIWidget" << endl;
    delete m_timer_max;
    delete m_timer_min;
    delete m_timer_slide;
    delete m_tray;
    delete m_backgroundimage;
}

void RSIWidget::startMinimizeTimer()
{
    kdDebug() << "Entering startMinimizeTimer" << endl;

    hide();
    m_timer_min->stop();
    m_targetTime = QTime::currentTime().addSecs(m_timeMinimized);
    m_timer_max->start(m_timeMinimized*1000, true);
}

void RSIWidget::loadImage()
{
    if (m_files.count() == 0)
        return;

    // reset if all images are shown
    if (m_files_done.count() == m_files.count())
        m_files_done.clear();

    // get a net yet used number
    int j = (int) ((m_files.count()) * (rand() / (RAND_MAX + 1.0)));
    while (m_files_done.findIndex( QString::number(j) ) != -1)
        j = (int) ((m_files.count()) * (rand() / (RAND_MAX + 1.0)));

    // Use that number to load the right image
    m_files_done.append(QString::number(j));

    kdDebug() << "Loading: " << m_files[j] <<
                    "( " << j << " / "  << m_files.count() << " ) " << endl;

    QImage m = QImage( m_files[ j ]).smoothScale(
                        QApplication::desktop()->width(),
                        QApplication::desktop()->height(),
                        QImage::ScaleMax);


    if (m.isNull())
    {
        kdWarning() << "constructed a null-image" << endl;
        kdDebug() << "format: " <<
                QImageIO::imageFormat(m_files[j]) << endl;

        QImageIO iio;
        iio.setFileName(m_files[j]);
        if ( iio.read() )
            kdDebug() << "Read is ok" << endl;
        else
            kdDebug() << "Read failed" << endl;

        kdDebug() << iio.status() << endl;
        return;
    }

    if (m_backgroundimage->convertFromImage(m))
        kdDebug() << "New background image set" << endl;
    else
        kdWarning() << "Failed to set new background image" << endl;
}

int RSIWidget::idleTime()
{
    int totalIdle = 0;

#ifdef HAVE_LIBXSS      // Idle detection.
    XScreenSaverInfo*  _mit_info;
    _mit_info= XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(qt_xdisplay(), qt_xrootwin(), _mit_info);
    totalIdle = (_mit_info->idle/1000);
#endif // HAVE_LIBXSS

    return totalIdle;
}

void RSIWidget::breakNow( int t )
// this shows a break request all over the screen.
{
    kdDebug() << "Entering breakNow for " << t << "seconds " << endl;
    m_targetTime = QTime::currentTime().addSecs(t);
    m_timer_min->start(t*1000, true);

    if (m_slideInterval>0)
        m_timer_slide->start( m_slideInterval*1000 );

    setCounters();

    show(); // Keep it above the KWin calls.
    KWin::forceActiveWindow(winId());
    KWin::setOnAllDesktops(winId(),true);
    KWin::setState(winId(), NET::KeepAbove);
    KWin::setState(winId(), NET::FullScreen);
}

void RSIWidget::setCounters()
{
    int s = (int)ceil(QTime::currentTime().msecsTo(m_targetTime)/1000);

    if (s > 0)
        m_countDown->setText( QString::number( s ) );
    else
        m_countDown->setText( QString::null );

    // TODO: tell something about tinyBreaks, bigBreaks.
    if (s > 0)
        QToolTip::add(m_tray, i18n("One second remaining",
                      "%n seconds remaining",s));
    else
        QToolTip::add(m_tray, i18n("Waiting for the right moment to break"));
}


void RSIWidget::findImagesInFolder(const QString& folder)
{
    kdDebug() << "Looking for pictures in " << folder << endl;

    if ( folder.isNull() )
        return;

    QDir dir( folder);

    // TODO: make an automated filter, maybe with QImageIO.
    QString ext("*.png *.jpg *.jpeg *.tif *.tiff *.gif *.bmp *.xpm *.ppm *.pnm *.xcf *.pcx");
    dir.setNameFilter(ext + " " + ext.upper());
    dir.setMatchAllDirs ( true );

    if ( !dir.exists() or !dir.isReadable() )
    {
        kdWarning() << "Folder does not exist or is not readable: "
                << folder << endl;
        return;
    }

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


// -------------------------- SLOTS ------------------------//


void RSIWidget::slotMaximize()
// Do not be mislead by the name "slotMaximize", the user cannot maximize the window. This is
// exclusively triggered by the timer whose signal is connected.
{
    kdDebug() << "Entering slotMaximize" << endl;

    if (m_currentInterval > 0)
        m_currentInterval--;

    m_needBreak = 0;
    m_timer_max->stop();

    int totalIdle = idleTime();
    int minNeeded;
    if ( m_currentInterval == 0 )
        minNeeded = m_bigTimeMaximized;
    else
        minNeeded = m_timeMaximized;

    kdDebug() << "BigBreak in " << m_currentInterval << "; "
            << "Idle " << totalIdle << "s; "
            << "Needed " << minNeeded << "s; "
            << "IdleLong: " << m_idleLong << "; "
            << endl;

    // If user has been idle since the last break, it will
    // get a bonus in the way that it gains a tinyBreak
    if (totalIdle >= m_timeMinimized)
    {
        m_idleLong=true;
        kdDebug() << "Next break will be delayed, "
                     "you have been idle a while now" << endl;

        m_currentInterval++; 		// give back the this one
        if (m_currentInterval < m_bigInterval)
            m_currentInterval++;	// give a bonus

        startMinimizeTimer();
        return;
    }

    // if user has been idle for at least two breaks, there is no
    // need to break immediatly, we can postpone the break
    if ( m_idleLong )
    {
        kdDebug() << "Break delayed, you have been idle for a while recently" << endl;
        m_currentInterval++;
        startMinimizeTimer();
        m_idleLong=false;
        return;
    }

    kdDebug() << "You need a break, monitoring keyboard for the right moment..." << endl;
    m_needBreak=minNeeded;
    if ( m_currentInterval == 0 )
        m_currentInterval=m_bigInterval;
}

void RSIWidget::slotMinimize()
{
    kdDebug() << "Entering slotMinimize" << endl;

    startMinimizeTimer();
    loadImage();
}

void RSIWidget::slotNewSlide()
{
    kdDebug() << "Entering slotNewSlide" << endl;

    if (m_timer_min->isActive())
    {
        loadImage();
        repaint( false );
    }
    else
        m_timer_slide->stop();
}

void RSIWidget::slotStop( )
{
    kdDebug() << "Entering slotStop" << endl;
    startMinimizeTimer();
    m_timer_max->stop();
    m_needBreak=false;
}

void RSIWidget::slotStart( )
{
    kdDebug() << "Entering slotStart" << endl;
    // the interuption can not be considered a real break
    // only needed fot a big break of course
    if (m_currentInterval == m_bigInterval)
        m_currentInterval=0;

    slotMinimize();
}


// ----------------------------- EVENTS -----------------------//


void RSIWidget::timerEvent( QTimerEvent* )
{
    setCounters();

    int t = idleTime();
    if (t == 0)
        m_idleIndex++;
    else
        m_idleIndex--;
    m_idleIndexAmount++;

    int idleAvg = m_idleIndexAmount == 0 ? 0 : (int)(m_idleIndex*100 / m_idleIndexAmount);

    if ( idleAvg < 0 )
        m_tray->setIcon( 0 );
    if ( idleAvg >=20 && idleAvg<40 )
        m_tray->setIcon( 1 );
    if ( idleAvg >=40 && idleAvg<50 )
        m_tray->setIcon( 2 );
    if ( idleAvg >=50 && idleAvg<60 )
        m_tray->setIcon( 3 );
    if ( idleAvg >=60 )
        m_tray->setIcon( 4 );

    // If we are waiting for the right time to have a break, check the idle timeout
    // and activate the break if needed...
    if ( m_needBreak > 0 )
    {
        if (!m_idleDetection)
        {
            breakNow( m_needBreak );
            m_needBreak=0;
            return;
        }

        m_tool->setText(i18n("Please relax for 1 second",
                             "Please relax for %n seconds",
                             m_needBreak-t));
        m_popup->show();

        // if user is idle for more then 5 seconds, remember that.
        if (t >= 5 || QTime::currentTime().secsTo(m_targetTime) <= -30)
            m_targetReached=true;

        kdDebug() << "Idle for: " << t << "s; "
                << "BreakFor "<< m_needBreak << "s; "
                << "TargetReached " << m_targetReached << "; "
                << "Waiting for " << QTime::currentTime().secsTo(m_targetTime) << "s"
                << endl;

        // User has been idle for 5 seconds, and wants to start working again -> break Now!
        if (m_targetReached && t < 5)
        {
            kdDebug() << "Activity detected, break!" << endl;
            breakNow( m_needBreak );
            m_targetReached=false;
            m_popup->hide();
            m_needBreak=0;
        }

        // User has been idle for the time of the break now, this break is no longer needed!
        if (m_targetReached && t >= m_needBreak)
        {
            kdDebug() << "You have been idle for the duration of the break, thanks!" << endl;
            m_targetReached=false;
            m_needBreak=0;
            m_popup->hide();
            startMinimizeTimer();
        }
    }
}

void RSIWidget::paintEvent( QPaintEvent * )
{
    kdDebug() << "Entering paintEvent" << endl;
    bitBlt( this, 0, 0, m_backgroundimage );
    m_countDown->setPaletteBackgroundPixmap( *m_backgroundimage );
}


//--------------------------- CONFIG ----------------------------//


void RSIWidget::readConfig()
{
    kdDebug() << "Entering readConfig" << endl;
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

    bool recursive =
            config->readBoolEntry("SearchRecursiveCheck", false);
    QString path = config->readEntry("ImageFolder");

    if (m_basePath != path || m_searchRecursive != recursive)
    {
        m_files.clear();
        m_files_done.clear();
        m_basePath = path;
        m_searchRecursive = recursive;
        findImagesInFolder( path );
        loadImage();
    }

    m_timeMinimized = config->readNumEntry("TinyInterval", 10)*60;
    m_timeMaximized = config->readNumEntry("TinyDuration", 20);

    m_bigInterval = config->readNumEntry("BigInterval", 3);
    m_bigTimeMaximized = config->readNumEntry("BigDuration", 1)*60;
    m_currentInterval = m_bigInterval;

    m_slideInterval = config->readNumEntry("SlideInterval", 2);

    if (config->readBoolEntry("DEBUG"))
    {
        kdDebug() << "Debug mode activated" << endl;
        m_timeMinimized = m_timeMinimized/60;
        m_bigTimeMaximized = m_bigTimeMaximized/60;
    }

    delete Black;
}

void RSIWidget::slotReadConfig()
{
    kdDebug() << "Config changed" << endl;
    readConfig();
    startMinimizeTimer();
}

void RSIWidget::slotLock()
{
    kdDebug() << "slotLock entered" << endl;

    QCString appname( "kdesktop" );
    int rsibreak_screen = qt_xscreen();
    if ( rsibreak_screen )
        appname.sprintf("kdesktop-screen-%d", rsibreak_screen );
    kapp->dcopClient()->send(appname, "KScreensaverIface", "lock()", "");
}

#include "rsiwidget.moc"
