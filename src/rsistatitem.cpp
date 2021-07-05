/*
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2006 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsistatitem.h"

#include <QLabel>

const int totalarraysize = 60 * 60 * 24;

RSIStatItem::RSIStatItem( const QString &description, const QVariant &init )
{
    m_description = new QLabel( description, 0 );
    m_value = init;
    m_init = init;
}

RSIStatItem::~RSIStatItem() {}

void RSIStatItem::addDerivedItem( RSIStat stat )
{
    m_derived += stat;
}

void RSIStatItem::reset()
{
    m_value = m_init;
}


RSIStatBitArrayItem::RSIStatBitArrayItem( const QString &description, const QVariant &init, int size )
        : RSIStatItem( description, init ), m_size( size ), m_counter( 0 )
{
    Q_ASSERT( size <= totalarraysize );

    m_end = 0;
    m_begin = totalarraysize - size;
}

RSIStatBitArrayItem::~RSIStatBitArrayItem() {}

void RSIStatBitArrayItem::reset()
{
    RSIStatItem::reset();
    RSIGlobals::instance()->resetUsage();

    m_end = 0;
    m_begin = totalarraysize - m_size;
    m_counter = 0;
}

void RSIStatBitArrayItem::setActivity()
{
    QBitArray *array = RSIGlobals::instance()->usageArray();

    if ( !array->testBit( m_begin ) )
        ++m_counter;

    array->setBit( m_end );

    Q_ASSERT( m_counter <= m_size );

    m_value = QVariant( 100.0 * ( double )( m_counter ) / ( double )( m_size ) );
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

    m_value = QVariant( 100.0 * ( double )( m_counter ) / ( double )( m_size ) );
    m_begin = ( m_begin + 1 ) % totalarraysize;
    m_end = ( m_end + 1 ) % totalarraysize;
}
