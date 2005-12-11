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

#ifndef RSITimer_H
#define RSITimer_H

#include <qmap.h>
#include <qtimer.h>
#include <qdatetime.h>

/**
 * @class RSITimer
 * This class controls the timings and arranges the maximizing
 * and minimizing of the widget.
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSITimer : public QObject
{
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param parent Parent Widget
         * @param name Name
         */
        RSITimer( QObject *parent = 0, const char *name = 0 );
        ~RSITimer();

        bool isSuspended() const { return m_suspended; }

    public slots:
        void slotMinimize();
        void slotMaximize();
        void slotReadConfig();
        void slotStop();
        void slotSuspend();
        void slotUnSuspend();
        void slotRestart();

    protected:
        virtual void timerEvent( QTimerEvent* );

    signals:
        void breakNow();
        void setCounters( const QTime & );
        void updateIdleAvg( int );
        void minimize();
        void relax( int );

    private:
        void startMinimizeTimer();
        void readConfig();
        int idleTime();
        void breakNow( int t );

        QTime           m_targetTime;
        QTimer*         m_timer_max;
        QTimer*         m_timer_min;

        bool            m_idleLong;
        bool            m_idleDetection;
        bool            m_suspended;

        int             m_needBreak;

        int             m_currentInterval;
        QMap<QString,int> m_intervals;

        int             m_normalTimer;
};

#endif
