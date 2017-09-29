/*
   Copyright (C) 2005-2006,2008-2010 Tom Albers <toma@kde.org>
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

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

#include <QThread>
#include <memory>

#include "rsiglobals.h"
#include "rsitimercounter.h"
#include "rsiidletime.h"

/**
 * @class RSITimer
 * This class controls the timings and arranges the maximizing
 * and minimizing of the widget.
 * @author Tom Albers <toma.org>
 */
class RSITimer : public QThread
{
    Q_OBJECT
    friend class RSITimerTest;

public:
    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit RSITimer( QObject *parent = 0 );

    // Check whether the timer is suspended.
    bool isSuspended() const { return m_state == TimerState::Suspended; }

    int tinyLeft() { return m_tinyBreakCounter->counterLeft(); };

    int bigLeft() { return m_bigBreakCounter->counterLeft(); };

public slots:
    /**
      Reads the configuration and restarts the timer with slotRestart.
    */
    void updateConfig( bool doRestart = false );

    /**
      Stops the timer activity. This does not imply resetting counters.
     */
    void slotStop();

    /**
      Called when the user suspends RSIBreak from the docker.
      @param suspend If true the timer will suspend, if false the timer will unsuspend.
    */
    void slotSuspended( bool suspend );

    /**
      Prepares the timer so that it can start/continue.
     */
    void slotStart();

    /**
      Called when user locks the screen for pause. Resets current timers if currently suggesting.
    */
    void slotLock();

    /**
      When the user presses the Skip button during a break,
      this function will be called. It will act like a break has
      just passed.
    */
    void skipBreak();

    /**
      When the user presses the postpone break button during a break,
      this function will be called. It will postpone the break for the
      configured amount of seconds.
    */
    void postponeBreak();

    /**
      Queries X how many seconds the user has been idle. A value of 0
      means there was activity during the last second.
      @returns The amount of seconds of idling.
    */
    int idleTime();

private slots:
    /**
      The pumping heart of the timer. This will evaluate user's activity and
      decide what to do (wait, popup a relax notification or a fullscreen break.
    */
    virtual void timeout();

signals:
    /** Enforce a fullscreen big break. */
    void breakNow();

    /**
      Update counters in tooltip.
      @param tinyLeft If <=0 a tiny break is active, else it defines how
      much time is left until the next tiny break.
      @param bigLeft If <=0 a big break is active, else it defines how
      much time is left until the next big break.
    */
    void updateToolTip( const int tinyLeft, const int bigLeft );

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
    void minimize();

    /**
      Pop up a relax notification to the user for @p sec seconds.
      @param sec The amount of seconds the user should relax to make the
      popup disappear. A value of -1 will hide the relax popup.
      @param nextBreakIsBig True if the break after the next break is a big break.
      We can warn the user in advance.
    */
    void relax( int sec, bool nextBreakIsBig );

    /**
      Indicates a tinyBreak is skipped because user was enough idle
    */
    void tinyBreakSkipped();

    /**
       Indicates a bigBreak is skipped because user was enough idle
     */
    void bigBreakSkipped();

private:
    std::unique_ptr<RSIIdleTime> m_idleTimeInstance;

    bool m_usePopup;
    bool m_useIdleTimers;
    QVector<int> m_intervals;

    enum class TimerState {
        Suspended = 0,      // user has suspended either via dbus or tray.
        Monitoring,         // normal cycle, waiting for break to trigger.
        Suggesting,         // politely suggest to take a break with some patience.
        Resting             // suggestion ignored, waiting out the break.
    } m_state;

    std::unique_ptr<RSITimerCounter> m_bigBreakCounter;
    std::unique_ptr<RSITimerCounter> m_tinyBreakCounter;
    std::unique_ptr<RSITimerCounter> m_pauseCounter;
    std::unique_ptr<RSITimerCounter> m_popupCounter;

    void hibernationDetector( const int totalIdle );
    void suggestBreak( const int time );
    void defaultUpdateToolTip();
    void createTimers();

    // This function is called when a break has passed.
    void resetAfterBreak();

    // Start this thread.
    void run();

    /**
      Some internal preparations for a fullscreen break window.
      @param breakTime The amount of seconds to break.
      @param nextBreakIsBig Whether the next break will be big.
    */
    void doBreakNow( const int breakTime, const bool nextBreakIsBig );

    // Constructor for tests. Ownership is taken over for _idleTime.
    RSITimer( RSIIdleTime* _idleTime, const QVector<int> _intervals, const bool _usePopup, const bool _useIdleTimers );
};

#endif
