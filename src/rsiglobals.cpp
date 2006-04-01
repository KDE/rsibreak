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

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <math.h>

#include "rsistats.h"
#include "rsiglobals.h"

RSIGlobals *RSIGlobals::m_instance = 0;
RSIStats *RSIGlobals::m_stats = 0;

RSIGlobals::RSIGlobals( QObject *parent, const char *name )
: QObject( parent, name )
{
    slotReadConfig();
}

RSIGlobals::~RSIGlobals()
{
    delete m_stats;
    m_stats = 0L;
}

RSIGlobals *RSIGlobals::instance()
{
    if ( !m_instance )
    {
      m_instance = new RSIGlobals();
      m_stats = new RSIStats();
    }

    return m_instance;
}

QString RSIGlobals::formatSeconds( const int seconds )
{
    int mins, secs, hours, remaining;
    QString hString, mString1, mString2, sString1, sString2;

    remaining = seconds;

    hours = (int)floor(remaining/3600);
    remaining = remaining-(hours*3600);

    mins = (int)floor(remaining/60);
    secs = remaining-(mins*60);

    hString = i18n("One hour","%n hours", hours);
    mString1 = i18n("One minute","%n minutes", mins);
    mString2 = i18n("one minute","%n minutes", mins);
    sString1 = i18n("One second","%n seconds", secs);
    sString2 = i18n("one second","%n seconds", secs);

    if ( hours > 0 and mins >0 )
        return(i18n("Arguments: hours, minutes "
                    "both as you defined earlier",
                    "%1 and %2").arg(hString, mString2) );
    else if ( hours > 0 and mins == 0 )
        return( hString );

    else if ( hours == 0 )
    {
        if (mins > 0 && secs > 0)
            return(i18n("Arguments: minutes, seconds "
                        "both as you defined earlier",
                        "%1 and %2").arg(mString1, sString2) );

        else if ( mins == 0 && secs > 0 )
            return( sString1 );

        else if ( mins >0 && secs == 0 )
            return( mString1 );

        else
            return( i18n("0 seconds") );
    }

    return( QString::null ); //should not get here
}

void RSIGlobals::slotReadConfig()
{
    kdDebug() << "RSIGlobals::slotReadConfig() entered" << endl;

    KConfig* config = kapp->config();

    config->setGroup("General Settings");

    m_intervals["tiny_minimized"] = config->readNumEntry("TinyInterval", 10)*60;
    m_intervals["tiny_maximized"] = config->readNumEntry("TinyDuration", 20);
    m_intervals["big_minimized"] = config->readNumEntry("BigInterval", 60)*60;
    m_intervals["big_maximized"] = config->readNumEntry("BigDuration", 1)*60;
    m_intervals["big_maximized"] = config->readNumEntry("BigDuration", 1)*60;

    if (config->readBoolEntry("DEBUG", false))
    {
        kdDebug() << "Debug mode activated" << endl;
        m_intervals["tiny_minimized"] = m_intervals["tiny_minimized"]/60;
        m_intervals["big_minimized"] = m_intervals["big_minimized"]/60;
        m_intervals["big_maximized"] = m_intervals["big_maximized"]/60;
    }
}

QColor RSIGlobals::getTinyBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals["tiny_minimized"];
    double v = 100 * secsToBreak / (double)minimized;

    v = v > 100 ? 100 : v;
    v = v < 0 ? 0 : v;

    return QColor ( (int)(255 - 2.55 * v), (int)(1.60 * v), 0 );
}

QColor RSIGlobals::getBigBreakColor( int secsToBreak ) const
{
    int minimized = m_intervals["tiny_minimized"];
    double v = 100 * secsToBreak / (double)minimized;

    v = v > 100 ? 100 : v;
    v = v < 0 ? 0 : v;

    return QColor ( (int)(255 - 2.55 * v), (int)(1.60 * v), 0 );
}

#include "rsiglobals.moc"
