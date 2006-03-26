/* This file is part of the KDE project
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
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

#ifndef RSISTATS_H
#define RSISTATS_H

#include <qlabel.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qvariant.h>

enum RSIStat {
               TOTAL_TIME,
               ACTIVITY,
               IDLENESS,
               ACTIVITY_PERC,
               MAX_IDLENESS,
               IDLENESS_CAUSED_SKIP_TINY,
               IDLENESS_CAUSED_SKIP_BIG,
               TINY_BREAKS,
               TINY_BREAKS_SKIPPED,
               LAST_TINY_BREAK,
               BIG_BREAKS,
               BIG_BREAKS_SKIPPED,
               LAST_BIG_BREAK,
               PAUSE_SCORE
             };

class RSIStatItem;

class RSIStats
{
public:
    RSIStats();
    ~RSIStats();

    static RSIStats *instance();

    void reset();
    // int numberOfStats() const;

    /** Increase the value of statistic @p stat with @p delta (default: 1). */
    void increaseStat( RSIStat stat, int delta = 1 );
    void setStat( RSIStat stat, QVariant val, bool ifmax = false );

    /** Returns a description for the given @p stat. */
    QLabel *getDescription( RSIStat stat ) const;

    /** TODO Doxy */
    void updateLabels();

    /** Gets the value given the @p stat.  */
    QVariant getStat( RSIStat stat ) const;
    QLabel *getLabel( RSIStat stat ) const;

protected:
    /** TODO Doxy */
    void updateLabel( RSIStat );

    void updateDependentStats( RSIStat );

    void updateStat( RSIStat );

private:
    static RSIStats *m_instance;

    QMap<RSIStat,RSIStatItem> m_statistics;
    /** Contains formatted labels. */
    QMap<RSIStat,QLabel *> m_labels;
};

#endif // RSISTATS_H
