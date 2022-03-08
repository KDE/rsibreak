/*
    SPDX-FileCopyrightText: 2006 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIGLOBALS_H
#define RSIGLOBALS_H

#include <QBitArray>
#include <qmap.h>
#include <QObject>
#include <QStringList>

#include <kformat.h>
#include <kpassivepopup.h>

class RSIStats;

enum RSIStat {
    TOTAL_TIME = 0,
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
    TINY_BREAKS_POSTPONED,
    LAST_TINY_BREAK,
    BIG_BREAKS,
    BIG_BREAKS_SKIPPED,
    BIG_BREAKS_POSTPONED,
    LAST_BIG_BREAK,
    PAUSE_SCORE,
    STAT_COUNT
};

enum RSIInterval {
    TINY_BREAK_INTERVAL = 0,
    TINY_BREAK_DURATION,
    TINY_BREAK_THRESHOLD,
    BIG_BREAK_INTERVAL,
    BIG_BREAK_DURATION,
    BIG_BREAK_THRESHOLD,
    POSTPONE_BREAK_INTERVAL,
    PATIENCE_INTERVAL,
    SHORT_INPUT_INTERVAL,
    INTERVAL_COUNT
};

/**
 * @class RSIGlobals
 * This class consists of a few commonly used routines and values.
 * @author Tom Albers <toma.org>
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
    static RSIStats *stats() {
        return m_stats;
    }

    /**
     * Converts @p seconds to a reasonable string.
     * @param seconds the amount of seconds
     * @returns a formatted string.
     */
    QString formatSeconds( const int seconds );

    /**
     * Returns a reference to the mapping containing all intervals.
     * These intervals define when a tiny or big break should occur and for how
     * long.
     */
    const QVector<int> &intervals() const {
        return m_intervals;
    }

    /**
     * Returns true if tiny breaks are to be made at all.
     */
    bool useTinyBreaks() const {
        return m_intervals[TINY_BREAK_INTERVAL] != 0;
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
    static RSIStats *m_stats;
    QVector<int> m_intervals;
    QBitArray m_usageArray;
    KFormat m_format;
};

#endif // RSIGLOBALS_H
