/*
   Copyright (C) 2005-2006,2008-2010 Tom Albers <toma@kde.org>
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

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

#include "rsitimer.h"

#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kidletime.h>
#include <ksharedconfig.h>

// The order here is important, otherwise Qt headers are preprocessed into garbage.... :-(

#include <X11/Xlib.h>
#include <fixx11h.h>

#include "rsiglobals.h"
#include "rsistats.h"
#include <QTimer>
#include <QThread>
#include <QX11Info>
#include <QDebug>

RSITimer::RSITimer( QObject *parent )
        : QThread( parent ), m_breakRequested( false )
        , m_tinyBreakRequested( false )
        , m_bigBreakRequested( false )
        , m_suspended( false )
        , m_needRestart( false )
        , m_pause_left( 0 ), m_relax_left( 0 ), m_patience( 0 )
        , m_intervals( RSIGlobals::instance()->intervals() )
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    m_debug = config.readEntry( "DEBUG", 0 );
}

RSITimer::~RSITimer()
{
    writeConfig();
}

void RSITimer::run()
{
    QTimer timer;
    connect(&timer, &QTimer::timeout, this, &RSITimer::timeout);
    timer.start( 1000 );
    slotReadConfig( /* restart */ true );

    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    m_big_left = m_intervals[BIG_MINIMIZED_INTERVAL];

    restoreSession();
    exec(); //make timers work
}

void RSITimer::hibernationDetector()
{
    //poor mans hibernation detector....
    static QDateTime last = QDateTime::currentDateTime();
    QDateTime current = QDateTime::currentDateTime();
    if ( last.secsTo( current ) > 60 ) {
        qDebug() << "Not been checking idleTime for more than 60 seconds, "
        << "assuming the computer hibernated, resetting timers"
        << "Last: " << last
        << "Current: " << current;
        resetAfterBigBreak();
    }
    last = current;
}

int RSITimer::idleTime()
{
    int totalIdle = 0;

    // TODO Find a modern-desktop way to check if the screensaver is inhibited
    // and disable the timer because we assume you're doing for example a presentation and
    // don't want rsibreak to annoy you
    hibernationDetector();

    int totalIdleMs = KIdleTime::instance()->idleTime();
    totalIdle = totalIdleMs == 0 ? 0 : totalIdleMs/1000;

    return totalIdle;
}

void RSITimer::doBreakNow( int t )
{
    emit updateWidget( t );
    emit breakNow();
}

void RSITimer::resetAfterBreak()
{
    m_pause_left = 0;
    m_relax_left = 0;
    m_patience = 0;
    emit relax( -1, false );
    updateIdleAvg( 0.0 );
    m_nextBreak = m_tiny_left < m_big_left ? TINY_BREAK : BIG_BREAK;
    // and what about the break after the next break? pass it along relax()
    // so we can warn the user in advance
    m_nextnextBreak = m_nextBreak == TINY_BREAK &&
                      m_big_left <= 2 * m_tiny_left ? BIG_BREAK : TINY_BREAK;
}

void RSITimer::resetAfterTinyBreak()
{
    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    qDebug() << "********** resetAfterTinyBreak !!";
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    RSIGlobals::instance()->NotifyBreak( false, false );

    if ( m_big_left < m_tiny_left ) {
        // don't risk a big break just right after a tiny break, so delay it a bit
        m_big_left += m_tiny_left - m_big_left;
    }
}

void RSITimer::resetAfterBigBreak()
{
    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    qDebug() << "********** resetAfterBigBreak !!";
    m_big_left = m_intervals[BIG_MINIMIZED_INTERVAL];
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    RSIGlobals::instance()->NotifyBreak( false, true );
}

// -------------------------- SLOTS ------------------------//

void RSITimer::slotStart()
{
    emit updateIdleAvg( 0.0 );
    m_suspended = false;
}

