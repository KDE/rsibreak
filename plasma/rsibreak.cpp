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

#include <plasma/svg.h>
#include <plasma/theme.h>
#include <plasma/widgets/label.h>

RSIBreak::RSIBreak( QObject *parent, const QVariantList &args )
        : Plasma::Applet( parent, args ), m_text( i18n("Initialising") ),
          m_icon( "rsibreak" ), m_size( QSize( 140, 140 ) )
{
    setDrawStandardBackground( true );

    QDBusInterface rsibreak( "org.rsibreak.rsibreak", "/rsibreak",
                             "org.rsibreak.rsiwidget" );

    QTimer* m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( slotUpdate() ) );
    m_timer->start( 1000 );

    m_theme = new Plasma::Svg( "widgets/rsibreak", this );
    m_theme->setContentType( Plasma::Svg::SingleImage );

    m_h1 = new Plasma::Label( this );
    m_h1->setPen(QPen(Qt::white));
    m_h1->setText( i18n( "Currently Idle:" ) );

    m_h2 = new Plasma::Label( this );
    m_h2->setPen(QPen(Qt::white));
    m_h2->setText( i18n( "Next Short Break:" ) );

    m_h3 = new Plasma::Label( this );
    m_h3->setPen(QPen(Qt::white));
    m_h3->setText( i18n( "Next Big Break:" ) );

    m_t1 = new Plasma::Label( this );
    m_t1->setPen(QPen(Qt::white));

    m_t2 = new Plasma::Label( this );
    m_t2->setPen(QPen(Qt::white));

    m_t3 = new Plasma::Label( this );
    m_t3->setPen(QPen(Qt::white)); 
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
        m_t1->setText( i18np( "one second",
                            "%1 seconds", idle.value() ) );
        m_t2->setText( i18np( "one second",
                            "%1 seconds", active1.value() ) );
        m_t3->setText( i18np( "one second\n",
                            "%1 seconds\n", active2.value() ) );
    }

    //determine the size we need for the text.
    QFontMetrics* fm = new QFontMetrics( QFont() );
    m_heightPerLine = fm->height();
    QRect rect = fm->boundingRect( QRect(0,0,600,600), 
                                   Qt::AlignLeft | Qt::AlignTop, i18n( "Next Big Break" ) );
    m_size = rect.size();
    m_size += QSize( 25, 5*m_heightPerLine );

    //m_theme->resize( m_size );
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
    Q_UNUSED( option );

    painter->drawPixmap( boundingRect().topLeft(),
                         KIconLoader::global()->loadIcon( m_icon,
                                                          KIconLoader::Desktop ) );

    // resize the theme.
    m_theme->resize( contentsRect.width(), contentsRect.height() );
    QRect rect = QRect( contentsRect.topLeft(),
                          QSize(contentsRect.width(), m_heightPerLine) );
    QRect rect2 = QRect( 0, m_heightPerLine*2,
                         contentsRect.width(), m_heightPerLine);
    QRect rect3 = QRect( 0, m_heightPerLine*4,
                         contentsRect.width(), m_heightPerLine);

    m_theme->paint( painter, rect, "rect2951" );
    m_theme->paint( painter, rect, "path2953" );
    m_theme->paint( painter, rect2, "rect2790" );
    m_theme->paint( painter, rect2, "rect2810" );
    m_theme->paint( painter, rect3, "rect2893" );
    m_theme->paint( painter, rect3, "path2895" );
    
    // Header one
    m_h1->setGeometry( rect );
    m_h1->setPos( QPointF(0, 0) ); 
    // m_h1->paintWidget( painter, option, (QWidget*)this);
   
    // Text one 
    m_t1->setGeometry( rect );
    m_t1->setPos( QPointF(0, m_heightPerLine) ); 

    // Header two
    m_h2->setGeometry( rect );
    m_h2->setPos( QPointF(0, m_heightPerLine*2) ); 
   
    // Text two 
    m_t2->setGeometry( rect );
    m_t2->setPos( QPointF(0, m_heightPerLine*3) ); 

    // Header three 
    m_h3->setGeometry( rect );
    m_h3->setPos( QPointF(0, m_heightPerLine*4) ); 
   
    // Text three 
    m_t3->setGeometry( rect );
    m_t3->setPos( QPointF(0, m_heightPerLine*5) ); 
}

#include "rsibreak.moc"
