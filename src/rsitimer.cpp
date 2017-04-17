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

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kidletime.h>
#include <ksharedconfig.h>

#include "rsiglobals.h"
#include "rsistats.h"
#include <QTimer>
#include <QThread>
#include <QDebug>

RSITimer::RSITimer( QObject *parent )
        : QThread( parent )
        , m_usePopup( true )
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

    resetAfterBreak();

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
        resetAfterBigBreak( false /*doNotify*/ );
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
    m_pause_left = t;
    m_relax_left = 0;

    RSIGlobals::instance()->NotifyBreak( true, m_nextBreak == TINY_BREAK ? false : true );
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
    m_nextnextBreak = m_nextBreak == TINY_BREAK && m_big_left <= 2 * m_tiny_left ? BIG_BREAK : TINY_BREAK;
}

void RSITimer::resetAfterTinyBreak(bool doNotify)
{
    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    qDebug() << "********** resetAfterTinyBreak !!";
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    if (doNotify) {
        RSIGlobals::instance()->NotifyBreak( false, false );
    }

    if ( m_big_left < m_tiny_left ) {
        // don't risk a big break just right after a tiny break, so delay it a bit
        m_big_left += m_tiny_left - m_big_left;
    }
}

void RSITimer::resetAfterBigBreak(bool doNotify)
{
    m_tiny_left = m_intervals[TINY_MINIMIZED_INTERVAL];
    qDebug() << "********** resetAfterBigBreak !!";
    m_big_left = m_intervals[BIG_MINIMIZED_INTERVAL];
    resetAfterBreak();
    emit updateToolTip( m_tiny_left, m_big_left );
    if (doNotify) {
        RSIGlobals::instance()->NotifyBreak( false, true );
    }
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
        resetAfterBigBreak( true /*doNotify*/ );
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS_SKIPPED );
    } else {
        resetAfterTinyBreak( true /*doNotify*/ );
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

// ----------------------------- EVENTS -----------------------//

void RSITimer::timeout()
{
    // t == 0 means activity
    const int t = idleTime();

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

    if ( m_debug > 1 ) {
        qDebug() << "idleTime" << t <<
                    "m_nextBreak" << m_nextBreak <<
                    "m_nextnextBreak" << m_nextnextBreak <<
                    "m_tiny_left" << m_tiny_left <<
                    "m_big_left" << m_big_left <<
                    "m_pause_left" << m_pause_left <<
                    "m_relax_left" << m_relax_left <<
                    "m_patience" << m_patience;
    }

    if ( t > 0 && m_pause_left > 0 ) { // means: effect is happening
        if ( m_pause_left - 1 > 0 ) { // break is not over yet
            --m_pause_left;
            updateWidget( m_pause_left );
        } else { // user survived the break, set him/her free
            emit minimize();

            // make sure we clean up stuff in the code ahead
            if ( m_nextBreak == TINY_BREAK )
                resetAfterTinyBreak( true /*doNotify*/ );
            else if ( m_nextBreak == BIG_BREAK )
                resetAfterBigBreak( true /*doNotify*/ );

            emit updateToolTip( m_tiny_left, m_big_left );
        }

        return;
    }

    const int breakInterval = m_nextBreak == TINY_BREAK ? m_intervals[TINY_MAXIMIZED_INTERVAL] : m_intervals[BIG_MAXIMIZED_INTERVAL];

    if ( t == 0 ) { // activity!
        m_patience = qMax(0, m_patience - 1);
        if ( m_patience > 0 ) {
            // If we still have patience, reset the break interval
            emit relax( breakInterval, m_nextnextBreak == BIG_BREAK );
            m_relax_left = breakInterval;
        } else if ( m_relax_left > 0 ) {
            // no patience left and still moving during a relax moment?
            // do the full break
            doBreakNow( breakInterval );
            emit relax( -1, m_nextnextBreak == BIG_BREAK );
        } else if ( m_pause_left == 0 ) {
            // there's no relax moment or break going on.

            --m_tiny_left;
            --m_big_left;

            // This is an extra safeguard.
            if ( m_tiny_left < -1 || m_big_left < -1 ) {
                if ( m_nextBreak == TINY_BREAK )
                    resetAfterTinyBreak( true /*doNotify*/ );
                else if ( m_nextBreak == BIG_BREAK )
                    resetAfterBigBreak( true /*doNotify*/ );
            }
        }

        const double value = 100 - (( m_tiny_left / ( double )m_intervals[TINY_MINIMIZED_INTERVAL] ) * 100 );
        emit updateIdleAvg( value );
    } else if ( t == m_intervals[BIG_MAXIMIZED_INTERVAL] &&
                m_intervals[TINY_MAXIMIZED_INTERVAL] <= m_intervals[BIG_MAXIMIZED_INTERVAL] &&
                /* sigh, sometime we get 2 times in row the same idle ness
                   so, to prevent the signal emitted, we also chech that the
                   big_left does not equal the regular interval */
                m_big_left != m_intervals[BIG_MINIMIZED_INTERVAL] ) {
        // the user was sufficiently idle for a big break
        bool doNotify = true;
        if ( m_relax_left == 0 && m_pause_left == 0 ) {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
            RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
            emit bigBreakSkipped();
            doNotify = false;
        }

        resetAfterBigBreak(doNotify);
    } else if ( t == m_intervals[TINY_MAXIMIZED_INTERVAL] &&
                m_tiny_left < m_big_left &&
                /* sigh, sometime we get 2 times in row the same idle ness
                   so, to prevent the signal emitted, we also chech that the
                   big_left does not equal the regular interval */
                m_tiny_left != m_intervals[TINY_MINIMIZED_INTERVAL] ) {
        // the user was sufficiently idle for a tiny break
        bool doNotify = true;
        if ( m_relax_left == 0 && m_pause_left == 0 ) {
            RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
            RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
            emit tinyBreakSkipped();
            doNotify = false;
        }

        resetAfterTinyBreak(doNotify);
    } else if ( m_relax_left > 0 ) {
        --m_relax_left;

        // just in case the user dares to become active
        m_patience = qMax(0, m_patience - 1);

        emit relax( m_relax_left, m_nextnextBreak == BIG_BREAK );
    }

    // update the stats properly when breaking
    if ( t > m_intervals[BIG_MAXIMIZED_INTERVAL] &&
            m_relax_left == 0 && m_pause_left == 0 ) {
        RSIGlobals::instance()->stats()->setStat( LAST_BIG_BREAK, QVariant( QDateTime::currentDateTime() ) );
    }

    // update the stats properly when breaking
    if ( t > m_intervals[TINY_MAXIMIZED_INTERVAL] &&
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

        if ( m_usePopup ) {
            m_patience = m_intervals[PATIENCE_INTERVAL];

            emit relax( breakInterval, m_nextnextBreak == BIG_BREAK );
            m_relax_left = breakInterval;
        } else {
            doBreakNow( breakInterval );
            emit relax( -1, m_nextnextBreak == BIG_BREAK );
        }
    }

    emit updateToolTip( m_tiny_left, m_big_left );
}

//--------------------------- CONFIG ----------------------------//

void RSITimer::readConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General" );
    m_lastrunDt = config.readEntry( "LastRunTimeStamp", QDateTime() );
    m_lastrunTiny = config.readEntry( "LastRunTinyLeft", 0 );
    m_lastrunBig = config.readEntry( "LastRunBigLeft", 0 );

    config = KSharedConfig::openConfig()->group( "Popup Settings" );
    m_usePopup = config.readEntry( "UsePopup", true );
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
    // Don't change the tray icon when suspended, or evaluate
    // a possible break.
    if ( m_suspended )
        return;

    // Just spot some long time inactivity...
    const int idle = idleTime();
    if ( idle == 600 ) {
        resetAfterBigBreak( false /*doNotify*/ );
    } else if ( idle > 600 )
        return;

    RSIGlobals::instance()->stats()->increaseStat( TOTAL_TIME );
    RSIGlobals::instance()->stats()->increaseStat( ACTIVITY );

    if ( m_debug > 1 ) {
        qDebug() << " pause_left: " << m_pause_left
        <<  " tiny_left: " << m_tiny_left  << " big_left: "
        <<  m_big_left << " nextBreak: " << m_nextBreak;
    }

    if ( m_pause_left > 0 ) {
        --m_pause_left;
        if ( m_pause_left == 0 ) {
            // break is over
            emit minimize();
            emit relax( -1, false );
            if ( m_nextBreak == TINY_BREAK ) {
                resetAfterTinyBreak( false /*doNotify*/ );
                RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_TINY );
            } else if ( m_nextBreak == BIG_BREAK ) {
                resetAfterBigBreak( false /*doNotify*/ );
                RSIGlobals::instance()->stats()->increaseStat( IDLENESS_CAUSED_SKIP_BIG );
            }
        } else {
            emit updateWidget( m_pause_left );
        }
    }

    if ( m_pause_left == 0 && m_tiny_left == 0 && m_big_left != 0 ) {
        m_nextBreak = TINY_BREAK;
        doBreakNow( m_intervals[TINY_MAXIMIZED_INTERVAL] );
        RSIGlobals::instance()->stats()->setStat( LAST_TINY_BREAK, QVariant( QDateTime::currentDateTime() ) );
        RSIGlobals::instance()->stats()->increaseStat( TINY_BREAKS );
    } else {
        --m_tiny_left;
    }

    if ( m_pause_left == 0 && m_big_left == 0 ) {
        m_nextBreak = BIG_BREAK;
        doBreakNow( m_intervals[BIG_MAXIMIZED_INTERVAL] );
        RSIGlobals::instance()->stats()->setStat( LAST_BIG_BREAK, QVariant( QDateTime::currentDateTime() ) );
        RSIGlobals::instance()->stats()->increaseStat( BIG_BREAKS );
    } else {
        --m_big_left;
    }

    const double value = 100 - (( m_tiny_left / ( double )m_intervals[TINY_MINIMIZED_INTERVAL] ) * 100 );
    emit updateIdleAvg( value );

    emit updateToolTip( m_tiny_left, m_big_left );
}
