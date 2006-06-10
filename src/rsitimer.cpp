/* This file is part of the KDE project
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

   The parts for idle detection is based on
   kdepim's karm idletimedetector.cpp/.h

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

#include <kapplication.h>
#include <kdebug.h>
#include <kconfig.h>

// The order here is important, otherwise Qt headers are preprocessed into garbage.... :-(

#include "config.h"     // HAVE_LIBXSS
#ifdef HAVE_LIBXSS      // Idle detection.
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/extensions/scrnsaver.h>
    #include <fixx11h.h>
#endif // HAVE_LIBXSS

#include "rsiglobals.h"
#include "rsistats.h"
#include "rsitimer.h"
#include "rsitimer_dpms.h"


RSITimer::RSITimer( QObject *parent, const char *name )
    : QObject( parent, name ), m_breakRequested( false )
    , m_tinyBreakRequested( false )
    , m_bigBreakRequested( false )
    , m_suspended( false )
    , m_needRestart( false )
    , m_pause_left( 0 ), m_relax_left( 0 )
    , dpmsOff( -10 ), dpmsStandby(-10), dpmsSuspend(-10)
    , m_lastActivity( QDateTime::currentDateTime() )
    , m_intervals( RSIGlobals::instance()->intervals() )
{
    kdDebug() << "Entering RSITimer::RSITimer" << endl;

    // if big_maximized < tiny_maximized, the bigbreaks will not get reset,
    // guard against that situation.
    if (m_intervals["big_maximized"] < m_intervals["tiny_maximized"])
    {
      kdDebug() << "max big > max tiny, not allowed & corrected" << endl;
      m_intervals["big_maximized"] = m_intervals["tiny_maximized"];
    }

    startTimer( 1000 );
    slotReadConfig( /* restart */ true );

    m_tiny_left = m_intervals["tiny_minimized"];
    m_big_left = m_intervals["big_minimized"];

    restoreSession();
}

RSITimer::~RSITimer()
{
    writeConfig();
    kdDebug() << "Entering RSITimer::~RSITimer" << endl;
}

int RSITimer::idleTime()
{
    int totalIdle = 0;

#ifdef HAVE_LIBXSS      // Idle detection.
    XScreenSaverInfo*  _mit_info;
    _mit_info= XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(qt_xdisplay(), qt_xrootwin(), _mit_info);
    totalIdle = (_mit_info->idle/1000);
    XFree(_mit_info);

    // When dpms turns off the monitor the idle gets a reset to 0
    // Eat the activity in that area. Bug 6439 bugs.freedesktop.org
    int t = m_lastActivity.secsTo(QDateTime::currentDateTime());

    // refresh timings when there is idleness for 6 minutes, should be
    // enough to have values before it is needed.
    if (t == 3600)
    {
        QueryDPMSTimeouts(qt_xdisplay(), dpmsStandby, dpmsSuspend, dpmsOff);
        kdDebug() << "DPMS settings: " << dpmsStandby << " - "
                << dpmsSuspend << " - " << dpmsOff << endl;
    }

    if (totalIdle == 0 && ( t < dpmsOff-1     || t > dpmsOff+1     )
                       && ( t < dpmsStandby-1 || t > dpmsStandby+1 )
                       && ( t < dpmsSuspend-1 || t > dpmsSuspend+1 ))
        m_lastActivity=QDateTime::currentDateTime();
    else
        totalIdle = m_lastActivity.secsTo(QDateTime::currentDateTime());

#else
    totalIdle = m_pause_left > 0 ? 1 : 0;
#endif // HAVE_LIBXSS

    return totalIdle;
}

void RSITimer::breakNow( int t )
{
    kdDebug() << "Entering RSITimer::breakNow for " << t << "seconds " << endl;

    emit updateWidget( t );
    emit breakNow();
}

void RSITimer::resetAfterBreak()
{
    kdDebug() << "Entering RSITimer::resetAfterBreak" << endl;

    m_pause_left = 0;
    m_relax_left = 0;
    m_patience = 0;
    emit relax( -1 );
    updateIdleAvg( 0.0 );
}

void RSITimer::resetAfterTinyBreak()
{
    kdDebug() << "Entering RSITimer::resetAfterTinyBreak" << endl;

    m_tiny_left = m_intervals["tiny_minimized"];
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    RSIGlobals::instance()->DCOPBreak( false, false );

    if ( m_big_left < m_tiny_left )
    {
        // don't risk a big break just right after a tiny break, so delay it a bit
        m_big_left += m_tiny_left - m_big_left;
    }
}

