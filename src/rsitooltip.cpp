/*
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2006 Tom Albers <toma@kde.org>

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

#include "rsitooltip.h"
#include "rsiglobals.h"
#include "rsistats.h"

#include <KLocale>
#include <KStatusNotifierItem>

#include <QTextDocument>

RSIToolTip::RSIToolTip( QWidget *parent, KStatusNotifierItem* item )
        : QObject( parent ), m_item( item ), m_suspended( false )
{
}

RSIToolTip::~RSIToolTip() {}

static QString colorizedText( const QString& text, const QColor& color )
{
    return QString("<font color='%1'>&#9679;</font> %2")
        .arg(color.name())
        .arg(Qt::escape(text))
        ;
}

void RSIToolTip::setCounters( int tiny_left, int big_left )
{
    if ( m_suspended )
        m_item->setToolTipSubTitle( i18n( "Suspended" ) );
    else {
        QColor tinyColor = RSIGlobals::instance()->getTinyBreakColor( tiny_left );
        RSIGlobals::instance()->stats()->setColor( LAST_TINY_BREAK, tinyColor );

        QColor bigColor = RSIGlobals::instance()-> getBigBreakColor( big_left );
        RSIGlobals::instance()->stats()->setColor( LAST_BIG_BREAK, bigColor );

        // Only add the line for the tiny break when there is not
        // a big break planned at the same time.

        QStringList lines;
        if ( tiny_left != big_left ) {
            QString formattedText = RSIGlobals::instance()->formatSeconds( tiny_left );
            if ( !formattedText.isNull() ) {
                lines << colorizedText(
                    i18n( "%1 remaining until next short break", formattedText ),
                    tinyColor
                    );
            }
        }

        // do the same for the big break
        if ( big_left > 0 )
            lines << colorizedText(
                i18n( "%1 remaining until next long break",
                    RSIGlobals::instance()->formatSeconds( big_left ) ),
                bigColor
                );
        m_item->setToolTipSubTitle(lines.join("<br>"));
    }
}

void RSIToolTip::setSuspended( bool b )
{
    m_suspended = b;
}

#include "rsitooltip.moc"
