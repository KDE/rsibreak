/*
   Copyright (C) 2008 Rafał Rzepecki <divided.mind@gmail.com>

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
  QGraphicsGridLayout *layout = new QGraphicsGridLayout(this);

  Plasma::Label *label;
  Plasma::Label *idle, *tiny, *big;
  int row = 0;

  label = new Plasma::Label(this);
  label->setText(i18nc("@label", "Idle time:"));
  layout->addItem(label, row, 0);
  idle = new Plasma::Label(this);
  layout->addItem(idle, row, 1);
  row++;

  label = new Plasma::Label(this);
  label->setText(i18nc("@label", "Left to tiny:"));
  layout->addItem(label, row, 0);
  tiny = new Plasma::Label(this);
  layout->addItem(tiny, row, 1);
  row++;

  label = new Plasma::Label(this);
  label->setText(i18nc("@label", "Left to big:"));
  layout->addItem(label, row, 0);
  big = new Plasma::Label(this);
  layout->addItem(big, row, 1);
  row++;

  Plasma::DataEngine *engine = dataEngine("rsibreak");

  engine->connectSource("idleTime", idle, 1000);
  engine->connectSource("tinyLeft", tiny, 1000);
  engine->connectSource("bigLeft", big, 1000);
}

RSIBreak::~RSIBreak()
{
}

#include "rsibreak.moc"
