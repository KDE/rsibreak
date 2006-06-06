/* This file is part of the KDE project
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>
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

        enum { TINY_BREAK = 0, BIG_BREAK = 1, NO_BREAK = 2 };

    public slots:
        /**
          Reads the configuration and restarts the timer with
          slotRestart.
          @param restart If this is true, restart immediately. That
          means the timer is not suspended. If false, it will restart
          as soon the timer gets unsuspended.
        */
        void slotReadConfig( bool restart );

        /**
          Stops the timer activity. This does not imply
          resetting counters. Will not load an image
        */
        void slotStopNoImage();

        /**
          Stops the timer activity. This does not imply
          resetting counters.
          @param newImage Load a new image
         */
        void slotStop( bool newImage=true );

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
        void slotStartNoImage();

        /**
          Prepares the timer so that it can start/continue. This
          does not imply resetting counters.
          @param newImage Load a new image
         */
        void slotStart( bool newImage=true );

        /**
          The user can request a break from the docker. This function
          notifies the timer event handler of this request.
        */
        void slotRequestBreak();

        /** Forces a tiny break. */
        void slotRequestTinyBreak();

        /** Forces a big break. */
        void slotRequestBigBreak();

        /**
          Reset the timer. This implies resetting the counters for a
          tiny and big break.
        */
        void slotRestart();

        /**
          When the user presses the Skip button during a break,
          this function will be called. It will act like a break has
          just passed.
        */
        void skipBreak();

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
          @param secondsLeft Shows the user how many seconds are remaining.
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
          @param newImage Load a new image
        */
        void minimize( bool newImage );

        /**
          Pop up a relax notification to the user for @p sec seconds.
          @param sec The amount of seconds the user should relax to make the
          popup disappear. A value of -1 will hide the relax popup.
        */
        void relax( int sec );

        /**
          Indicates a tinyBreak is skipped because user was enough idle
        */
        void tinyBreakSkipped();

        /**
           Indicates a bigBreak is skipped because user was enough idle
         */
        void bigBreakSkipped();

        /**
         Emitted after bigBreakSkipped() and tinyBreakSkipped() on first
         activity;
         */
        void skipBreakEnded();

    protected: // TODO: What should be private and what not?
        void readConfig();
        void writeConfig();

        /**
          Restores the timers of RSIBreak when the application was closed and
          started in a short amount of time.
        */
        void restoreSession();

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
        bool            m_tinyBreakRequested;
        bool            m_bigBreakRequested;
        bool            m_suspended;
        bool            m_needRestart;

        int             m_tiny_left;
        int             m_big_left;
        int             m_pause_left;
        int             m_relax_left;
        int             m_useIdleDetection;
        int             dpmsOff;
        int             dpmsStandby;
        int             dpmsSuspend;

        QDateTime       m_lastActivity;
        /**
          When it's time for a break, we wait patiently till the user
          becomes idle. We show a relax popup during this interval.
          But if the user keeps active, our patience runs out and we
          we force him a break (full screen).
        */
        int             m_patience;

        // restore vars
        QDateTime       m_lastrunDt;
        int             m_lastrunTiny;
        int             m_lastrunBig;

        QMap<QString,int> &m_intervals;
};

/**
 * This timer is almost the same as the RSITimer, except it does not take
 * idle detection into account.
 *
 * @author Bram Schoenmakers <bramschoenmakers@kde.nl>
*/
class RSITimerNoIdle : public RSITimer
{
  Q_OBJECT
  public:
    RSITimerNoIdle( QObject *parent = 0, const char *name = 0 );
    ~RSITimerNoIdle();
  protected:
    virtual void timerEvent( QTimerEvent * );
};

#endif
