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

        enum { TINY_BREAK = 0, BIG_BREAK = 1 };

    public slots:
        void slotReadConfig();
        void slotStop();
        /**
          Called when the user suspends RSIBreak from the docker.
          True means suspend, false means unsuspend.
        */
        void slotSuspended( bool );
        void slotStart();
        void slotRequestBreak();
        void slotRestart();
        void skipTinyBreak();

    protected:
        virtual void timerEvent( QTimerEvent* );

        void resetAfterBreak();
        void resetAfterTinyBreak();
        void resetAfterBigBreak();

    signals:
        /** Enforce a fullscreen big break. */
        void breakNow();

        /**
          Update counters in tooltip.
          @param tiny If <=0 a tiny break is active, else it defines how
          much time is left until the next tiny break.
          @param big If <=0 a big break is active, else it defines how
          much time is left until the next big break.
        */
        void updateToolTip( int tiny, int big );

        void updateWidget( int secondsLeft );

        /**
          Update the systray icon.
          @param v How much time has passed until a tiny break (relative)
                   Varies from 0 to 100.
        */
        void updateIdleAvg( double v );
        void minimize();
        void relax( int );

    private:
        void readConfig();
        int idleTime();
        void breakNow( int t );

        bool            m_breakRequested;
        bool            m_idleDetection;
        bool            m_suspended;

        int             m_tiny_left;
        int             m_big_left;
        int             m_pause_left;
        int             m_relax_left;
        /**
          When it's time for a break, we wait patiently till the user
          becomes idle. We show a relax popup during this interval.
          But if the user keeps active, our patience runs out and we
          we force him a break (full screen).
        */
        int             m_patience;

        QMap<QString,int> m_intervals;
};

#endif
