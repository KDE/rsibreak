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

#include <kdebug.h>
#include <klocale.h>

#include <math.h>

#include "rsiglobals.h"

QString RSIGlobals::formatSeconds( const int seconds )
{
    int mins, secs;
    QString mString, sString1, sString2;
    
    mins = (int)floor(seconds/60);
    secs = seconds-(mins*60);
    mString = i18n("One minute","%n minutes", mins);
    sString1 = i18n("One second","%n seconds", secs);
    sString2 = i18n("one second","%n seconds", secs);
    
    if (mins > 0 && secs > 0)
        return(i18n("First argument: minutes, second: seconds "
                    "both as you defined earlier",
                    "%1 and %2").arg(mString, sString2) );
    
    else if ( mins == 0 && secs > 0 )
        return( i18n("Argument: seconds part or minutes part as "
                      "defined earlier",
                      "%1").arg(sString1) );
    
    else if ( mins >0 && secs == 0 )
        return( i18n("Argument: seconds part or minutes part as "
                     "defined earlier",
                     "%1").arg(mString) );
    else // minutes = 0 and seconds = 0, remove the old text.
        return( QString::null );
}
