/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

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
#include <qdatetime.h>
#include <qlineedit.h>
#include <qimage.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qdesktopwidget.h>

#include "config.h"

#include <kwin.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaccel.h>
#include <kdebug.h>
#include <kconfig.h>
#include <dcopclient.h>
#include <kmessagebox.h>

#include <stdlib.h>
#include <math.h>

#include "rsiwidget.h"
#include "rsitimer.h"
#include "rsidock.h"
#include "rsipopup.h"

RSIWidget::RSIWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    kdDebug() << "Entering RSIWidget::RSIWidget" << endl;

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

    m_backgroundimage = new QPixmap(QApplication::desktop()->width(),
                                    QApplication::desktop()->height());

    m_tray = new RSIDock(this,"Tray Item");
    m_tray->show();

    m_popup = new RSIPopup(m_tray);
    connect( m_popup, SIGNAL( lock() ), SLOT( slotLock() ) );

    m_timer = new RSITimer(this,"Timer");
    connect( m_timer, SIGNAL( breakNow() ), SLOT( maximize() ) );
    connect( m_timer, SIGNAL( setCounters( const QTime &, const int ) ), 
             SLOT( setCounters( const QTime &, const int ) ) );
    connect( m_timer, SIGNAL( updateIdleAvg( double ) ), SLOT( updateIdleAvg( double ) ) );
    connect( m_timer, SIGNAL( minimize() ), SLOT( minimize() ) );
    connect( m_timer, SIGNAL( relax( int ) ), m_popup, SLOT( relax( int ) ) );

    connect( m_tray, SIGNAL( quitSelected() ), kapp, SLOT( quit() ) );
    connect( m_tray, SIGNAL( configChanged() ), SLOT( readConfig() ) );
    connect( m_tray, SIGNAL( configChanged() ), m_timer, SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( configChanged() ), m_popup, SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( dialogEntered() ), m_timer, SLOT( slotStop() ) );
    connect( m_tray, SIGNAL( dialogLeft() ), m_timer, SLOT( slotRestart() ) );
    connect( m_tray, SIGNAL( breakRequest() ), m_timer, SLOT( slotMaximize() ) );
    connect( m_tray, SIGNAL( suspend() ), m_timer, SLOT( slotSuspend() ) );
    connect( m_tray, SIGNAL( suspend() ), m_popup, SLOT( hide() ) );
    connect( m_tray, SIGNAL( unsuspend() ), m_timer, SLOT( slotUnSuspend() ) );

    srand ( time(NULL) );

    QBoxLayout *topLayout = new QVBoxLayout( this, 5);

    m_countDown = new QLabel(this);
    m_countDown->setAlignment( AlignCenter );
    m_countDown->setBackgroundOrigin(QWidget::ParentOrigin);
    topLayout->addWidget(m_countDown);

    topLayout->addStretch(5);

    QBoxLayout *buttonRow = new QHBoxLayout( topLayout );

    m_miniButton = new QPushButton( i18n("Minimize"), this );
    buttonRow->addWidget( m_miniButton );
    connect( m_miniButton, SIGNAL( clicked() ), SLOT( slotMinimize() ) );

    m_lockButton = new QPushButton( i18n("Lock desktop"), this );
    buttonRow->addWidget( m_lockButton );
    connect( m_lockButton, SIGNAL( clicked() ), SLOT( slotLock() ) );

    m_accel = new KAccel(this);
    m_accel->insert("minimize", i18n("Minimize"),
                    i18n("Abort a break"),Qt::Key_Escape,
                    this, SLOT( slotMinimize() ));

    buttonRow->addStretch(10);

    m_timer_slide = new QTimer(this);
    connect(m_timer_slide, SIGNAL(timeout()),  SLOT(slotNewSlide()));

    readConfig();
}

RSIWidget::~RSIWidget()
{
    kdDebug() << "Entering RSIWidget::~RSIWidget" << endl;
    delete m_backgroundimage;
}

void RSIWidget::minimize()
{
    kdDebug() << "Entering RSIWidget::Minimize" << endl;
    m_timer_slide->stop();
    hide();
    loadImage();
}

