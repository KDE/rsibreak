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

// Local includes.

#include "dcopiface.h"

DCOPIface::DCOPIface(QObject *parent, const char *name)
    : QObject(parent, name), DCOPObject(name)
{
}

DCOPIface::~DCOPIface()
{
}

void DCOPIface::suspend()
{
    emit signalSuspend( true );
}
 
void DCOPIface::resume()
{
    emit signalSuspend( false );
}

#include "dcopiface.moc"