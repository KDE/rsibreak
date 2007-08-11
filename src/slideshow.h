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

#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <QWidget>

class BoxDialog;

/**
 * @class SlideShow
 * This widget is the main widget for RSIBreak.
 * It minimizes and maximized the widget
 * @author Tom Albers <tomalbers@kde.nl>
 */
class SlideShow : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit SlideShow( QWidget *parent = 0 );

    /**
     * Destructor
     */
    ~SlideShow();

    void reset( const QString& path, bool recursive, int interval );
    void start();
    void stop();
    bool hasImages();
    void loadImage();

    BoxDialog* dialog() {
        return m_dialog;
    };

private slots:
    void slotNewSlide();

private:
    void findImagesInFolder( const QString& folder );

    BoxDialog*      m_dialog;
    QPixmap         m_backgroundimage;
    QString         m_basePath;
    QTimer*         m_timer_slide;

    bool            m_searchRecursive;
    int             m_slideInterval;

    QStringList     m_files;
    QStringList     m_files_done;
};

#   endif
