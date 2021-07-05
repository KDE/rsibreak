/*
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef RSIBREAK_RSIIDLETIME_H
#define RSIBREAK_RSIIDLETIME_H

#include <KIdleTime/KIdleTime>

class RSIIdleTime
{
public:
    virtual ~RSIIdleTime() = default;
    virtual int getIdleTime() const = 0;
};

class RSIIdleTimeImpl : public RSIIdleTime
{
public:
    ~RSIIdleTimeImpl() = default;
    int getIdleTime() const override;
};

class RSIIdleTimeFake : public RSIIdleTime
{
private:
    int m_idleTime = 0;
public:
    ~RSIIdleTimeFake() = default;
    int getIdleTime() const override;
    void setIdleTime( const int _idleTime );
};

#endif //RSIBREAK_RSIIDLETIME_H
