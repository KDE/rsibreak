/*
   Copyright (C) 2009 Tom Albers <tomalbers@kde.nl>

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

#include "breakcontrol.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <KDebug>
#include <KHBox>
#include <KLocale>

BreakControl::BreakControl( QWidget* parent, Qt::WindowType type)
        : QWidget( parent, type )
{
    m_vbox = new QVBoxLayout;
    m_textLabel = new QLabel( this );
    m_textLabel->setAlignment( Qt::AlignHCenter );

    KHBox* hbox = new KHBox( this );
    m_skipButton = new QPushButton( i18n( "Skip" ), hbox );
    connect( m_skipButton, SIGNAL( clicked() ), SIGNAL( skip() ) );

    m_lockButton = new QPushButton( i18n( "Lock" ), hbox );
    connect( m_lockButton, SIGNAL( clicked() ), SIGNAL( lock() ) );

    m_vbox->addWidget( m_textLabel );
    m_vbox->addWidget( hbox );

    setLayout( m_vbox );

    //center it!
    const QRect r( QApplication::desktop()->screenGeometry(
                   QApplication::desktop()->primaryScreen() ) );

    const QPoint center(r.width() / 2 - sizeHint().width() / 2, r.y());
    move( center );
}

void BreakControl::setText(const QString& text)
{
    m_textLabel->setText( text );
    repaint();
}

void BreakControl::paintEvent(QPaintEvent *event)
{
    if (event->type() == QEvent::Paint) {
        int margin = 3;
        QPainterPath box;
        box.moveTo(rect().topLeft());
        box.lineTo(rect().bottomLeft());
        box.lineTo(rect().bottomRight());
        box.lineTo(rect().topRight());
        box.closeSubpath();

        QColor highlight = palette().highlight().color();
        highlight.setAlphaF(0.7);

        QPen pen( highlight );
        pen.setWidth( margin );

        QPainter painter(this);
        painter.setPen(pen);
        painter.drawPath(box);
    }
}

#include "breakcontrol.moc"
