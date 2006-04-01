/* ============================================================
 * 
 * Copyright (C) 2006 Tom Albers <tomalbers@kde.nl>
 *
 * Orginal copied from digikam: Leonid Zeitlin <lz@europe.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */
 
#ifndef DCOPIFACE_H
#define DCOPIFACE_H

// Qt includes.

#include <qobject.h>
#include <dcopobject.h>

/**
* This class implements a DCOP interface for RSIBreak.
* At the moment it supports stopping and resuming
*
* @short DCOP interface for RSIBreak
* @author Leonid Zeitlin, Tom Albers
*/

class DCOPIface : public QObject, public DCOPObject
{
    K_DCOP
    Q_OBJECT

public:

    /**
     * Standard constructor.
     * @param parent Parent object reference, passed to QObject constructor
     * @param name Specifis the name of the object, passed to QObject constructor
     */
    DCOPIface(QObject *parent = 0, const char *name = 0);

    /**
     * Standard destructor
    */
    ~DCOPIface();

signals:
    
    /**
     * This signal is emitted when @ref suspend() or @ref resume() is called via DCOP
     * @param suspend true to suspend and false when resume is requested.
     */
    void signalSuspend( bool suspend );

public:

k_dcop:
    /**
     * This method can be called via DCOP to interrupt RSIBreak
     */
    ASYNC suspend();

    /**
     * This method can be called via DCOP to resume RSIBreak
     */
    ASYNC resume();
};

#endif // DCOPIFACE_H
