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

#ifndef RSIWidget_H
#define RSIWidget_H

#include <qdatetime.h>
#include <qlabel.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QHideEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCloseEvent>

class QTimer;
class QPushButton;

class RSIDock;
class RSITimer;
class RSIRelaxPopup;
class RSIToolTip;
class GrayWidget;
class SlideShow;

/**
 * @class RSIWidget
 * This widget is the main widget for RSIBreak.
 * It minimizes and maximized the widget
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIWidget : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.rsibreak.rsiwidget")

    public:
        /**
         * Constructor
         * @param parent Parent Widget
         * @param name Name
         */
        explicit RSIWidget( QObject *parent = 0);

        /**
         * Destructor
         */
        ~RSIWidget();

    /* Available through D-Bus */
    public Q_SLOTS:
        void slotShowWhereIAm();

    private slots:
        void slotWelcome();
        void slotLock();
        void minimize( bool newImage = true );
        void maximize();
        void setCounters( int );
        void updateIdleAvg( double );
        void readConfig();
        void tinyBreakSkipped();
        void bigBreakSkipped();
        void breakSkipped();
        void skipBreakEnded();

    protected:
        /** Sets appropriate icon in tooltip and docker. */
        void setIcon( int );

    private:
        QString takeScreenshotOfTrayIcon();
        void findImagesInFolder(const QString& folder);
        void loadImage();
        void startTimer( bool idle);

        RSIDock*        m_tray;
        RSITimer*       m_timer;
        GrayWidget*     m_grayWidget;
        SlideShow*      m_slideShow;

        QLabel*         m_tool;

        bool            m_useImages;
        bool            m_showTimerReset;

        RSIRelaxPopup*  m_relaxpopup;
        RSIToolTip*     m_tooltip;

        bool            m_useIdleDetection;
};

#   endif
