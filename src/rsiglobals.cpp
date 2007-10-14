/* This file is part of the KDE project
   Copyright (C) 2006 Tom Albers <tomalbers@kde.nl>

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

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <knotification.h>
#include <kconfiggroup.h>

#include <math.h>
#include <kglobal.h>

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
    int mins, secs, hours, remaining;
    QString hString, mString, sString;

    remaining = seconds;

    hours = ( int )floor( remaining / 3600 );
    remaining = remaining - ( hours * 3600 );

    mins = ( int )floor( remaining / 60 );
    secs = remaining - ( mins * 60 );

    hString = i18np( "1 hour", "%1 hours", hours );
    mString = i18np( "1 minute", "%1 minutes", mins );
    sString = i18np( "1 second", "%1 seconds", secs );

    if ( hours > 0 && mins > 0 )
        return( i18nc( "Arguments: hours, minutes "
                       "both as you defined earlier",
                       "%1 and %2", hString, mString ) );
    else if ( hours > 0 && mins == 0 )
        return( hString );

    else if ( hours == 0 ) {
        if ( mins > 0 && secs > 0 )
            return( i18nc( "Arguments: minutes, seconds "
                           "both as you defined earlier",
                           "%1 and %2", mString, sString ) );

        else if ( mins == 0 && secs > 0 )
            return( sString );

        else if ( mins > 0 && secs == 0 )
            return( mString );

        else
            return( i18n( "0 seconds" ) );
    }

    return QString(); //should not get here
}

void RSIGlobals::slotReadConfig()
{
    KConfigGroup config = KGlobal::config()->group( "General Settings" );

    m_intervals["tiny_minimized"] = config.readEntry( "TinyInterval", 10 ) * 60;
    m_intervals["tiny_maximized"] = config.readEntry( "TinyDuration", 20 );
    m_intervals["big_minimized"] = config.readEntry( "BigInterval", 60 ) * 60;
    m_intervals["big_maximized"] = config.readEntry( "BigDuration", 1 ) * 60;
    m_intervals["big_maximized"] = config.readEntry( "BigDuration", 1 ) * 60;

    if ( config.readEntry( "DEBUG", false ) ) {
        kDebug() << "Debug mode activated";
        m_intervals["tiny_minimized"] = m_intervals["tiny_minimized"] / 60;
        m_intervals["big_minimized"] = m_intervals["big_minimized"] / 60;
        m_intervals["big_maximized"] = m_intervals["big_maximized"] / 60;
    }

    // if big_maximized < tiny_maximized, the bigbreaks will not get reset,
    // guard against that situation.
    if ( m_intervals["big_maximized"] < m_intervals["tiny_maximized"] ) {
        kDebug() << "max big > max tiny, not allowed & corrected";
        m_intervals["big_maximized"] = m_intervals["tiny_maximized"];
    }

}

QColor RSIGlobals::getTinyBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals["tiny_minimized"];
    double v = 100 * secsToBreak / ( double )minimized;

    v = v > 100 ? 100 : v;
    v = v < 0 ? 0 : v;

    return QColor(( int )( 255 - 2.55 * v ), ( int )( 1.60 * v ), 0 );
}

QColor RSIGlobals::getBigBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals["big_minimized"];
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

#include "rsiglobals.moc"
