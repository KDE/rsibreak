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

#include <QBitArray>
#include <qmap.h>
#include <QObject>
#include <QStringList>

#include <kpassivepopup.h>

class RSIStats;

enum RSIStat {
    TOTAL_TIME,
    ACTIVITY,
    IDLENESS,
    ACTIVITY_PERC,
    ACTIVITY_PERC_MINUTE,
    ACTIVITY_PERC_HOUR,
    ACTIVITY_PERC_6HOUR,
    MAX_IDLENESS,
    CURRENT_IDLE_TIME,
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

/**
 * @class RSIGlobals
 * This class consists of a few commonly used routines and values.
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIGlobals : public QObject
{
    Q_OBJECT
public:
    /** Default constructor. */
    explicit RSIGlobals( QObject *parent = 0 );

    /** Default destructor. */
    ~RSIGlobals();

    /**
     * Returns an instance of RSIGlobals. Never create your own instance
     * of RSIGlobals, but use this method instead to get the one and only
     * instance.
     */
    static RSIGlobals *instance();

    /**
     * Returns the one and only statistics component.
     *
     * @see RSIStats
     */
    RSIStats *stats() {
        return m_stats;
    }

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
    QString formatSeconds( const int seconds );

    /**
     * Returns a reference to the mapping containing all intervals.
     * These intervals define when a tiny or big break should occur and for how
     * long.
     */
    QMap<QString, int> &intervals() {
        return m_intervals;
    }

    /**
     * This function returns a color ranging from green to red.
     * The more red, the more the user needs a tiny break.
     */
    QColor getTinyBreakColor( int secsToBreak ) const;

    /**
     * This function returns a color ranging from green to red.
     * The more red, the more the user needs a tiny break.
     */
    QColor getBigBreakColor( int secsToBreak ) const;

    /**
     * Returns the array which keeps track per second for 24 hours when the
     * user was active or idle. Activity = 1, idle = 0.
     * The RSIStatBitArrayItem can read and write a certain interval of this
     * array, for example to measure the activity in 60 seconds or 1 hour.
     *
     * @see RSIStatBitArrayItem
     */
    QBitArray *usageArray() {
        return &m_usageArray;
    }

    /**
     * Resets the usage array, with all values to 0.
     */
    void resetUsage();

    /**
     *
     * Hook to KDE's Notifying system at start/end of a break.
     * @param start when true the start commands are executed, false executes
     *              the ones at the end of a break.
     * @param big   true for big breaks, false for short ones.
     */
    void NotifyBreak( bool start, bool big );

public slots:
    /**
     * Reads the configuration.
     */
    void slotReadConfig();

private:
    static RSIGlobals *m_instance;
    QMap<QString, int> m_intervals;
    static RSIStats *m_stats;
    QBitArray m_usageArray;
    QStringList m_dcopstartbig;
    QStringList m_dcopendbig;
    QStringList m_dcopstarttiny;
    QStringList m_dcopendtiny;

};

#endif // RSIGLOBALS_H
