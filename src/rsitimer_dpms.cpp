/*
   Copyright (C) 2006 Tom Albers <toma@kde.org>

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


#include <X11/Xutil.h>
#include <X11/extensions/dpms.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>

#include <fixx11h.h>


bool QueryDPMSTimeouts( Display* display, int& standby, int& suspend, int& off )
{
    bool result = false;
    standby = 0;
    suspend = 0;
    off = 0;

    int event_base;
    int error_base;
    CARD16 x_standby;
    CARD16 x_suspend;
    CARD16 x_off;

    if ( DPMSQueryExtension( display, &event_base, &error_base ) )
        if ( DPMSCapable( display ) )
            if ( DPMSGetTimeouts( display, &x_standby, &x_suspend, &x_off ) ) {
                standby = ( int )x_standby;
                suspend = ( int )x_suspend;
                off     = ( int )x_off;
                result  = true;
            }
    return result;
}

