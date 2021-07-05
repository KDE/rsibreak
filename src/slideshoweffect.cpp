/*
    SPDX-FileCopyrightText: 2005-2006, 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "slideshoweffect.h"
#include "breakbase.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QRandomGenerator>

#include <KWindowSystem>


SlideEffect::SlideEffect( QObject *parent )
        : BreakBase( parent ), m_searchRecursive( false ), m_showSmallImages( false )
{
    // Make all other screens gray...
    slotGray();
    connect( qApp, &QGuiApplication::screenAdded, this, &SlideEffect::slotGray );
    connect( qApp, &QGuiApplication::screenRemoved, this, &SlideEffect::slotGray );

    m_slidewidget = new SlideWidget( 0 );
    KWindowSystem::forceActiveWindow( m_slidewidget->winId() );
    KWindowSystem::setOnAllDesktops( m_slidewidget->winId(), true );
    KWindowSystem::setState( m_slidewidget->winId(), NET::KeepAbove );
    KWindowSystem::setState( m_slidewidget->winId(), NET::FullScreen );

    setReadOnly( true );

    m_timer_slide = new QTimer( this );
    connect(m_timer_slide, &QTimer::timeout, this, &SlideEffect::slotNewSlide);
}

SlideEffect::~SlideEffect()
{
    delete m_slidewidget;
}

void SlideEffect::slotGray()
{
    // Make all other screens gray...
    setGrayEffectOnAllScreens( true );
    excludeGrayEffectOnScreen( QGuiApplication::primaryScreen() );
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
    const QSize size = QGuiApplication::primaryScreen()->geometry().size();

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
            j = QRandomGenerator::global()->bounded(m_files.count());
            name = m_files[ j ];
        } while ( m_files_done.indexOf( name ) != -1 );

        // load image
        qDebug() << "Loading:" << name << "(" << j << "/"  << m_files.count() << ") ";
        image.load( name );

        // Check size
        if ( image.width() * image.height() >= min_image_surface || m_showSmallImages ) {
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

    const Qt::AspectRatioMode mode = ( m_expandImageToFullScreen ) ? Qt::KeepAspectRatioByExpanding
                                                             : Qt::KeepAspectRatio;
    const QImage m( image.scaled( size.width(), size.height(), mode ) );

    if ( m.isNull() )
        return;

    m_slidewidget->setImage( m );
}


void SlideEffect::findImagesInFolder( const QString& folder )
{
    if ( folder.isNull() )
        return;

    QDir dir( folder );

    if ( !dir.exists() || !dir.isReadable() ) {
        qWarning() << "Folder does not exist or is not readable: " << folder;
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

void SlideEffect::reset( const QString& path, bool recursive, bool showSmallImages, bool expandImageToFullScreen, int slideInterval )
{
    m_files.clear();
    m_files_done.clear();
    m_basePath = path;
    m_searchRecursive = recursive;
    m_showSmallImages = showSmallImages;
    m_slideInterval = slideInterval;
    m_expandImageToFullScreen = expandImageToFullScreen;

    findImagesInFolder( path );
    qDebug() << "Amount of Files:" << m_files.count();
    QTimer::singleShot( 2000, this, &SlideEffect::slotNewSlide );
}

// ------------------ Show widget


SlideWidget::SlideWidget( QWidget *parent )
        : QWidget( parent, Qt::Popup )
{
    slotDimension();
    connect( qApp, &QGuiApplication::screenAdded, this, &SlideWidget::slotDimension );
    connect( qApp, &QGuiApplication::screenRemoved, this, &SlideWidget::slotDimension );

    QVBoxLayout *boxLayout = new QVBoxLayout( this );
    boxLayout->setSpacing(0);
    boxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout( boxLayout );
    m_imageLabel = new QLabel();
    m_imageLabel->setMaximumSize( width(), height() );
    m_imageLabel->setAlignment( Qt::AlignCenter );
    layout()->addWidget(m_imageLabel);
}

SlideWidget::~SlideWidget() {}

void SlideWidget::slotDimension()
{
    const QRect rect = QGuiApplication::primaryScreen()->geometry();
    setGeometry( rect );
}

void SlideWidget::setImage( const QImage &image )
{
    m_imageLabel->setPixmap( QPixmap::fromImage(image) );
}
