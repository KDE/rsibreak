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

#include <kdebug.h>
#include <klocale.h>
#include <qtimer.h>

#include "rsistatwidget.h"

RSIStatWidget::RSIStatWidget( QWidget *parent, const char *name )
: QWidget( parent, name )
{
    mGrid = new QGridLayout( this );
    mGrid->setSpacing( 5 );

    addStat( TOTAL_TIME, 0 );
    addStat( ACTIVITY, 1 );
    addStat( IDLENESS, 2 );
    addStat( MAX_IDLENESS, 3 );
    addStat( TINY_BREAKS, 4 );
    addStat( LAST_TINY_BREAK, 5 );
    addStat( TINY_BREAKS_SKIPPED, 6 );
    addStat( IDLENESS_CAUSED_SKIP_TINY, 7 );
    addStat( BIG_BREAKS, 8 );
    addStat( LAST_BIG_BREAK, 9 );
    addStat( BIG_BREAKS_SKIPPED, 10 );
    addStat( IDLENESS_CAUSED_SKIP_BIG, 11 );
    addStat( PAUSE_SCORE, 12 );
}

RSIStatWidget::~RSIStatWidget()
{
}

void RSIStatWidget::addStat( RSIStat stat, int row )
{
    QLabel *l = new QLabel( RSIStats::instance()->getDescription( stat ), this );
    mGrid->addWidget( l, row, 0 );
    l = RSIStats::instance()->getLabel( stat );
    l->reparent( this, 0, QPoint() );
    l->setAlignment( Qt::AlignRight );
    mGrid->addWidget( l, row, 1 );
}

#include "rsistatwidget.moc"
