/*
    SPDX-FileCopyrightText: 2006, 2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
    m_intervals[SHORT_INPUT_INTERVAL] = config.readEntry( "ShortInputInterval", 2 );

    if ( config.readEntry( "DEBUG", 0 ) > 0 ) {
        qDebug() << "Debug mode activated";
        m_intervals[TINY_BREAK_INTERVAL] = m_intervals[TINY_BREAK_INTERVAL] / 60;
        m_intervals[BIG_BREAK_INTERVAL] = m_intervals[BIG_BREAK_INTERVAL] / 60;
        m_intervals[BIG_BREAK_DURATION] = m_intervals[BIG_BREAK_DURATION] / 60;
        m_intervals[POSTPONE_BREAK_INTERVAL] = m_intervals[POSTPONE_BREAK_INTERVAL] / 60;
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
