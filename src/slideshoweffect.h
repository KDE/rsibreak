/*
   Copyright (C) 2005-2006 Tom Albers <toma.org>
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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QWidget>
#include "breakbase.h"

class SlideWidget;

class SlideEffect : public BreakBase
{
    Q_OBJECT

public:
    SlideEffect( QObject *parent );
    ~SlideEffect();
    void reset( const QString& path, bool recursive, int interval );
    void activate();
    void deactivate();
    bool hasImages();
    void loadImage();


private slots:
    void slotNewSlide();

private:
    void findImagesInFolder( const QString& folder );

    SlideWidget*    m_slidewidget;
    QString         m_basePath;
    QTimer*         m_timer_slide;

    bool            m_searchRecursive;
    int             m_slideInterval;

    QStringList     m_files;
    QStringList     m_files_done;
};

class SlideWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit SlideWidget( QWidget *parent = 0 );

    /**
     * Destructor
     */
    ~SlideWidget();

    void setImage( QImage* image );
};

#   endif