void RSITimer::slotStop()
{
    m_suspended = true;
    emit updateIdleAvg( 0.0 );
    emit updateToolTip( 0, 0 );
}

void RSITimer::slotSuspended( bool b )
{
    m_needRestart ? slotRestart() : ( b ? slotStop() : slotStart() );
}

void RSITimer::slotRestart()
{
    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    qDebug() << "********** slotRestart !!";
    m_big_left = m_intervals[BIG_MINIMIZED_INTERVAL];
    resetAfterBreak();
    slotStart();
    m_needRestart = false;
}

void RSITimer::skipBreak()
{
    emit minimize();
    if ( m_big_left <= m_tiny_left + 1 ) {
        resetAfterBigBreak();
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS_SKIPPED );
    } else {
        resetAfterTinyBreak();
        RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS_SKIPPED );
    }
    slotStart();
}

void RSITimer::postponeBreak()
{
   qDebug() << "kidle: On postponeBreak !!" << m_intervals[POSTPONE_BREAK_INTERVAL];
   m_patience = 0;
   m_relax_left = 0;
   m_pause_left = 0;   
   m_big_left += m_intervals[POSTPONE_BREAK_INTERVAL];
   m_tiny_left += m_intervals[POSTPONE_BREAK_INTERVAL];

   if ( m_big_left <= m_tiny_left + 1 ) {
       RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS_POSTPONED );
   } else {
       RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS_POSTPONED );
   }
  
   emit relax( -1, false );
   emit updateToolTip( m_tiny_left, m_big_left );
   emit minimize();  
}

void RSITimer::slotReadConfig( bool restart )
{
    const QVector<int> oldIntervals = m_intervals;
    readConfig();
    m_intervals = RSIGlobals::instance()->intervals();
    if ( restart ) {
        if ( m_intervals == oldIntervals ) {
            qDebug() << "No change in timers, continue...";
        } else {
            qDebug() << "Change in timers, reset...";
            slotRestart();
        }
    } else
        m_needRestart = true;
}

void RSITimer::slotRequestBreak()
{
    m_breakRequested = true;
}

void RSITimer::slotRequestTinyBreak()
{
    slotRequestBreak();
    if ( !m_bigBreakRequested ) {
        m_tinyBreakRequested = true;
        RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
    }
}

void RSITimer::slotRequestBigBreak()
{
    slotRequestBreak();
    if ( !m_tinyBreakRequested ) {
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
        m_bigBreakRequested = true;
    }
}

// ----------------------------- EVENTS -----------------------//