void RSITimer::resetAfterBigBreak()
{
    kdDebug() << "Entering RSITimer::resetAfterBigBreak" << endl;

    m_tiny_left = m_intervals["tiny_minimized"];
    m_big_left = m_intervals["big_minimized"];
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    RSIGlobals::instance()->DCOPBreak( false, true );
}

// -------------------------- SLOTS ------------------------//

void RSITimer::slotStartNoImage( )
{
    slotStart( false );
}


void RSITimer::slotStart( bool newImage )
{
    kdDebug() << "Entering RSITimer::slotStart " << newImage << endl;

    emit minimize( newImage );
    emit updateIdleAvg( 0.0 );
    m_suspended = false;
}

void RSITimer::slotStopNoImage()
{
    slotStop( false );
}

void RSITimer::slotStop( bool newImage )
{
    kdDebug() << "Entering RSITimer::slotStop " << newImage << endl;

    m_suspended = true;

    emit minimize( newImage );
    emit updateIdleAvg( 0.0 );
    emit updateToolTip( 0, 0 );
}

void RSITimer::slotSuspended( bool b )
{
    kdDebug() << "Entering RSITimer::slotSuspend" << endl;

    m_needRestart ? slotRestart() : (b ? slotStop() : slotStart() );
}

void RSITimer::slotRestart()
{
    kdDebug() << "Entering RSITimer::slotRestart" << endl;

    m_tiny_left = m_intervals["tiny_minimized"];
    m_big_left = m_intervals["big_minimized"];
    resetAfterBreak();
    slotStart( false );
    m_needRestart = false;
}

void RSITimer::skipBreak()
{
    kdDebug() << "Entering RSITimer::skipBreak" << endl;

    if ( m_big_left <= m_tiny_left )
    {
        resetAfterBigBreak();
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS_SKIPPED );
    }
    else
    {
        resetAfterTinyBreak();
        RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS_SKIPPED );
    }
    slotStart();
}

void RSITimer::slotReadConfig( bool restart )
{
    kdDebug() << "Entering RSITimer::slotReadConfig" << endl;
    readConfig();

    m_intervals = RSIGlobals::instance()->intervals();
    if ( restart )
      slotRestart();
    else
      m_needRestart = true;
}

void RSITimer::slotRequestBreak()
{
    kdDebug() << "Entering RSITimer::slotRequestBreak" << endl;
    m_breakRequested = true;
}

void RSITimer::slotRequestTinyBreak()
{
  kdDebug() << "Entering RSITimer::slotRequestTinyBreak" << endl;
  m_breakRequested = true;
  RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
  if ( !m_bigBreakRequested )
    m_tinyBreakRequested = true;
}

void RSITimer::slotRequestBigBreak()
{
  kdDebug() << "Entering RSITimer::slotRequestBigBreak" << endl;
  m_breakRequested = true;
  RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
  if ( !m_tinyBreakRequested )
    m_bigBreakRequested = true;
}

// ----------------------------- EVENTS -----------------------//

