/* This file is part of the KDE project
   Copyright (C) 2006 Tom Albers <tomalbers@kde.nl>

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

#ifndef RSIGLOBALS_H
#define RSIGLOBALS_H

#include <qdatetime.h>
#include <qimage.h>

#include <kpassivepopup.h>

/**
 * @class RSIGlobals
 * This class consists of a few commonly used routines
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIGlobals
{
  public:
    /**
     * Converts @p seconds to a reasonable string.
     * @param seconds the amount of seconds
     * @returns a formatted string. Examples:
     * - 3601: "One hour"
     * - 3665: "One hour and one minute"
     * - 60: "One minute"
     * - 61: "One minute and one second"
     * - 125: "2 minutes and 5 seconds"
     * - 2: "2 seconds"
     * - 0: "0 seconds"
     */
    static QString formatSeconds(const int seconds);
};

#endif // RSITOOLTIP_H
