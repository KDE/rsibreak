/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsiidletime.h"


int RSIIdleTimeImpl::getIdleTime() const
{
    return KIdleTime::instance()->idleTime();
}

int RSIIdleTimeFake::getIdleTime() const
{
    return m_idleTime;
}

void RSIIdleTimeFake::setIdleTime( const int _idleTime )
{
    m_idleTime = _idleTime;
}
