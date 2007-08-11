/*
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2007 Tom Albers <tomalbers@kde.nl>

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

#include "rsistatwidget.h"
#include "rsistats.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

#include <KGlobalSettings>
#include <KLocale>

RSIStatWidget::RSIStatWidget( QWidget *parent )
        : QWidget( parent )
{
    mGrid = new QGridLayout( this );
    mGrid->setSpacing( 5 );

    QGroupBox *gb = new QGroupBox( i18n( "Time" ), this );
    QGridLayout* subgrid = new QGridLayout( gb );
    addStat( TOTAL_TIME, subgrid, 0 );
    addStat( ACTIVITY, subgrid, 1 );
    addStat( IDLENESS, subgrid, 2 );
    addStat( CURRENT_IDLE_TIME, subgrid, 3 );
    addStat( MAX_IDLENESS, subgrid, 4 );
    mGrid->addWidget( gb, 0, 0 );

    gb = new QGroupBox( i18n( "Short Breaks" ), this );
    subgrid = new QGridLayout( gb );
    addStat( TINY_BREAKS, subgrid, 0 );
    addStat( LAST_TINY_BREAK, subgrid, 1 );
    addStat( TINY_BREAKS_SKIPPED, subgrid, 2 );
    addStat( IDLENESS_CAUSED_SKIP_TINY, subgrid, 3 );
    mGrid->addWidget( gb, 0, 1 );

    gb = new QGroupBox( i18n( "Pause" ), this );
    subgrid = new QGridLayout( gb );
    addStat( ACTIVITY_PERC, subgrid, 0 );
    addStat( ACTIVITY_PERC_MINUTE, subgrid, 1 );
    addStat( ACTIVITY_PERC_HOUR, subgrid, 2 );
    addStat( ACTIVITY_PERC_6HOUR, subgrid, 3 );
    addStat( PAUSE_SCORE, subgrid, 4 );
    mGrid->addWidget( gb, 1, 0 );

    gb = new QGroupBox( i18n( "Long Breaks" ), this );
    subgrid = new QGridLayout( gb );
    addStat( BIG_BREAKS, subgrid, 0 );
    addStat( LAST_BIG_BREAK, subgrid, 1 );
    addStat( BIG_BREAKS_SKIPPED, subgrid, 2 );
    addStat( IDLENESS_CAUSED_SKIP_BIG, subgrid, 3 );
    mGrid->addWidget( gb, 1, 1 );
}

RSIStatWidget::~RSIStatWidget() {}

void RSIStatWidget::addStat( RSIStat stat, QGridLayout *grid, int row )
{
    QLabel *l = RSIGlobals::instance()->stats()->getDescription( stat );
    l->setParent( grid->parentWidget() );

    QLabel *m = RSIGlobals::instance()->stats()->getLabel( stat );
    m->setParent( grid->parentWidget() );
    m->setAlignment( Qt::AlignRight );

    grid->addWidget( l, row, 0 );
    grid->addWidget( m, row, 1 );

    // measure minimal width with current font settings
    QFontMetrics fm( KGlobalSettings::generalFont() );
    int width = 0;
    switch ( stat ) {
    case TOTAL_TIME:
    case ACTIVITY:
    case IDLENESS:
    case MAX_IDLENESS:
        width = fm.width( "One one and " +
                          i18nc( "Translate this as the longest plural form. This is used to "
                                 "calculate the width of window", "minutes" ) +
                          i18nc( "Translate this as the longest plural form. This is used to "
                                 "calculate the width of window", "seconds" ) );
        break;
    case LAST_TINY_BREAK:
    case LAST_BIG_BREAK: {
        QTime dt( QTime::currentTime() );
        width = ( int )( fm.width( KGlobal::locale()->formatTime( dt, true, false ) ) * 1.25 );
        break;
    }
    default:
        ;
    }

    if ( width > 0 )
        m->setMinimumWidth( width );
}

void RSIStatWidget::showEvent( QShowEvent * )
{
    RSIGlobals::instance()->stats()->doUpdates( true );
}

void RSIStatWidget::hideEvent( QHideEvent * )
{
    RSIGlobals::instance()->stats()->doUpdates( false );
}

#include "rsistatwidget.moc"
