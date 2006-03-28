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

#include "rsiglobals.h"
#include "rsistats.h"

#include "rsitimer.h"

// The order here is important, otherwise Qt headers are preprocessed into garbage.... :-(

#include "config.h"     // HAVE_LIBXSS
#ifdef HAVE_LIBXSS      // Idle detection.
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/extensions/scrnsaver.h>
  #include <fixx11h.h>
#endif // HAVE_LIBXSS

RSITimer::RSITimer( QObject *parent, const char *name )
    : QObject( parent, name ), m_breakRequested( false ), m_suspended( false )
    , m_pause_left( 0 ), m_relax_left( 0 )
    , m_intervals( RSIGlobals::instance()->intervals() )
{
    kdDebug() << "Entering RSITimer::RSITimer" << endl;

#ifdef HAVE_LIBXSS      // Idle detection.
    int event_base, error_base;
    if(XScreenSaverQueryExtension(qt_xdisplay(), &event_base, &error_base))
        m_idleDetection = true;
#endif

    kdDebug() << "IDLE Detection is "
              << (m_idleDetection?QString::null:"not")
              << "possible" << endl;

    // if big_maximized < tiny_maximized, the bigbreaks will not get reset,
    // guard against that situation.
    if (m_intervals["big_maximized"] < m_intervals["tiny_maximized"])
    {
      kdDebug() << "max big > max tiny, not allowed & corrected" << endl;
      m_intervals["big_maximized"] = m_intervals["tiny_maximized"];
    }

    startTimer( 1000 );
    slotReadConfig();

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
}

// -------------------------- SLOTS ------------------------//

void RSITimer::slotStart()
{
    kdDebug() << "Entering RSITimer::slotStart" << endl;

    emit minimize();
    emit updateIdleAvg( 0.0 );
    m_suspended = false;
}

void RSITimer::slotStop()
{
    kdDebug() << "Entering RSITimer::slotStop" << endl;

    m_suspended = true;

    emit minimize();
    emit updateIdleAvg( 0.0 );
    emit updateToolTip( 0, 0 );
}

void RSITimer::slotSuspended( bool b )
{
    kdDebug() << "Entering RSITimer::slotSuspend" << endl;

    b ? slotStop() : slotStart();
}

void RSITimer::slotRestart()
{
    m_tiny_left = m_intervals["tiny_minimized"];
    m_big_left = m_intervals["big_minimized"];
    resetAfterBreak();
    slotStart();
}

void RSITimer::skipBreak()
{
    kdDebug() << "Entering RSITimer::skipBreak" << endl;

    if ( m_big_left <= m_tiny_left )
    {
        resetAfterBigBreak();
        RSIStats::instance()->increaseStat( BIG_BREAKS_SKIPPED );
    }
    else
    {
        resetAfterTinyBreak();
        RSIStats::instance()->increaseStat( TINY_BREAKS_SKIPPED );
    }
    slotStart();
}

void RSITimer::slotReadConfig()
{
    kdDebug() << "Entering RSITimer::slotReadConfig" << endl;
    readConfig();

    slotRestart();
}

void RSITimer::slotRequestBreak()
{
    kdDebug() << "Entering RSITimer::slotRequestBreak" << endl;
    m_breakRequested = true;
}

// ----------------------------- EVENTS -----------------------//

void RSITimer::timerEvent( QTimerEvent * )
{
    /**
      Contains the amount of time left for a big fullscreen break.
    */
    static int nextBreak = TINY_BREAK;

    // Dont change the tray icon when suspended, or evaluate
    // a possible break.
    if ( m_suspended )
        return;

    RSIStats::instance()->increaseStat( TOTAL_TIME );

    int t = idleTime();

    if ( t == 0 )
        RSIStats::instance()->increaseStat( ACTIVITY );
    else
        RSIStats::instance()->setStat( MAX_IDLENESS, t, true );

    /*
    kdDebug() << m_intervals["tiny_maximized"] << " " << m_intervals["big_maximized"] << " " << t << endl;
    */

    int breakInterval = m_tiny_left < m_big_left ?
            m_intervals["tiny_maximized"] : m_intervals["big_maximized"];
    nextBreak = m_tiny_left < m_big_left ? TINY_BREAK : BIG_BREAK;

    if ( m_breakRequested )
    {
        breakNow( breakInterval );
        m_pause_left = breakInterval;
        m_breakRequested = false;
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
            emit minimize();

            // make sure we clean up stuff in the code ahead
            if ( nextBreak == TINY_BREAK )
                resetAfterTinyBreak();
            else if ( nextBreak == BIG_BREAK )
                resetAfterBigBreak();

            emit updateToolTip( m_tiny_left, m_big_left );

            return;
        }
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
            if ( !m_patience ) // that's it!
            {
                emit relax( -1 );
                m_relax_left = 0;

                breakNow( breakInterval );
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
        }

        double value = 100 - ( ( m_tiny_left / (double)m_intervals["tiny_minimized"] ) * 100 );
        emit updateIdleAvg( value );
    }
    else if ( m_useIdleDetection && t == m_intervals["big_maximized"] &&
             m_intervals["tiny_maximized"] <= m_intervals["big_maximized"] )
    {
        // the user was sufficiently idle for a big break
        kdDebug() << "Time being idle == big break length" << endl;

        RSIStats::instance()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
        RSIStats::instance()->increaseStat( BIG_BREAKS );

        resetAfterBigBreak();
        emit bigBreakSkipped();
    }
    else if ( m_useIdleDetection && t == m_intervals["tiny_maximized"] &&
              m_tiny_left < m_big_left )
    {
        // the user was sufficiently idle for a tiny break
        kdDebug() << "Time being idle == tiny break length" << endl;

        RSIStats::instance()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
        RSIStats::instance()->increaseStat( TINY_BREAKS );

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

    if ( m_patience == 0 && m_pause_left == 0 && m_relax_left == 0 &&
         ( m_tiny_left == 0 || m_big_left == 0 ) )
    {
        if ( nextBreak == TINY_BREAK )
        {
            RSIStats::instance()->increaseStat( TINY_BREAKS );
        }
        else
        {
            RSIStats::instance()->increaseStat( BIG_BREAKS );
        }

        m_patience = 15;
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

      if ( between < m_intervals["big_minimized"] )
      {
          m_big_left = m_lastrunBig - between;
      }

      if ( between < m_intervals["tiny_minimized"] )
      {
          m_tiny_left = m_lastrunTiny - between;
      }
    }
}

#include "rsitimer.moc"
