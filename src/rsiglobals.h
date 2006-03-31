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

#ifndef RSIGLOBALS_H
#define RSIGLOBALS_H

#include <qdatetime.h>
#include <qimage.h>

#include <kpassivepopup.h>

class RSIStats;

enum RSIStat {
               TOTAL_TIME,
               ACTIVITY,
               IDLENESS,
               ACTIVITY_PERC,
               MAX_IDLENESS,
               CURRENT_IDLE_TIME,
               IDLENESS_CAUSED_SKIP_TINY,
               IDLENESS_CAUSED_SKIP_BIG,
               TINY_BREAKS,
               TINY_BREAKS_SKIPPED,
               LAST_TINY_BREAK,
               LAST_TINY_BREAK_COLOR, /* internal */
               BIG_BREAKS,
               BIG_BREAKS_SKIPPED,
               LAST_BIG_BREAK,
               LAST_BIG_BREAK_COLOR, /* internal */
               PAUSE_SCORE
             };

/**
 * @class RSIGlobals
 * This class consists of a few commonly used routines and values.
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIGlobals : public QObject
{
  Q_OBJECT
  public:
    RSIGlobals( QObject *parent = 0, const char *name = 0 );
    ~RSIGlobals();

    static RSIGlobals *instance();

    RSIStats *stats() { return m_stats; }

    /**
     * Converts @p seconds to a reasonable string.
     * @param seconds the amount of seconds
     * @returns a formatted string. Examples:
     * - 3601: "One hour"
     * - 3665: "One hour and one minute"
     * - 60: "One minute"
     * - 61: "One minute and one second"
     * - 125: "2 minutes and 5 seconds"
     * - 2: "2 seconds"
     * - 0: "0 seconds"
     */
    QString formatSeconds(const int seconds);

    QMap<QString,int> &intervals() { return m_intervals; }

    QColor getTinyBreakColor() const;
    QColor getBigBreakColor() const;

  public slots:
    void slotReadConfig();

  private:
    static RSIGlobals *m_instance;
    QMap<QString,int> m_intervals;
    static RSIStats *m_stats;
};

#endif // RSIGLOBALS_H
