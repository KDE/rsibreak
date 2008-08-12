/*
 *   Copyright (c) 2008 Rafał Rzepecki <divided.mind@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "engine.h"

#include "rsibreak_interface.h"

RSIBreakEngine::RSIBreakEngine(QObject* parent, const QVariantList& args)
              : Plasma::DataEngine(parent, args),
                m_rsibreakInterface(0)
{
}

void RSIBreakEngine::init()
{
  m_rsibreakInterface = new OrgRsibreakRsiwidgetInterface("org.rsibreak.rsibreak", 
                                                           "/rsibreak", 
                                                           QDBusConnection::sessionBus(),
                                                           this);
  updateSourceEvent("idleTime");
  updateSourceEvent("tinyLeft");
  updateSourceEvent("bigLeft");
  setMinimumPollingInterval(333);
}

bool RSIBreakEngine::updateSourceEvent(const QString &name)
{
  QDBusReply<int> reply = m_rsibreakInterface->call(name);
  if (reply.isValid()) {
    setData(name, reply.value());
    return true;
  }

  return false;
}

K_EXPORT_PLASMA_DATAENGINE(rsibreak, RSIBreakEngine)
 
#include "engine.moc"