void RSIWidget::maximize()
{
    kdDebug() << "Entering RSIWidget::Maximize" << endl;

    if (m_slideInterval>0)
        m_timer_slide->start( m_slideInterval*1000 );

    show(); // Keep it above the KWin calls.
    KWin::forceActiveWindow(winId());
    KWin::setOnAllDesktops(winId(),true);
    KWin::setState(winId(), NET::KeepAbove);
    KWin::setState(winId(), NET::FullScreen);
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

    // Base the size on the size of the screen, for xinerama.
    QRect size = QApplication::desktop()->screenGeometry(
                        QApplication::desktop()->primaryScreen() );

    QImage m = QImage( m_files[ j ]).smoothScale( size.width(),
                                                  size.height(),
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

void RSIWidget::slotNewSlide()
{
    kdDebug() << "Entering RSIWidget::slotNewSlide" << endl;

    loadImage();
    repaint( false );
}

void RSIWidget::slotLock()
{
    kdDebug() << "Entering RSIWidget::slotLock" << endl;

    QCString appname( "kdesktop" );
    int rsibreak_screen = qt_xscreen();
    if ( rsibreak_screen )
        appname.sprintf("kdesktop-screen-%d", rsibreak_screen );
    kapp->dcopClient()->send(appname, "KScreensaverIface", "lock()", "");
}

void RSIWidget::slotMinimize()
{
    kdDebug() << "slotMinize entered" << endl;
    m_timer->slotRestart();
}

void RSIWidget::setCounters( const QTime &time, const int currentBreak )
{
    int s = (int)ceil(QTime::currentTime().msecsTo( time )/1000);

    if (s > 0)
    {
        int minutes = (int)floor(s/60);
        int seconds  = s-(minutes*60);
        QString mString = i18n("one minute","%n minutes",minutes);
        QString sString = i18n("one second","%n seconds",seconds);
        QString finalString;
        QString cdString;
        
        if (minutes > 0 && seconds > 0)
        {
            finalString = i18n("First argument: minutes, second: seconds "
                               "both as you defined earlier",
                               "%1 and %2 remaining").arg(mString, sString);
            cdString = i18n("minutes:seconds","%1:%2")
                    .arg( QString::number( minutes ))
                    .arg( QString::number( seconds).rightJustify(2,'0'));
        }
        if ( minutes == 0 && seconds > 0 )
        {
            finalString = i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining").arg(sString);
            cdString = QString::number( seconds );
        }
        
        if ( minutes >0 && seconds == 0 )
        {
            finalString = i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining").arg(mString);
            cdString = i18n("minutes:seconds","%1:00")
                    .arg( QString::number( minutes ) );
        }
        
        int i=currentBreak-1;
        if (i == 0)
            finalString.append( "\n" + i18n("Next break is a big break") );
        else
            finalString.append( "\n" + i18n("Big break after next break",
                             "Big break after %n breaks", i) );
        
        kdDebug() << cdString << " " << finalString << endl;
        m_countDown->setText( cdString );
        QToolTip::add(m_tray, finalString );
        
    }
    else if ( m_timer->isSuspended() )
    {
        m_countDown->setText( i18n("Suspended") );
        QToolTip::add( m_tray, i18n("RSIBreak is currently suspended"));
    }
    else
    {
        m_countDown->setText (QString::null );
        QToolTip::add(m_tray, i18n("Waiting for the right moment to break"));
    }
}

void RSIWidget::updateIdleAvg( double idleAvg )
{
    // kdDebug() << "RSIWidget::updateIdleAvg() entered, idleAvg = " << idleAvg << endl;

    if ( idleAvg == 0.0 )
        m_tray->setIcon( 0 );
    else if ( idleAvg >0 && idleAvg<30 )
        m_tray->setIcon( 1 );
    else if ( idleAvg >=30 && idleAvg<60 )
        m_tray->setIcon( 2 );
    else if ( idleAvg >=60 && idleAvg<90 )
        m_tray->setIcon( 3 );
    else
        m_tray->setIcon( 4 );
}

// ----------------------------- EVENTS -----------------------//

void RSIWidget::paintEvent( QPaintEvent * )
{
    kdDebug() << "Entering RSIWidget::paintEvent" << endl;
    bitBlt( this, 0, 0, m_backgroundimage );
    m_countDown->setPaletteBackgroundPixmap( *m_backgroundimage );
}

void RSIWidget::closeEvent( QCloseEvent * )
{
    hide();
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

    m_slideInterval = config->readNumEntry("SlideInterval", 2);

    delete Black;
    delete t;
}

#include "rsiwidget.moc"
