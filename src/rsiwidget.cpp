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
#include <qevent.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qfileinfo.h>

#include <kwin.h>
#include <klocale.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>

#include <stdlib.h>

#include "rsiwidget.h"
#include "rsidock.h"

RSIWidget::RSIWidget( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    m_tray = new RSIDock(this,"Tray Item");
    m_tray->show();
    connect( m_tray, SIGNAL( quitSelected() ), kapp, SLOT( quit() ) );
    connect( m_tray, SIGNAL( configChanged() ), SLOT( slotReadConfig() ) );
    connect( m_tray, SIGNAL( dialogEntered() ), SLOT( slotStop() ) );
    connect( m_tray, SIGNAL( dialogLeft() ), SLOT( slotStart() ) );

    srand ( time(NULL) );

    QBoxLayout *topLayout = new QVBoxLayout( this, 5);
    setEraseColor(QColor("black"));

    m_countDown = new QLabel(this);
    m_countDown->setAlignment( AlignCenter );
    m_countDown->setBackgroundOrigin(QWidget::WindowOrigin);
    topLayout->addWidget(m_countDown);

    topLayout->addStretch(5);

    QBoxLayout *buttonRow = new QHBoxLayout( topLayout );

    m_miniButton = new QPushButton( i18n("Minimize"), this );
    buttonRow->addWidget( m_miniButton );
    connect( m_miniButton, SIGNAL( clicked() ), SLOT( slotStart() ) );

    buttonRow->addStretch(10);

    QTime m_targetTime;
    
    m_timer_max = new QTimer(this);
    connect(m_timer_max, SIGNAL(timeout()), SLOT(slotMaximize()));

    m_timer_min = new QTimer(this);
    connect(m_timer_min, SIGNAL(timeout()),  SLOT(slotMinimize()));

    m_timer_max->start(m_timeMinimized, true);
    m_normalTimer = startTimer( 500 );

    readConfig();
    slotMinimize();
}

RSIWidget::~RSIWidget()
{
    delete m_timer_max;
    delete m_timer_min;
    delete m_tray;
}

void RSIWidget::slotMinimize()
{

    int maxWidth, maxHeight;
    maxWidth = QApplication::desktop()->width();
    maxHeight = QApplication::desktop()->height();

    // reset background
    repaint(0,0,maxWidth,maxHeight);
    hide();

    m_timer_min->stop();
    m_targetTime = QTime::currentTime().addSecs(m_timeMinimized);
    m_timer_max->start(m_timeMinimized*1000, true);

    if (m_files.count() == 0)
        return;

    // reset if all images are shown
    if (m_files_done.count() == m_files.count())
        m_files_done.clear();

    // get a net yet used number
    int j = (int) ( (m_files.count()) * (rand() / (RAND_MAX + 1.0)));
    while (m_files_done.findIndex( QString::number(j) ) != -1)
        j = (int) ((m_files.count()) * (rand() / (RAND_MAX + 1.0)));

    // Use that number to load the right image
    m_files_done.append(QString::number(j));
    //     kdDebug() << "Loading " << j
    //             << " Total " << m_files.count()
    //             << " Already shown " << m_files_done.count() << endl;

    QPixmap m = QPixmap(maxWidth, maxHeight);
    m.load(m_files[ j ]);
    setPaletteBackgroundPixmap( m );

/*
    TODO: officially we should not load it this way, but 
          do some resizing of the image to load. I can not
          see a way to do this right now.
          Use doubles below ;-)

    if (maxWidth/m.width() < maxHeight/m.height())
     {
        newWidth = (int)maxWidth;
        newHeight = (int)((maxWidth/m.width())*m.height());
     } else {
        newHeight = (int)maxHeight;
        newWidth = (int)((maxHeight/m.height())*m.width());
    }

    m.resize(newWidth, newHeight); */


//     QLabel* temp = new QLabel(this);
//     temp->setScaledContents(true);
//     temp->resize(QSize(newWidth,newHeight));
//     temp->move(0,m_countDown->height());
//     kdDebug() << "img " << m.width() << "x" << m.height() << endl;
//     kdDebug() << "max " << maxWidth << "x" << maxHeight << endl;
//     kdDebug() << "new " << newWidth << "x" << newHeight << endl;
//     temp->setPixmap(m);
//     temp->show();
}

