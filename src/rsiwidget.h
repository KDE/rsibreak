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

/**
 * @class RSILabel
 * This label supports a transparent background, used for the
 * RSIWidget when it's maximized.
 * @author Bram Schoenmakers <bramschoenmakers@kde.nl>
 */
class RSILabel : public QLabel
{
  Q_OBJECT

  public:
    /** Constructor */
    explicit RSILabel( QWidget * = 0);

    /** Destructor */
    ~RSILabel();

  public slots:
    /** Sets the text of the label and updates the mask. */
    virtual void setText( const QString & );

  protected:
    /** Update mask according to the text. Called by setText(). */
    virtual void updateMask();
};

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
        explicit RSIWidget( QWidget *parent = 0);

        /**
         * Destructor
         */
        ~RSIWidget();

        RSITimer* getTimer() const;

    private slots:
        void slotWelcome();
        void slotShowWhereIAm();
        void slotNewSlide();
        void slotLock();
        void minimize( bool newImage = true );
        void maximize();
        void setCounters( int );
        void updateIdleAvg( double );
        void readConfig();
        void slotGrab();
        void tinyBreakSkipped();
        void bigBreakSkipped();
        void breakSkipped();
        void skipBreakEnded();

    protected:
        virtual void closeEvent ( QCloseEvent * );
        virtual void mousePressEvent( QMouseEvent * e );
        virtual void mouseReleaseEvent( QMouseEvent * e );
        virtual void keyPressEvent( QKeyEvent * e);
        virtual void hideEvent( QHideEvent * e);

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

        QPixmap         m_backgroundimage;
        int             m_currentY;
        QString         m_basePath;
        QTimer*         m_timer_slide;
        QTimer*         m_grab;
        RSILabel*         m_countDown;
//        KAccel*         m_accel;

        QLabel*         m_tool;

        bool            m_useImages;
        bool            m_searchRecursive;
        bool            m_showTimerReset;
        int             m_slideInterval;

        QStringList     m_files;
        QStringList     m_files_done;
        QPushButton*    m_miniButton;
        QPushButton*    m_lockButton;

        RSIRelaxPopup*  m_relaxpopup;
        RSIToolTip*     m_tooltip;

        bool            m_useIdleDetection;
};

#   endif
