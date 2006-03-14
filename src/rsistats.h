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

class QWidget;

class RSIStats
{
public:
    RSIStats();
    ~RSIStats();

    static RSIStats *instance();

    void reset();
    int numberOfStats();

    enum RSIStat { TOTAL_TIME,
                   ACTIVITY,
                   IDLENESS,
                   TINY_BREAKS,
                   TINY_BREAKS_SKIPPED,
                   BIG_BREAKS,
                   BIG_BREAKS_SKIPPED
                 };

    /** Increase the value of statistic @p stat with 1. */
    void increaseStat( RSIStat stat );
    
    /** Gets the value given the @p stat. */
    int getStat( RSIStat stat ) const;
    
    /**
      Makes a nice string given the amount of seconds.
      @param secs The amount of seconds to parse
      @return A string expressed in days, hours, minutes and
      seconds.
    */
    QString prettifySeconds( int secs );
    
    /** Returns a description for the given @p stat. */
    QString getDescription( RSIStat stat ) const;
    
    QWidget *widgetFactory( QWidget *parent = 0 );
    
    // TODO: Afgeleide functies schrijven
    
private:
    static RSIStats *m_instance;
    QMap<RSIStat,int> m_statistics;
};

#endif // RSISTATS_H
