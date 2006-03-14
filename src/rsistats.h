/* This file is part of the KDE project
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#ifndef RSISTATS_H
#define RSISTATS_H

#include <qmap.h>

class RSIStats
{
public:
    RSIStats();
    ~RSIStats();

    static RSIStats *instance();

    void reset();

    enum RSIStat { TOTALTIME,
                   ACTIVITY,
                   IDLENESS,
                   TINY_BREAKS,
                   TINY_BREAKS_SKIPPED,
                   BIG_BREAKS,
                   BIG_BREAKS_SKIPPED
                 };

    void increaseStat( RSIStat stat );
    int getStat( RSIStat stat ) const;
    
    /**
      Makes a nice string given the amount of seconds.
      @param secs The amount of seconds to parse
      @return A string expressed in days, hours, minutes and
      seconds.
    */
    
    QString prettifyTimeInSeconds( int secs );
    
    // TODO: Afgeleide functies schrijven
    
private:
    QMap<QString,int> m_statistics;
};

#endif // RSISTATS_H
