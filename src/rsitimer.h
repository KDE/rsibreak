/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

        /**
          Default destructor.
        */
        ~RSITimer();

        /**
          Check whether the timer is suspended.
        */
        bool isSuspended() const { return m_suspended; }

        enum { TINY_BREAK = 0, BIG_BREAK = 1 };

    public slots:
        /**
          Reads the configuration and restarts the timer with
          slotRestart.
        */
        void slotReadConfig();

        /**
          Stops the timer activity. This does not imply
          resetting counters.
        */
        void slotStop();

        /**
          Called when the user suspends RSIBreak from the docker.
          @param suspend If true the timer will suspend,
          if false the timer will unsuspend.
        */
        void slotSuspended( bool suspend );

        /**
          Prepares the timer so that it can start/continue. This
          does not imply resetting counters.
        */
        void slotStart();

        /**
          The user can request a break from the docker. This function
          notifies the timer event handler of this request.
        */
        void slotRequestBreak();

        /**
          Reset the timer. This implies resetting the counters for a
          tiny and big break.
        */
        void slotRestart();

        /**
          When the user presses the Skip button, this function will be
          called. It will act like a tiny break has just passed.
        */
        void skipTinyBreak();

    protected:
        /**
          The pumping heart of the timer. This will evaluate user's activity
          and decide what to do (wait, popup a relax notification or a
          fullscreen break.
          You shouldn't call this function directly.
        */
        virtual void timerEvent( QTimerEvent* );

        /** This function is called when a break has passed. */
        void resetAfterBreak();

        /** Called when the user was idle for the duration of a tiny break. */
        void resetAfterTinyBreak();

        /** Called when the user was idle for the duration of a big break. */
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

        /**
          Update the time shown on the fullscreen widget.
          @param secondsleft Shows the user how many seconds are remaining.
        */
        void updateWidget( int secondsLeft );

        /**
          Update the systray icon.
          @param v How much time has passed until a tiny break (relative)
                   Varies from 0 to 100.
        */
        void updateIdleAvg( double v );

        /**
          A request to minimize the fullscreen widget, for example when the
          break is over.
        */
        void minimize();

        /**
          Pop up a relax notification to the user for @p sec seconds.
          @param sec The amount of seconds the user should relax to make the
          popup disappear. A value of -1 will hide the relax popup.
        */
        void relax( int sec );

    private:
        void readConfig();
        /**
          Queries X how many seconds the user has been idle. A value of 0
          means there was activity during the last second.
          @returns The amount of seconds of idling.
        */
        int idleTime();

        /**
          Some internal preparations for a fullscreen break window.
          @param t The amount of seconds to break.
        */
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
