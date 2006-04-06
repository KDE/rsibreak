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

#ifndef RSISTATITEM_H
#define RSISTATITEM_H

#include <qlabel.h>
#include <qvaluelist.h>
#include <qvariant.h>

#include "rsiglobals.h"

/**
 * This class represents one statistic.
 * It consists of a value, a description and a list
 * of items which have this statistic as a dependency.
 */
class RSIStatItem
{
  public:
    /**
     * Constructor. Pass a @p description along to give the
     * statistic a useful description. It will be visible in the
     * statistics widget.
     * @param description A i18n()'d text representing this statistic's meaning.
     * @param init The initial value of this statistic. Default value is an
     * integer zero.
     */
    RSIStatItem( const QString &description = QString::null, QVariant init = QVariant(0) );

    /** Default destructor. */
    ~RSIStatItem();

    /** Retrieve the item's description in QLabel format. */
    QLabel *getDescription() const { return m_description; }

    /** Retrieve the item's value in QVariant format. */
    QVariant getValue()      const { return m_value; }

    /**
     * Sets the value of this item.
     * @param v The new value of this statistic item.
     *
     * @see QVariant documentation for supported types of values.
     */
    void setValue( QVariant v ) { m_value = v; }

    /**
     * When other statistics depend on this statistic item, it should
     * be added to this list. When this statistic is updated, it will
     * iterate through the list of derived statistics and update them.
     */
    void addDerivedItem( RSIStat stat );

    /**
     * Returns the list of derived statistics.
     */
    QValueList<RSIStat> getDerivedItems() const { return m_derived; }

    /**
     * Resets current value to initial value, passed along with the
     * constructor.
     */
    void reset();

  private:
    QVariant m_value;
    QVariant m_init;
    QLabel *m_description;

    /** Contains a list of RSIStats which depend on *this* item. */
    QValueList< RSIStat > m_derived;
};

#endif