void RSITimer::timeout()
{
    int t = idleTime();

    // Don't change the tray icon when suspended, or evaluate
    // a possible break.
    if ( m_suspended )
        return;

    RSIGlobals::instance()->stats()->increaseStat( TOTAL_TIME );

    if ( t == 0 ) {
        RSIGlobals::instance()->stats()->increaseStat( ACTIVITY );
        RSIGlobals::instance()->stats()->setStat( CURRENT_IDLE_TIME, 0 );
    } else {
        RSIGlobals::instance()->stats()->setStat( MAX_IDLENESS, t, true );
        RSIGlobals::instance()->stats()->setStat( CURRENT_IDLE_TIME, t );
    }

    /*
    qDebug() << m_intervals[TINY_MAXIMIZED_INTERVAL] << " " << m_intervals[BIG_MAXIMIZED_INTERVAL] << " " << t;
    */

    int breakInterval = m_tiny_left < m_big_left ?
                        m_intervals[TINY_MAXIMIZED_INTERVAL] : m_intervals[BIG_MAXIMIZED_INTERVAL];


    if ( m_breakRequested ) {
        if ( m_tinyBreakRequested ) {
            doBreakNow( m_intervals[TINY_MAXIMIZED_INTERVAL] );
            m_pause_left = m_intervals[TINY_MAXIMIZED_INTERVAL];
            m_nextBreak = TINY_BREAK;
            RSIGlobals::instance()->NotifyBreak( true, false );
        } else if ( m_bigBreakRequested ) {
            doBreakNow( m_intervals[BIG_MAXIMIZED_INTERVAL] );
            m_pause_left = m_intervals[BIG_MAXIMIZED_INTERVAL];
            m_nextBreak = BIG_BREAK;
            RSIGlobals::instance()->NotifyBreak( true, true );
        } else {
            doBreakNow( breakInterval );
            m_pause_left = breakInterval;
        }

        m_breakRequested = false;
        m_bigBreakRequested = false;
        m_tinyBreakRequested = false;
        m_relax_left = 0;
    }

    if ( t > 0 && m_pause_left > 0 ) { // means: widget is maximized
        if ( m_pause_left - 1 > 0 ) { // break is not over yet
            --m_pause_left;
            updateWidget( m_pause_left );
        } else { // user survived the break, set him/her free
            emit minimize();

            // make sure we clean up stuff in the code ahead
            if ( m_nextBreak == TINY_BREAK )
                resetAfterTinyBreak();
            else if ( m_nextBreak == BIG_BREAK )
                resetAfterBigBreak();

            emit updateToolTip( m_tiny_left, m_big_left );
        }

        return;
    }

    // ignore the idle time if that setting is activated.
    // simulate no activity. 
    if ( m_ignoreIdleForTinyBreaks && m_relax_left > 0 && 
         m_tiny_left < m_big_left) {
        t = breakInterval-m_relax_left+1;
        if ( m_debug > 1 )
            qDebug() << "idleness reset" << t;
    }

    if ( m_debug > 1 ) {
        qDebug() << " patience: " << m_patience  << " pause_left: "
        << m_pause_left << " relax_left: " << m_relax_left
        <<  " tiny_left: " << m_tiny_left  << " big_left: "
        <<  m_big_left << " idle: " << t << endl;
    }

    if ( t == 0 ) { // activity!
        if ( m_patience > 0 ) { // we're trying to break
            --m_patience;
            if ( m_patience == 0 ) { // that's it!
                emit relax( -1, false );
                m_relax_left = 0;

                doBreakNow( breakInterval );
                m_nextBreak == TINY_BREAK  ?
                RSIGlobals::instance()->NotifyBreak( true, false ) :
                RSIGlobals::instance()->NotifyBreak( true, true );
                m_pause_left = breakInterval;
            } else { // reset relax dialog
                emit relax( breakInterval, m_nextnextBreak == BIG_BREAK );
                m_relax_left = breakInterval;
            }
        } else if ( m_relax_left > 0 ) {
            // no patience left and still moving during a relax moment?
            // this will teach him
            doBreakNow( m_relax_left );
            m_pause_left = m_relax_left;
            m_relax_left = 0;
            emit relax( -1, false );
        } else if ( m_pause_left == 0 ) {
            // there's no relax moment or break going on.

            --m_tiny_left;
            --m_big_left;

            // This is an extra safeguard. When m_useIdleDetection is false
            // timers are not reset after the user have had a break. This
            // will make sure the timers are being reset when this happens.
            if ( m_tiny_left < -1 || m_big_left < -1 ) {
                if ( m_nextBreak == TINY_BREAK )
                    resetAfterTinyBreak();
                else if ( m_nextBreak == BIG_BREAK )
                    resetAfterBigBreak();
            }
        }

        double value = 100 - (( m_tiny_left / ( double )m_intervals[TINY_MINIMIZED_INTERVAL] ) * 100 );
        emit updateIdleAvg( value );
    } else if ( m_useIdleDetection && t == m_intervals[BIG_MAXIMIZED_INTERVAL] &&
                m_intervals[TINY_MAXIMIZED_INTERVAL] <= m_intervals[BIG_MAXIMIZED_INTERVAL] &&
                /* sigh, sometime we get 2 times in row the same idle ness
                   so, to prevent the signal emitted, we also chech that the
                   big_left does not equal the regular interval */
                m_big_left != m_intervals[BIG_MINIMIZED_INTERVAL] ) {
        // the user was sufficiently idle for a big break
        if ( m_relax_left == 0 && m_pause_left == 0 ) {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
            RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
        }

        resetAfterBigBreak();
        emit bigBreakSkipped();
    } else if ( m_useIdleDetection && t == m_intervals[TINY_MAXIMIZED_INTERVAL] &&
                m_tiny_left < m_big_left &&
                /* sigh, sometime we get 2 times in row the same idle ness
                   so, to prevent the signal emitted, we also chech that the
                   big_left does not equal the regular interval */
                m_tiny_left != m_intervals[TINY_MINIMIZED_INTERVAL] ) {
        // the user was sufficiently idle for a tiny break
        if ( m_relax_left == 0 && m_pause_left == 0 ) {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
            RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
        }

        resetAfterTinyBreak();
        emit tinyBreakSkipped();
    } else if ( m_relax_left > 0 ) {
        --m_relax_left;

        // just in case the user dares to become active
        --m_patience;

        emit relax( m_relax_left, m_nextnextBreak == BIG_BREAK );
    }

    // update the stats properly when breaking
    if ( m_useIdleDetection && t > m_intervals[BIG_MAXIMIZED_INTERVAL] &&
            m_relax_left == 0 && m_pause_left == 0 ) {
        RSIGlobals::instance()->stats()->setStat( LAST_BIG_BREAK, QVariant( QDateTime::currentDateTime() ) );
    }

    // update the stats properly when breaking
    if ( m_useIdleDetection && t > m_intervals[TINY_MAXIMIZED_INTERVAL] &&
            m_relax_left == 0 && m_pause_left == 0 ) {
        RSIGlobals::instance()->stats()->setStat( LAST_TINY_BREAK, QVariant( QDateTime::currentDateTime() ) );
    }


    // show relax popup
    if ( m_patience == 0 && m_pause_left == 0 && m_relax_left == 0 &&
            ( m_tiny_left == 0 || m_big_left == 0 ) ) {
        if ( m_nextBreak == TINY_BREAK ) {
            RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
        } else {
            RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
        }

        m_patience = m_intervals[PATIENCE_INTERVAL];

        emit relax( breakInterval, m_nextnextBreak == BIG_BREAK );
        m_relax_left = breakInterval;
    }

    emit updateToolTip( m_tiny_left, m_big_left );
}

