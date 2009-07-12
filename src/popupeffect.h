/*
   Copyright (C) 2009 Tom Albers <toma.org>

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

#ifndef POPUPEFFECT_H
#define POPUPEFFECT_H

#include <breakbase.h>

#include <QObject>

class PassivePopup;
class QLabel;
class QSystemTrayIcon;

class PopupEffect : public BreakBase
{
    Q_OBJECT

public:
    PopupEffect( QObject* );
    ~PopupEffect();
    void setTray( QSystemTrayIcon* );
    void setLabel( const QString& );

public slots:
    void activate();
    void deactivate();

private:
    PassivePopup* m_popup;
    QLabel* m_label;
    QSystemTrayIcon* m_tray;
};

#endif // POPUPEFFECT_H
