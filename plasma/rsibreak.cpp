/*
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

#include "rsibreak.h"

#include <KIconLoader>

#include <QPainter>
#include <QTimer>
#include <QFontMetrics>
#include <QDBusInterface>
#include <QDBusReply>

RSIBreak::RSIBreak( QObject *parent, const QStringList &args )
        : Plasma::Applet( parent, args ), m_icon( "rsibreak" ),
          m_size( QSize( 140, 140 ) ), m_text( i18n("Initialising") )
{
    setDrawStandardBackground( true );

    QDBusInterface rsibreak( "org.rsibreak.rsibreak", "/rsibreak",
                             "org.rsibreak.rsiwidget" );

    QTimer* m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( slotUpdate() ) );
    m_timer->start( 1000 );
}

RSIBreak::~RSIBreak()
{
}

void RSIBreak::slotUpdate()
{
    QDBusInterface rsibreak( "org.rsibreak.rsibreak", "/rsibreak",
                             "org.rsibreak.rsiwidget" );

    if ( !rsibreak.isValid() ) {
        m_text = i18n( "Please\nlaunch\nRSIBreak" );
    }  else {
        QDBusReply<int> idle = rsibreak.call( "idleTime" );
        QDBusReply<int> active1 = rsibreak.call( "tinyLeft" );
        QDBusReply<int> active2 = rsibreak.call( "bigLeft" );
        QDBusReply<QString> icon = rsibreak.call( "currentIcon" );

        m_icon = icon.value();
        m_text = i18np( "Currently Idle:\none second\n",
                            "Currently Idle:\n%1 seconds\n", idle.value() );
        m_text.append( i18np( "Next short break:\none second\n",
                            "Next short break:\n%1 seconds\n", active1.value() ) );
        m_text.append( i18np( "Next big break:\none second\n",
                            "Next big break:\n%1 seconds\n", active2.value() ) );
    }

    //determine the size we need for the text.
    QFontMetrics* fm = new QFontMetrics( QFont() );
    QRect rect = fm->boundingRect( QRect(0,0,600,600), 
                                   Qt::AlignLeft | Qt::AlignTop, m_text );
    m_size = rect.size();
    m_size += QSize( 25, 10 );
    delete fm;

    update();
}

QSizeF RSIBreak::contentSize() const
{
    return m_size;
}

void RSIBreak::paintInterface( QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               const QRect &contentsRect )
{

    painter->save();
    painter->setPen( Qt::white );
    painter->drawPixmap( boundingRect().topLeft(),
                         KIconLoader::global()->loadIcon( m_icon, 
                                                          K3Icon::Desktop ) );
    painter->drawText( QRect( 10, 10, m_size.width()-10, m_size.height()-10 ),
                       Qt::AlignBottom + Qt::AlignHCenter,
                       m_text );
    painter->drawText( boundingRect(),
                       Qt::AlignBottom + Qt::AlignHCenter,
                       "RSIBreak Info" );
    painter->restore();
}

#include "rsibreak.moc"