//--------------------------- CONFIG ----------------------------//

void RSITimer::readConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );

    m_useIdleDetection = config.readEntry( "UseIdleDetection", true );
    m_ignoreIdleForTinyBreaks = config.readEntry( "IgnoreIdleForTinyBreaks", false );

    config = KSharedConfig::openConfig()->group( "General" );
    m_lastrunDt = config.readEntry( "LastRunTimeStamp", QDateTime() );
    m_lastrunTiny = config.readEntry( "LastRunTinyLeft", 0 );
    m_lastrunBig = config.readEntry( "LastRunBigLeft", 0 );
}

void RSITimer::writeConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General" );
    config.writeEntry( "LastRunTimeStamp", QDateTime::currentDateTime() );
    config.writeEntry( "LastRunTinyLeft", m_tiny_left );
    config.writeEntry( "LastRunBigLeft", m_big_left );
}

void RSITimer::restoreSession()
{
    if ( !m_lastrunDt.isNull() ) {
        int between = m_lastrunDt.secsTo( QDateTime::currentDateTime() );

        if ( between < m_intervals[BIG_MINIMIZED_INTERVAL] &&
                ( m_lastrunBig - between ) > 20 ) {
            m_big_left = m_lastrunBig - between;
        }

        if ( between < m_intervals[TINY_MINIMIZED_INTERVAL] &&
                ( m_lastrunTiny - between ) > 20 ) {
            m_tiny_left = m_lastrunTiny - between;
        }
    }
}

