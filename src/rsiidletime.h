/*
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

#ifndef RSIBREAK_RSIIDLETIME_H
#define RSIBREAK_RSIIDLETIME_H

#include <KIdleTime/KIdleTime>

class RSIIdleTime
{
public:
    virtual ~RSIIdleTime() = 0;
    virtual int getIdleTime() const = 0;
};

class RSIIdleTimeImpl : public RSIIdleTime
{
public:
    virtual ~RSIIdleTimeImpl() = default;
    virtual int getIdleTime() const;
};

class RSIIdleTimeFake : public RSIIdleTime
{
private:
    int m_idleTime = 0;
public:
    virtual ~RSIIdleTimeFake() = default;
    virtual int getIdleTime() const;
    void setIdleTime( const int _idleTime );
};

#endif //RSIBREAK_RSIIDLETIME_H
