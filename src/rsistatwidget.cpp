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
#include <qdatetime.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include "rsistats.h"
#include "rsistatwidget.h"

RSIStatWidget::RSIStatWidget( QWidget *parent, const char *name )
: QWidget( parent, name )
{
    mGrid = new QGridLayout( this );
    mGrid->setSpacing( 5 );

    QGroupBox *gb = new QGroupBox( 1, Qt::Vertical, i18n("Time"), this );
    QGrid *subgrid = new QGrid( 2, Qt::Horizontal, gb );
    subgrid->setSpacing( 5 );
    addStat( TOTAL_TIME, subgrid );
    addStat( ACTIVITY, subgrid );
    addStat( IDLENESS, subgrid );
    addStat( CURRENT_IDLE_TIME, subgrid );
    addStat( MAX_IDLENESS, subgrid );
    mGrid->addWidget( gb, 0, 0 );

    gb = new QGroupBox( 1, Qt::Vertical, i18n("Short breaks"), this );
    subgrid = new QGrid( 2, Qt::Horizontal, gb );
    subgrid->setSpacing( 5 );
    addStat( TINY_BREAKS, subgrid );
    addStat( LAST_TINY_BREAK, subgrid );
    addStat( TINY_BREAKS_SKIPPED, subgrid );
    addStat( IDLENESS_CAUSED_SKIP_TINY, subgrid );
    mGrid->addWidget( gb, 0, 1 );

    gb = new QGroupBox( 1, Qt::Vertical, i18n("Pause"), this );
    subgrid = new QGrid( 2, Qt::Horizontal, gb );
    subgrid->setSpacing( 5 );
    addStat( ACTIVITY_PERC, subgrid );
    addStat( ACTIVITY_PERC_MINUTE, subgrid );
    addStat( ACTIVITY_PERC_HOUR, subgrid );
    addStat( ACTIVITY_PERC_6HOUR, subgrid );
    addStat( PAUSE_SCORE, subgrid );
    mGrid->addWidget( gb, 1, 0 );

    gb = new QGroupBox( 1, Qt::Vertical, i18n("Long breaks"), this );
    subgrid = new QGrid( 2, Qt::Horizontal, gb );
    subgrid->setSpacing( 5 );
    addStat( BIG_BREAKS, subgrid );
    addStat( LAST_BIG_BREAK, subgrid );
    addStat( BIG_BREAKS_SKIPPED, subgrid );
    addStat( IDLENESS_CAUSED_SKIP_BIG, subgrid );
    mGrid->addWidget( gb, 1, 1 );
}

RSIStatWidget::~RSIStatWidget()
{
}

void RSIStatWidget::addStat( RSIStat stat, QGrid *grid )
{
    QLabel *l = RSIGlobals::instance()->stats()->getDescription( stat );
    l->reparent( grid, 0, QPoint() );

    QLabel *m = RSIGlobals::instance()->stats()->getLabel( stat );
    m->reparent( grid, 0, QPoint() );
    m->setAlignment( Qt::AlignRight );

    // measure minimal width with current font settings
    QFontMetrics fm( KGlobalSettings::generalFont() );
    int width = 0;
    switch ( stat )
    {
      case TOTAL_TIME:
      case ACTIVITY:
      case IDLENESS:
      case MAX_IDLENESS:
        width = fm.width( "One one and " +
            i18n("Translate this as the longest plural form. This is used to "
                 "calculate the width of window","minutes") +
            i18n("Translate this as the longest plural form. This is used to "
                 "calculate the width of window","seconds") );
        break;
      case LAST_TINY_BREAK:
      case LAST_BIG_BREAK:
      {
        QTime dt( QTime::currentTime() );
        width = (int)(fm.width( KGlobal::locale()->formatTime( dt ) ) * 1.25);
        break;
      }
      default: ;
    }

    if ( width > 0 )
      m->setMinimumWidth( width );
}

#include "rsistatwidget.moc"