void RSITimer::timerEvent( QTimerEvent * )
{
    /*
      Contains the amount of time left for a big fullscreen break.
    */
    static int nextBreak = TINY_BREAK;

    // Dont change the tray icon when suspended, or evaluate
    // a possible break.
    if ( m_suspended )
        return;

    RSIGlobals::instance()->stats()->increaseStat( TOTAL_TIME );

    int t = idleTime();

    if ( t == 0 )
    {
        RSIGlobals::instance()->stats()->increaseStat( ACTIVITY );
        RSIGlobals::instance()->stats()->setStat( CURRENT_IDLE_TIME, 0 );
    }
    else
    {
        RSIGlobals::instance()->stats()->setStat( MAX_IDLENESS, t, true );
        RSIGlobals::instance()->stats()->setStat( CURRENT_IDLE_TIME, t );
    }

    /*
    kdDebug() << m_intervals["tiny_maximized"] << " " << m_intervals["big_maximized"] << " " << t << endl;
    */

    int breakInterval = m_tiny_left < m_big_left ?
            m_intervals["tiny_maximized"] : m_intervals["big_maximized"];
    nextBreak = m_tiny_left < m_big_left ? TINY_BREAK : BIG_BREAK;

    if ( m_breakRequested )
    {
        if ( m_tinyBreakRequested )
        {
          breakNow( m_intervals["tiny_maximized"] );
          m_pause_left = m_intervals["tiny_maximized"];
          nextBreak = TINY_BREAK;
          RSIGlobals::instance()->DCOPBreak( true, false );
        }
        else if ( m_bigBreakRequested )
        {
          breakNow( m_intervals["big_maximized"] );
          m_pause_left = m_intervals["big_maximized"];
          nextBreak = BIG_BREAK;
          RSIGlobals::instance()->DCOPBreak( true, true );
        }
        else
        {
          breakNow( breakInterval );
          m_pause_left = breakInterval;
        }

        m_breakRequested = false;
        m_bigBreakRequested = false;
        m_tinyBreakRequested = false;
        m_relax_left = 0;
    }

    if ( t > 0 && m_pause_left > 0 ) // means: widget is maximized
    {
        if ( m_pause_left - 1 > 0 ) // break is not over yet
        {
            --m_pause_left;
            updateWidget( m_pause_left );
        }
        else // user survived the break, set him/her free
        {
            emit minimize( true );

            // make sure we clean up stuff in the code ahead
            if ( nextBreak == TINY_BREAK )
                resetAfterTinyBreak();
            else if ( nextBreak == BIG_BREAK )
                resetAfterBigBreak();

            emit updateToolTip( m_tiny_left, m_big_left );
        }

        return;
    }

    /*
    kdDebug() << " patience: " << m_patience  << " pause_left: "
            << m_pause_left << " relax_left: " << m_relax_left
            <<  " tiny_left: " << m_tiny_left  << " big_left: "
            <<  m_big_left << " idle: " << t << endl;
    */

    if ( t == 0 ) // activity!
    {
        if ( m_patience > 0 ) // we're trying to break
        {
            --m_patience;
            if ( m_patience == 0 ) // that's it!
            {
                emit relax( -1 );
                m_relax_left = 0;

                breakNow( breakInterval );
                nextBreak == TINY_BREAK  ?
                        RSIGlobals::instance()->DCOPBreak( true, false ):
                        RSIGlobals::instance()->DCOPBreak( true, true );
                m_pause_left = breakInterval;
            }
            else // reset relax dialog
            {
                emit relax( breakInterval );
                m_relax_left = breakInterval;
            }
        }
        else if ( m_relax_left > 0 )
        {
            // no patience left and still moving during a relax moment?
            // this will teach him
            breakNow( m_relax_left );
            m_pause_left = m_relax_left;
            m_relax_left = 0;
            emit relax( -1 );
        }
        else if ( m_pause_left == 0 )
        {
            // there's no relax moment or break going on.

            // If we emitted tiny/bigBreakSkipped then we have
            // to emit a signal again when user becomes active.
            // so if the timers are original, emit it.
            if ( m_tiny_left == m_intervals["tiny_minimized"] ||
                m_big_left == m_intervals["big_minimized"] )
            {
                emit skipBreakEnded();
            }

            --m_tiny_left;
            --m_big_left;

            // This is an extra safeguard. When m_useIdleDetection is false
            // timers are not reset after the user have had a break. This
            // will make sure the timers are being reset when this happens.
            if (m_tiny_left < -1 || m_big_left < -1)
            {
                if ( nextBreak == TINY_BREAK )
                    resetAfterTinyBreak();
                else if ( nextBreak == BIG_BREAK )
                    resetAfterBigBreak();
            }
        }

        double value = 100 - ( ( m_tiny_left / (double)m_intervals["tiny_minimized"] ) * 100 );
        emit updateIdleAvg( value );
    }
    else if ( m_useIdleDetection && t == m_intervals["big_maximized"] &&
             m_intervals["tiny_maximized"] <= m_intervals["big_maximized"] )
    {
        // the user was sufficiently idle for a big break
        kdDebug() << "Time being idle == big break length" << endl;

        if ( m_relax_left == 0 && m_pause_left == 0 )
        {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
            RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
        }

        resetAfterBigBreak();
        emit bigBreakSkipped();
    }
    else if ( m_useIdleDetection && t == m_intervals["tiny_maximized"] &&
              m_tiny_left < m_big_left  )
    {
        // the user was sufficiently idle for a tiny break
        kdDebug() << "Time being idle == tiny break length" << endl;

        if ( m_relax_left == 0 && m_pause_left == 0 )
        {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
            RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
        }

        resetAfterTinyBreak();
        emit tinyBreakSkipped();
    }
    else if ( m_relax_left > 0 )
    {
        --m_relax_left;

        // just in case the user dares to become active
        --m_patience;

        emit relax( m_relax_left );
    }

    // update the stats properly when breaking
    if ( m_useIdleDetection && t > m_intervals["big_maximized"] &&
         m_relax_left == 0 && m_pause_left == 0 )
    {
        RSIGlobals::instance()->stats()->setStat( LAST_BIG_BREAK, QVariant( QDateTime::currentDateTime() ) );
    }

    // update the stats properly when breaking
    if ( m_useIdleDetection && t > m_intervals["tiny_maximized"] &&
         m_relax_left == 0 && m_pause_left == 0 )
    {
        RSIGlobals::instance()->stats()->setStat( LAST_TINY_BREAK, QVariant( QDateTime::currentDateTime() ) );
    }


    // show relax popup
    if ( m_patience == 0 && m_pause_left == 0 && m_relax_left == 0 &&
         ( m_tiny_left == 0 || m_big_left == 0 ) )
    {
        if ( nextBreak == TINY_BREAK )
        {
            RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
        }
        else
        {
            RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
        }

        m_patience = 15;
        if (m_patience > breakInterval)
            m_patience=breakInterval;

        emit relax( breakInterval );
        m_relax_left = breakInterval;
    }

    emit updateToolTip( m_tiny_left, m_big_left );
}