RSITimerNoIdle::RSITimerNoIdle( QObject *parent )
        : RSITimer( parent )
{
    qDebug() << "Starting noIdle timer";
}

RSITimerNoIdle::~RSITimerNoIdle() {}

void RSITimerNoIdle::timeout()
{
    // Just spot some long time inactivity...
    int idle = idleTime();
    if ( idle == 600 ) {
        resetAfterBigBreak();
    } else if ( idle > 600 )
        return;

    // Don't change the tray icon when suspended, or evaluate
    // a possible break.
    if ( m_suspended )
        return;

    RSIGlobals::instance()->stats()->increaseStat( TOTAL_TIME );
    RSIGlobals::instance()->stats()->increaseStat( ACTIVITY );

    int breakInterval = m_tiny_left < m_big_left ?
                        m_intervals[TINY_MAXIMIZED_INTERVAL] : m_intervals[BIG_MAXIMIZED_INTERVAL];

    if ( m_breakRequested ) {
        if ( m_tinyBreakRequested ) {
            doBreakNow( m_intervals[TINY_MAXIMIZED_INTERVAL] );
            m_pause_left = m_intervals[TINY_MAXIMIZED_INTERVAL];
            m_nextBreak = TINY_BREAK;
            RSIGlobals::instance()->NotifyBreak( true, false );
        } else if ( m_bigBreakRequested ) {
            doBreakNow( m_intervals[BIG_MAXIMIZED_INTERVAL] );
            m_pause_left = m_intervals[BIG_MAXIMIZED_INTERVAL];
            m_nextBreak = BIG_BREAK;
            RSIGlobals::instance()->NotifyBreak( true, true );
        }

        doBreakNow( breakInterval );
        m_pause_left = breakInterval;
        m_breakRequested = false;
        m_bigBreakRequested = false;
        m_tinyBreakRequested = false;
        m_relax_left = 0;
    }


    if ( m_debug > 1 ) {
        qDebug() << " pause_left: " << m_pause_left
        <<  " tiny_left: " << m_tiny_left  << " big_left: "
        <<  m_big_left << " m_nextbeak: " << m_nextBreak << endl;
    }

    if ( m_pause_left > 0 ) {
        --m_pause_left;
        if ( m_pause_left == 0 ) {
            // break is over
            emit minimize();
            emit relax( -1, false );
            if ( m_nextBreak == TINY_BREAK ) {
                resetAfterTinyBreak();
                RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
            } else if ( m_nextBreak == BIG_BREAK ) {
                resetAfterBigBreak();
                RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
            }

            m_nextBreak = NO_BREAK;
        } else {
            emit updateWidget( m_pause_left );
        }
    }

    if ( m_pause_left == 0 && m_tiny_left == 0 && m_big_left != 0 ) {
        m_pause_left = breakInterval;
        m_nextBreak = TINY_BREAK;
        doBreakNow( breakInterval );
        RSIGlobals::instance()->stats()->setStat( LAST_TINY_BREAK,
                QVariant( QDateTime::currentDateTime() ) );
        RSIGlobals::instance()->NotifyBreak( true, false );
        RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
    } else {
        --m_tiny_left;
    }

    if ( m_pause_left == 0 && m_big_left == 0 ) {
        m_pause_left = breakInterval;
        m_nextBreak = BIG_BREAK;
        doBreakNow( breakInterval );
        RSIGlobals::instance()->stats()->setStat( LAST_BIG_BREAK,
                QVariant( QDateTime::currentDateTime() ) );
        RSIGlobals::instance()->NotifyBreak( true, true );
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
    } else {
        --m_big_left;
    }

    double value = 100 - (( m_tiny_left / ( double )m_intervals[TINY_MINIMIZED_INTERVAL] ) * 100 );
    emit updateIdleAvg( value );

    emit updateToolTip( m_tiny_left, m_big_left );
}
