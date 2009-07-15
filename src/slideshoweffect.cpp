/*
   Copyright (C) 2005-2006 Tom Albers <toma@kde.org>
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

#include "slideshoweffect.h"
#include "breakbase.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QTimer>

#include <KDebug>
#include <KWindowSystem>


SlideEffect::SlideEffect( QObject *parent )
        : BreakBase( parent ), m_searchRecursive( false )
{
    // Make all other screens gray...
    setGrayEffectOnAllScreens( true );
    excludeGrayEffectOnScreen( QApplication::desktop()->primaryScreen() );

    m_slidewidget = new SlideWidget( 0 );
    KWindowSystem::forceActiveWindow( m_slidewidget->winId() );
    KWindowSystem::setOnAllDesktops( m_slidewidget->winId(), true );
    KWindowSystem::setState( m_slidewidget->winId(), NET::KeepAbove );
    KWindowSystem::setState( m_slidewidget->winId(), NET::FullScreen );

    setReadOnly( true );

    m_timer_slide = new QTimer( this );
    connect( m_timer_slide, SIGNAL( timeout() ),  SLOT( slotNewSlide() ) );
}

SlideEffect::~SlideEffect()
{
    delete m_slidewidget;
}

bool SlideEffect::hasImages()
{
    return m_files.count() > 0;
}

void SlideEffect::activate()
{
    m_slidewidget->show();
    m_timer_slide->start( m_slideInterval*1000 );
    BreakBase::activate();
}

void SlideEffect::deactivate()
{
    m_timer_slide->stop();
    m_slidewidget->hide();
    BreakBase::deactivate();
}

void SlideEffect::loadImage()
{
    if ( m_files.count() == 0 )
        return;

    // Base the size on the size of the screen, for xinerama.
    QRect size = QApplication::desktop()->screenGeometry(
                     QApplication::desktop()->primaryScreen() );

    // Do not accept images whose surface is more than 3 times smaller than
    // screen
    int min_image_surface = size.width() * size.height() / 3;
    QImage image;

    while ( true ) {
        // reset if all images are shown
        if ( m_files_done.count() == m_files.count() )
            m_files_done.clear();

        // get a not yet used image
        int j;
        QString name;
        do {
            j = ( int )( m_files.count() * ( rand() / ( RAND_MAX + 1.0 ) ) );
            name = m_files[ j ];
        } while ( m_files_done.indexOf( name ) != -1 );

        // load image
        kDebug() << "Loading:" << name << "(" << j << "/"  << m_files.count() << ") ";
        image.load( name );

        // Check size
        if ( image.width() * image.height() >= min_image_surface ) {
            // Image is big enough, leave while loop
            m_files_done.append( name );
            break;
        } else {
            // Too small, remove from list
            m_files.removeAll( name );
            if ( m_files.count() == 0 ) {
                // Couldn't find any image big enough, leave function
                return;
            }
        }
    }

    QImage* m = new QImage( image.scaled( size.width(), size.height(),
                                          Qt::KeepAspectRatioByExpanding ) );

    if ( m->isNull() )
        return;

    m_slidewidget->setImage( m );
}

void SlideEffect::findImagesInFolder( const QString& folder )
{
    if ( folder.isNull() )
        return;

    QDir dir( folder );

    if ( !dir.exists() || !dir.isReadable() ) {
        kWarning() << "Folder does not exist or is not readable: "
        << folder << endl;
        return;
    }

    // TODO: make an automated filter, maybe with QImageIO.
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.tif" << "*.tiff" <<
    "*.gif" << "*.bmp" << "*.xpm" << "*.ppm" <<  "*.pnm"  << "*.xcf" <<
    "*.pcx";
    QStringList filtersUp;
    for ( int i = 0; i < filters.size(); ++i )
        filtersUp << filters.at( i ).toUpper();
    dir.setNameFilters( filters << filtersUp );
    dir.setFilter( QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::AllDirs );

    const QFileInfoList list = dir.entryInfoList();
    for ( int i = 0; i < list.count(); ++i ) {
        QFileInfo fi = list.at( i );
        if ( fi.isFile() )
            m_files.append( fi.filePath() );
        else if ( fi.isDir() && m_searchRecursive &&
                  fi.fileName() != "." &&  fi.fileName() != ".." )
            findImagesInFolder( fi.absoluteFilePath() );
    }
}

void SlideEffect::slotNewSlide()
{
    if ( m_files.count() == 1 )
        return;

    loadImage();
}

void SlideEffect::reset( const QString& path, bool recursive, int slideInterval )
{
    m_files.clear();
    m_files_done.clear();
    m_basePath = path;
    m_searchRecursive = recursive;
    m_slideInterval = slideInterval;

    findImagesInFolder( path );
    kDebug() << "Amount of Files:" << m_files.count();
    QTimer::singleShot( 2000, this, SLOT( slotNewSlide() ) );
}

// ------------------ Show widget


SlideWidget::SlideWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup )
{
    QRect rect = QApplication::desktop()->screenGeometry(
                     QApplication::desktop()->primaryScreen() );
    setGeometry( rect );
}

SlideWidget::~SlideWidget() {}

void SlideWidget::setImage( QImage* image )
{
    QPalette palette;
    palette.setBrush( backgroundRole(), QBrush( QPixmap::fromImage( *image ) ) );
    setPalette( palette );
}

#include "slideshoweffect.moc"
