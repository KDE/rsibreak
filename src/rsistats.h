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
#include <qvariant.h>

#include "rsiglobals.h"

class RSIStatItem;

class RSIStats
{
public:
    /** Default constructor. */
    RSIStats();
    /** Default destructor. */
    ~RSIStats();

    void reset();

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

    /**
      Updates the given statistic.
      @param stat The statistic you've just assigned a value to.
      @param updateDerived If true, update the derived statistics when
      calling this function.
    */
    void updateStat( RSIStat stat, bool updateDerived = true );

    QString getWhatsThisText( RSIStat stat ) const;

private:
    static RSIStats *m_instance;

    QMap<RSIStat,RSIStatItem> m_statistics;
    /** Contains formatted labels. */
    QMap<RSIStat,QLabel *> m_labels;
};

#endif // RSISTATS_H
