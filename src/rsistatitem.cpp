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

#include <kdebug.h>

#include "rsistatitem.h"

const int totalarraysize = 60 * 60 * 24;

RSIStatItem::RSIStatItem( const QString &description, QVariant init )
{
    m_description = new QLabel( description, 0 );
    m_value = init;
    m_init = init;
}

RSIStatItem::~RSIStatItem()
{
}

void RSIStatItem::addDerivedItem( RSIStat stat )
{
    m_derived += stat;
}

void RSIStatItem::reset()
{
    m_value = m_init;
}


RSIStatBitArrayItem::RSIStatBitArrayItem( const QString &description, QVariant init, int size )
: RSIStatItem( description, init ), m_size( size ), m_counter( 0 )
{
  Q_ASSERT ( size <= totalarraysize );

  m_end = 0;
  m_begin = totalarraysize - size;
}

RSIStatBitArrayItem::~RSIStatBitArrayItem()
{
}

void RSIStatBitArrayItem::reset()
{
  RSIStatItem::reset();
  RSIGlobals::instance()->resetUsage();

  m_end = 0;
  m_begin = totalarraysize - m_size;
}

void RSIStatBitArrayItem::setActivity()
{
  QBitArray *array = RSIGlobals::instance()->usageArray();

  if ( !array->testBit( m_begin ) )
    ++m_counter;

  array->setBit( m_end );

  Q_ASSERT( m_counter <= m_size );

  m_value = QVariant( 100.0 * (double)(m_counter) / (double)(m_size) );
  m_begin = ( m_begin + 1 ) % totalarraysize;
  m_end = ( m_end + 1 ) % totalarraysize;
}

void RSIStatBitArrayItem::setIdle()
{
  QBitArray *array = RSIGlobals::instance()->usageArray();

  if ( array->testBit( m_begin ) )
    m_counter > 0 ? --m_counter : m_counter;

  array->clearBit( m_end );

  Q_ASSERT( m_counter <= m_size );

  m_value = QVariant( 100.0 * (double)(m_counter) / (double)(m_size) );
  m_begin = ( m_begin + 1 ) % totalarraysize;
  m_end = ( m_end + 1 ) % totalarraysize;
}
