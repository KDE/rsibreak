/*
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2006 Tom Albers <toma@kde.org>

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

#include "rsiglobals.h"

class QLabel;

class RSIStatItem;

/**
  This class records all statistics, gathered by the RSITimer.
  To add a stat, you should add an alias to the RSIStat enum, found
  in RSIGlobal. Then, add the statistic to the constructor of this class
  and to the updateLabel method. Don't forget to add a What's This text as well
  in the getWhatsThisText() method.
  If you add a statistic which is calculated from other statistics, don't
  forget to add those statistics as a dependency in the constructor of this
  class. The value of the derived statistic will be calculated in
  updateDependentStats().
  The last step involves to actually put it in the statistics widget. Use
  the addStat() method there.

  @see RSIGlobals
  @see RSIStatDialog
  @see RSITimer
*/
class RSIStats
{
public:
    /** Default constructor. */
    RSIStats();
    /** Default destructor. */
    ~RSIStats();

    /** Sets all statistics to it's initial value. */
    void reset();

    /** Increase the value of statistic @p stat with @p delta (default: 1). */
    void increaseStat( RSIStat stat, int delta = 1 );

    /**
     * Sets the value of a statistic.
     * @param stat The statistic in question.
     * @param val The value to be assigned to the statistic. In QVariant format.
     * @param ifmax If true, the value will only be assigned if the current
     * value is lower than the given @p value. Please note that derived stats
     * are updated regardless of the fact if a new value is set.
     */
    void setStat( RSIStat stat, const QVariant &val, bool ifmax = false );

    /**
     * Set the color of a given statistic.
     * @param stat The statistic in question.
     * @param color The color in QColor format.
     */
    void setColor( RSIStat stat, const QColor &color );

    /** Returns a description for the given @p stat. */
    QLabel *getDescription( RSIStat stat ) const;

    /**
     * Updates all labels to the current value of their corresponding
     * statistic.
     */
    void updateLabels();

    /** Gets the value given the @p stat.*/
    QVariant getStat( RSIStat stat ) const;

    /** Gets the value of the statistic @p stat in QLabel format. */
    QLabel *getLabel( RSIStat stat ) const;

    /**
      This function prevents RSIStats from calls to updateLabel when
      it's not really needed, e.g. when the widget is not visible.
      @param b If true, it will update the labels as soon as the stats
      update.
    */
    void doUpdates( bool b );

protected:
    /** Update the label of given @p stat to it's corresponding value. */
    void updateLabel( RSIStat stat );

    /**
     * Some statistics are calculated based on values of other statistics.
     * This function updates all statistics with @p stat as dependency.
     */
    void updateDependentStats( RSIStat stat );

    /**
     * Updates the given statistic.
     * @param stat The statistic you've just assigned a value to.
     * @param updateDerived If true, update the derived statistics when
     * calling this function.
     */
    void updateStat( RSIStat stat, bool updateDerived = true );

    /**
     * Retrieves What's This? text for a given statistic @p stat.
     */
    QString getWhatsThisText( RSIStat stat ) const;

private:
    static RSIStats *m_instance;

    bool m_doUpdates;

    QVector<RSIStatItem *> m_statistics;
    /** Contains formatted labels. */
    QVector<QLabel *> m_labels;
};

#endif // RSISTATS_H
