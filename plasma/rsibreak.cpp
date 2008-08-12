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

#include <Plasma/Label>
#include <QtGui/QGraphicsLinearLayout>

RSIBreak::RSIBreak( QObject *parent, const QVariantList &args )
        : Plasma::Applet( parent, args )
{
  QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(this);
  Plasma::Label *label = new Plasma::Label(this);
  label->setText("just testing");
  layout->addItem(label);
}

RSIBreak::~RSIBreak()
{
}

#include "rsibreak.moc"
