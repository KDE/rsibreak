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

#include "rsistatitem.h"

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
