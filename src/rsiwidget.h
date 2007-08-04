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

#ifndef RSIObject_H
#define RSIObject_H

#include <QLabel>
#include "rsitimer.h"

class RSIDock;
class RSIRelaxPopup;
class RSIToolTip;
class GrayWidget;
class SlideShow;

/**
 * @class RSIObject
 * This controls all RSIBreak components
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIObject : public QObject
{
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param parent Parent Widget
         * @param name Name
         */
        explicit RSIObject( QWidget *parent = 0);

        /**
         * Destructor
         */
        ~RSIObject();

        /**
         * Access to the timer
         */
        RSITimer* timer() { return m_timer; };

        void showWhereIAm();

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

class RSIWidget: public QLabel
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "org.rsibreak.rsiwidget")

  public:
    explicit RSIWidget();

  /* Available through D-Bus */
  public Q_SLOTS:
    void showWhereIAm() {m_rsiobject->showWhereIAm();};
    void doBigBreak() {m_rsiobject->timer()->slotRequestBigBreak();};
    void doTinyBreak() {m_rsiobject->timer()->slotRequestTinyBreak();};
    void resume() {m_rsiobject->timer()->slotSuspended( false ); };
    void suspend() {m_rsiobject->timer()->slotSuspended( true ); };
    int idleTime() {return m_rsiobject->timer()->idleTime(); };
    int tinyLeft() {return m_rsiobject->timer()->tinyLeft(); };
    int bigLeft() {return m_rsiobject->timer()->bigLeft(); };

  private:
    RSIObject*  m_rsiobject;
};

#   endif
