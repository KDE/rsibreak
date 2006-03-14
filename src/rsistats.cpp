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

#include <qgrid.h>
#include <qlabel.h>

#include <klocale.h>

#include "rsistats.h"

RSIStats *RSIStats::m_instance = 0;

RSIStats::RSIStats()
{
    reset();
}

RSIStats::~RSIStats()
{
    delete m_instance;
}

RSIStats *RSIStats::instance()
{
  if ( !m_instance )
  {
      m_instance = new RSIStats();
  }
  
  return m_instance;
}

void RSIStats::reset()
{
    /*
      Note: The order in this function implies the order
      in the statistics dialog.
    */
    
    m_statistics[TOTAL_TIME] = 0;
    m_statistics[ACTIVITY] = 0;
    m_statistics[IDLENESS] = 0;
    m_statistics[TINY_BREAKS] = 0;
    m_statistics[TINY_BREAKS_SKIPPED] = 0;
    m_statistics[BIG_BREAKS] = 0;
    m_statistics[BIG_BREAKS_SKIPPED] = 0;
}

int RSIStats::numberOfStats()
{
    return m_statistics.count();
}

void RSIStats::increaseStat( RSIStat stat )
{ 
    ++m_statistics[ stat ];
}

int RSIStats::getStat( RSIStat stat ) const
{
    return m_statistics[ stat ];
}

QString RSIStats::getDescription( RSIStat stat ) const
{
    switch ( stat )
    {
      case TOTAL_TIME : return i18n("Total recorded time");
      case ACTIVITY : return i18n("Total time of activity");
      case IDLENESS : return i18n("Total time being idle");
      case TINY_BREAKS : i18n("Total amount of tiny breaks");
      case TINY_BREAKS_SKIPPED : return i18n("Number of skipped tiny breaks");
      case BIG_BREAKS : return i18n("Total amount of big breaks");
      case BIG_BREAKS_SKIPPED : return i18n("Number of skipped big breaks");
      default: /* do nothing */ break;
    }

    return QString::null;
}

QString RSIStats::prettifySeconds( int secs )
{
    // TODO
    QString s = QString::number( secs );
    
    return s;
}

QWidget *RSIStats::widgetFactory( QWidget *parent )
{
  QGrid *w = new QGrid( 2, parent );
  w->setSpacing( 5 );
  
  QMapConstIterator<RSIStat,int> it;
  for ( it = m_statistics.begin() ; it != m_statistics.end() ; ++it )
  {
    new QLabel( getDescription( it.key() ), w );
    QLabel *l = new QLabel( prettifySeconds( it.data() ), w );
    l->setAlignment( Qt::AlignRight );
  }
  
  return w;
}
