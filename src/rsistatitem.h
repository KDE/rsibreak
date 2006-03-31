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

class RSIStatItem
{
  public:
    RSIStatItem( const QString &description = QString::null );
    ~RSIStatItem();

    QLabel *getDescription() const { return m_description; }
    QVariant getValue()      const { return m_value; }

    void setValue( QVariant v ) { m_value = v; }

    void addDerivedItem( RSIStat stat );
    QValueList<RSIStat> getDerivedItems() const { return m_derived; }

  private:
    QVariant m_value;
    QLabel *m_description;

    /** Contains a list of RSIStats which depend on *this* item. */
    QValueList< RSIStat > m_derived;
};

#endif