void RSIWidget::slotMaximize()
{
    m_currentInterval--;
    m_timer_max->stop();

    if (m_currentInterval > 0)
    {
        kdDebug() << "TinyBreak, next BigBreak in " << m_currentInterval << endl;
        m_targetTime = QTime::currentTime().addSecs(m_timeMaximized);
        m_timer_min->start(m_timeMaximized*1000, true);
    }
    else
    {
        kdDebug() << "BigBreak" << endl;
        m_targetTime = QTime::currentTime().addSecs(m_bigTimeMaximized);
        m_timer_min->start(m_bigTimeMaximized*1000, true);
        m_currentInterval=m_bigInterval;
    }

    KWin::forceActiveWindow(winId());
    KWin::setOnAllDesktops(winId(),true);
    KWin::setOnAllDesktops(winId(),false);
    KWin::setState(winId(), NET::KeepAbove);
    KWin::setState(winId(), NET::FullScreen);
    show();
}

void RSIWidget::timerEvent( QTimerEvent* )
{
    // TODO: tell something about tinyBreaks, bigBreaks.
    int s = QTime::currentTime().secsTo(m_targetTime) +1 ;
    m_countDown->setText( QString::number( s ) );
    QToolTip::add(m_tray, i18n("One second remaining",
                  "%n seconds remaining",s));
}

void RSIWidget::slotStop( )
{
    kdDebug() << "Timer stopped" << endl;
    slotMinimize();
    m_timer_max->stop();
}

void RSIWidget::slotStart( )
{
    kdDebug() << "Timer started" << endl;
    // the interuption can not be considered a real break
    // only needed fot a big break of course
    if (m_currentInterval == m_bigInterval)
        m_currentInterval=0;

    slotMinimize();
}

void RSIWidget::findImagesInFolder(const QString& folder)
{
    kdDebug() << "Looking for pictures in " << folder << endl;
    QDir dir( folder);

    // TODO: make an automated filter, maybe with QImageIO.
    QString ext("*.png *.jpg *.jpeg *.tif *.tiff *.gif *.bmp *.xpm *.ppm *.pnm *.xcf *.pcx");
    dir.setNameFilter(ext + ext.upper());
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

void RSIWidget::findBackgroundImages()
{
    QDir dir(QDir::home());

    QStringList fileList(dir.entryList(QDir::Dirs));
    for (QStringList::iterator it = fileList.begin(); it != fileList.end(); ++it)
    {
        if ((*it) == "." || (*it) == "..")
            continue;
        findImagesInFolder( dir.absPath() + "/" + (*it) );
    }
}

void RSIWidget::readConfig()
{
    KConfig* config = kapp->config();
    QColor *Black = new QColor(Qt::black);
    QFont *t = new QFont( "Verdana", 40, 75, true );

    // Make something fake, don't load anyyhing if not configured
    QString d = QDir::home().path()+"noimagesfolderconfigured";
    
    config->setGroup("General Settings");
    m_countDown->setPaletteForegroundColor(
            config->readColorEntry("CounterColor", Black ) );
    m_miniButton->setHidden(
            config->readBoolEntry("HideMinimizeButton", false));
    m_countDown->setHidden(
            config->readBoolEntry("HideCounter", false));
    m_countDown->setFont(
            config->readFontEntry("CounterFont", t ) );

    bool recursive =
            config->readBoolEntry("SearchRecursiveCheck", false);
    QString path = config->readEntry("ImageFolder", d );
    if (m_basePath != path || m_searchRecursive != recursive)
    {
        m_files.clear();
        m_files_done.clear();
        m_basePath = path;
        m_searchRecursive = recursive;
        findImagesInFolder( path );
    }

    m_timeMinimized = (int)(QString(config->readEntry("TinyInterval", "10")).toFloat()*60);
    m_timeMaximized = QString(config->readEntry("TinyDuration", "20")).toInt();

    m_bigInterval = QString(config->readEntry("BigInterval", "3")).toInt();
    m_bigTimeMaximized = (int)(QString(config->readEntry("BigDuration", "1")).toFloat()*60);
    m_currentInterval = m_bigInterval;

    delete Black;
}

void RSIWidget::slotReadConfig()
{
    kdDebug() << "Config changed" << endl;
    readConfig();
    slotMinimize();
}

#include "rsiwidget.moc"
