/*
   Copyright (C) 2006,2010 Tom Albers <toma@kde.org>
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

#include "rsiglobals.h"

#include <qdebug.h>
#include <kconfig.h>
#include <klocalizedstring.h>
#include <knotification.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <math.h>

#include "rsistats.h"

RSIGlobals *RSIGlobals::m_instance = 0;
RSIStats *RSIGlobals::m_stats = 0;

RSIGlobals::RSIGlobals( QObject *parent )
        : QObject( parent )
{
    resetUsage();
    slotReadConfig();
}

RSIGlobals::~RSIGlobals()
{
    delete m_stats;
    m_stats = 0L;
}

RSIGlobals *RSIGlobals::instance()
{
    if ( !m_instance ) {
        m_instance = new RSIGlobals();
        m_stats = new RSIStats();
    }

    return m_instance;
}

QString RSIGlobals::formatSeconds( const int seconds )
{
    return m_format.formatSpelloutDuration( seconds * 1000 );
}

void RSIGlobals::slotReadConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );

    m_intervals.resize(INTERVAL_COUNT);
    m_intervals[TINY_BREAK_INTERVAL] = config.readEntry( "TinyInterval", 10 ) * 60;
    m_intervals[TINY_BREAK_DURATION] = config.readEntry( "TinyDuration", 20 );
    m_intervals[TINY_BREAK_THRESHOLD] = config.readEntry( "TinyThreshold", 20 );
    m_intervals[BIG_BREAK_INTERVAL] = config.readEntry( "BigInterval", 60 ) * 60;
    m_intervals[BIG_BREAK_DURATION] = config.readEntry( "BigDuration", 1 ) * 60;
    m_intervals[BIG_BREAK_THRESHOLD] = config.readEntry( "BigThreshold", 1 ) * 60;
    m_intervals[POSTPONE_BREAK_INTERVAL] = config.readEntry( "PostponeBreakDuration", 5 ) * 60;
    m_intervals[PATIENCE_INTERVAL] = config.readEntry( "Patience", 30 );

    if ( config.readEntry( "DEBUG", 0 ) > 0 ) {
        qDebug() << "Debug mode activated";
        m_intervals[TINY_BREAK_INTERVAL] = m_intervals[TINY_BREAK_INTERVAL] / 60;
        m_intervals[BIG_BREAK_INTERVAL] = m_intervals[BIG_BREAK_INTERVAL] / 60;
        m_intervals[BIG_BREAK_DURATION] = m_intervals[BIG_BREAK_DURATION] / 60;
        m_intervals[POSTPONE_BREAK_INTERVAL] = m_intervals[POSTPONE_BREAK_INTERVAL] / 60;
    }

    // if big_maximized < tiny_maximized, the bigbreaks will not get reset,
    // guard against that situation.
    if ( m_intervals[BIG_BREAK_DURATION] < m_intervals[TINY_BREAK_DURATION] ) {
        qDebug() << "max big > max tiny, not allowed & corrected";
        m_intervals[BIG_BREAK_DURATION] = m_intervals[TINY_BREAK_DURATION];
    }

}

QColor RSIGlobals::getTinyBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals[TINY_BREAK_INTERVAL];
    double v = 100 * secsToBreak / ( double )minimized;

    v = v > 100 ? 100 : v;
    v = v < 0 ? 0 : v;

    return QColor(( int )( 255 - 2.55 * v ), ( int )( 1.60 * v ), 0 );
}

QColor RSIGlobals::getBigBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals[BIG_BREAK_INTERVAL];
    double v = 100 * secsToBreak / ( double )minimized;

    v = v > 100 ? 100 : v;
    v = v < 0 ? 0 : v;

    return QColor(( int )( 255 - 2.55 * v ), ( int )( 1.60 * v ), 0 );
}

void RSIGlobals::resetUsage()
{
    m_usageArray.fill( false, 60 * 60 * 24 );
}

void RSIGlobals::NotifyBreak( bool start, bool big )
{
    if ( start )
        big ? KNotification::event( "start long break",
                                    i18n( "Start of a long break" ) )
        : KNotification::event( "start short break",
                                i18n( "Start of a short break" ) );
    else
        big ? KNotification::event( "end long break",
                                    i18n( "End of a long break" ) )
        : KNotification::event( "end short break",
                                i18n( "End of a short break" ) );
}
