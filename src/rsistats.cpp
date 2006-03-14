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

#include "rsistats.h"

static RSIStats *g_instance = 0;

RSIStats::RSIStats()
{
    reset();
}

RSIStats::~RSIStats()
{
    delete g_instance; // TODO: ???
}

RSIStats *RSIStats::instance()
{
  if ( !g_instance )
  {
      g_instance = new RSIStats();
  }
  
  return g_instance;
}

void RSIStats::reset()
{
    m_statistics["total_recorded"] = 0;
    m_statistics["total_activity"] = 0;
    m_statistics["total_idleness"] = 0;
    m_statistics["total_tiny_breaks"] = 0;
    m_statistics["total_big_breaks"] = 0;
    m_statistics["tiny_breaks_skipped"] = 0;
    m_statistics["big_breaks_skipped"] = 0;
}

void RSIStats::increaseStat( RSIStat stat )
{
    switch ( stat )
    {
      case TOTALTIME : ++m_statistics["total_recorded"]; break;
      case ACTIVITY : ++m_statistics["total_activity"]; break;
      case IDLENESS : ++m_statistics["total_idleness"]; break;
      case TINY_BREAKS : ++m_statistics["total_tiny_breaks"]; break;
      case TINY_BREAKS_SKIPPED : ++m_statistics["tiny_breaks_skipped"]; break;
      case BIG_BREAKS : ++m_statistics["total_big_breaks"]; break;
      case BIG_BREAKS_SKIPPED : ++m_statistics["big_breaks_skipped"]; break;
      default: /* do nothing */ break;
    }
}

int RSIStats::getStat( RSIStat stat ) const
{
    switch ( stat )
    {
      case TOTALTIME : return m_statistics["total_recorded"];
      case ACTIVITY : return m_statistics["total_activity"];
      case IDLENESS : return m_statistics["total_idleness"];
      case TINY_BREAKS : return m_statistics["total_tiny_breaks"];
      case TINY_BREAKS_SKIPPED : return m_statistics["tiny_breaks_skipped"];
      case BIG_BREAKS : return m_statistics["total_big_breaks"];
      case BIG_BREAKS_SKIPPED : return m_statistics["big_breaks_skipped"];
      default: /* do nothing */ break;
    }
    
    return -1;
}
