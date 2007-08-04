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
#include <QPainter>
#include <QTimer>
#include <QFontMetrics>
#include <QDBusInterface>
#include <QDBusReply>

RSIBreak::RSIBreak(QObject *parent, const QStringList &args)
  : Plasma::Applet(parent, args)
{
    setDrawStandardBackground(true);

    QDBusInterface rsibreak("org.rsibreak.rsibreak","/rsibreak",
               "org.rsibreak.rsiwidget");

   QTimer* m_timer = new QTimer(this);
   connect( m_timer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
   m_timer->start(1000);
}

RSIBreak::~RSIBreak()
{
  if (failedToLaunch()) {
        // Do some cleanup here
  } else {
        // Save settings
  }
}

void RSIBreak::slotUpdate()
{
  update();
}

QSizeF RSIBreak::contentSize() const
{
  return QSizeF( 140, 140 );
}

void RSIBreak::paintInterface( QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          const QRect &contentsRect)
{
  QString text;
  QDBusInterface rsibreak("org.rsibreak.rsibreak","/rsibreak",
               "org.rsibreak.rsiwidget");
  
  if (!rsibreak.isValid())
  {
    text.append(i18n("Please\nlaunch\nRSIBreak"));
  }  else {
    QDBusReply<int> idle = rsibreak.call("idleTime");
    QDBusReply<int> active1 = rsibreak.call("tinyLeft");
    QDBusReply<int> active2 = rsibreak.call("bigLeft");

    text.append(i18np("Currently Idle:\none second\n", 
                      "Currently Idle:\n%1 seconds\n", idle.value()));
    text.append(i18np("Next short break:\none second\n",
                      "Next short break:\n%1 seconds\n", active1.value()));
    text.append(i18np("Next big break:\n%1 seconds\n",
                      "Next big break:\n%1 seconds\n", active2.value()));
  }

  painter->save();
  painter->setPen(Qt::white);
  painter->drawText(QRect(10,10,130,130),
              Qt::AlignBottom+Qt::AlignHCenter,
              text);
  painter->drawText(boundingRect(),
              Qt::AlignBottom+Qt::AlignHCenter,
              "RSIBreak Info");
  painter->restore();
}

#include "rsibreak.moc"