//--------------------------- CONFIG ----------------------------//

void RSITimer::readConfig()
{
    kdDebug() << "Entering RSITimer::readConfig" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General Settings");

    m_useIdleDetection = config->readBoolEntry("UseIdleDetection", true);

    config->setGroup("General");
    QDateTime *tempDt = new QDateTime();
    m_lastrunDt = config->readDateTimeEntry( "LastRunTimeStamp", tempDt );
    m_lastrunTiny = config->readNumEntry( "LastRunTinyLeft", 0 );
    m_lastrunBig = config->readNumEntry( "LastRunBigLeft", 0 );

    delete tempDt;
    tempDt = 0;
}

void RSITimer::writeConfig()
{
    kdDebug() << "Entering RSITimer::writeConfig" << endl;
    KConfig *config = kapp->config();

    config->setGroup("General");
    config->writeEntry( "LastRunTimeStamp", QDateTime::currentDateTime() );
    config->writeEntry( "LastRunTinyLeft", m_tiny_left );
    config->writeEntry( "LastRunBigLeft", m_big_left );
}

void RSITimer::restoreSession()
{
    if ( !m_lastrunDt.isNull() )
    {
      int between = m_lastrunDt.secsTo( QDateTime::currentDateTime() );

      if ( between < m_intervals["big_minimized"] &&
           (m_lastrunBig - between) > 20 )
      {
          m_big_left = m_lastrunBig - between;
      }

      if ( between < m_intervals["tiny_minimized"] &&
           (m_lastrunTiny - between) > 20 )
      {
          m_tiny_left = m_lastrunTiny - between;
      }
    }
}



RSITimerNoIdle::RSITimerNoIdle( QObject *parent, const char *name )
: RSITimer( parent, name )
{
}

RSITimerNoIdle::~RSITimerNoIdle()
{
}

void RSITimerNoIdle::timerEvent( QTimerEvent * )
{
    // TODO: Statistics
    // TODO: Set docker icon properly
    // TODO: m_tinyBreakRequested && m_bigBreakRequested

    int breakInterval = m_tiny_left < m_big_left ?
            m_intervals["tiny_maximized"] : m_intervals["big_maximized"];
    static int currentBreak = NO_BREAK;

    if ( m_pause_left > 0 )
    {
        --m_pause_left;
        if ( m_pause_left == 0 )
        {
            // break is over
            emit relax( -1 );
            if ( currentBreak == TINY_BREAK )
            {
                resetAfterTinyBreak();
            }
            else if ( currentBreak == BIG_BREAK )
            {
                resetAfterBigBreak();
            }

            currentBreak = NO_BREAK;
        }
        else
        {
            emit relax( m_pause_left );
        }
    }

    if ( m_pause_left == 0 && m_tiny_left == 0 )
    {
        emit relax( breakInterval );
        m_pause_left = breakInterval;
        currentBreak = TINY_BREAK;
    }
    else
    {
        --m_tiny_left;
    }

    if ( m_pause_left == 0 && m_big_left == 0 )
    {
        emit relax( breakInterval );
        m_pause_left = breakInterval;
        currentBreak = BIG_BREAK;
    }
    else
    {
        --m_big_left;
    }

    emit updateToolTip( m_tiny_left, m_big_left );
}

#include "rsitimer.moc"
