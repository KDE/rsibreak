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
 
#ifndef RSIBREAK_ENGINE_H
#define RSIBREAK_ENGINE_H
 
#include <Plasma/DataEngine>

/**
 * This engine provides the current status of the RSIBreak application.
 */
class RSIBreakEngine : public Plasma::DataEngine
{
  Q_OBJECT

  public:
    RSIBreakEngine(QObject* parent, const QVariantList& args);
};
 
#endif // RSIBREAK_ENGINE_H
