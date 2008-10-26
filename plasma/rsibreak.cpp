/*
   Copyright (C) 2008 Rafał Rzepecki <divided.mind@gmail.com>
   Copyright (C) 2008 Omat Holding B.V. <info@omat.nl>

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

#include <Plasma/DataEngine>
#include <Plasma/Label>

#include <QtGui/QGraphicsGridLayout>

RSIBreak::RSIBreak( QObject *parent, const QVariantList &args )
        : Plasma::Applet( parent, args )
{
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
}

void RSIBreak::init()
{
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();

    Plasma::Label *label;
    Plasma::Label *idle, *tiny, *big;

    label = new Plasma::Label( this );
    label->setText( i18nc( "@label", "Idle time:" ) );
    layout->addItem( label, 0, 0 );
    idle = new Plasma::Label( this );
    layout->addItem( idle, 0, 1 );

    label = new Plasma::Label( this );
    label->setText( i18nc( "@label", "Left to tiny:" ) );
    layout->addItem( label, 1, 0 );
    tiny = new Plasma::Label( this );
    layout->addItem( tiny, 1, 1 );

    label = new Plasma::Label( this );
    label->setText( i18nc( "@label", "Left to big:" ) );
    layout->addItem( label, 2, 0 );
    big = new Plasma::Label( this );
    layout->addItem( big, 2, 1 );

    setLayout( layout );

    resize(300,100);
    updateGeometry();

    Plasma::DataEngine *engine = dataEngine( "rsibreak" );
    engine->connectSource( "idleTime", idle, 1000 );
    engine->connectSource( "tinyLeft", tiny, 1000 );
    engine->connectSource( "bigLeft", big, 1000 );
}

RSIBreak::~RSIBreak()
{
}

#include "rsibreak.moc"
