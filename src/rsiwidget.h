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

#ifndef RSIWidget_H
#define RSIWidget_H

#include <qdatetime.h>

class QTimer;
class QLabel;
class QPushButton;
class KAccel;
class KPassivePopup;
class QPixmap;

class RSIDock;
class RSITimer;

/**
 * @class RSIWidget
 * This widget is the main widget for RSIBreak.
 * It minimizes and maximized the widget
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIWidget : public QWidget
{
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param parent Parent Widget
         * @param name Name
         */
        RSIWidget( QWidget *parent = 0, const char *name = 0 );

        /**
         * Destructor
         */
        ~RSIWidget();

    private slots:
        void slotNewSlide();
        void slotLock();
        void slotMinimize();
        void minimize();
        void maximize();
        void setCounters();
        void updateIdleAvg( int );
        void updateRelaxMsg( int );
        void readConfig();

    protected:
        virtual void paintEvent( QPaintEvent* );
        virtual void closeEvent ( QCloseEvent * );

    private:
        void findImagesInFolder(const QString& folder);
        void loadImage();

        RSIDock*        m_tray;
        RSITimer*       m_timer;

        QPixmap*        m_backgroundimage;
        QString         m_basePath;
        QTimer*         m_timer_slide;
        QLabel*         m_countDown;
        KAccel*         m_accel;

        QLabel*         m_tool;

        bool            m_searchRecursive;
        int             m_slideInterval;

        QStringList     m_files;
        QStringList     m_files_done;
        QPushButton*    m_miniButton;
        QPushButton*    m_lockButton;

        KPassivePopup*  m_popup;
};

#endif
