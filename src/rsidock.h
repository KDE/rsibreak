/* This file is part of the KDE project

   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

   The base was copied from ksynaptics:
      Copyright (C) 2004 Nadeem Hasan <nhasan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef RSIDOCK_H
#define RSIDOCK_H

#include <qpixmap.h>

#include <ksystemtray.h>

class RSIDock : public KSystemTray
{
  Q_OBJECT

  public:
    RSIDock( QWidget *parent, const char *name );
    ~RSIDock();

    signals:
        void configChanged();
        void dialogEntered();
        void dialogLeft();

    private slots:
        void slotConfigure();
        void slotAboutKDE();
        void slotAboutRSIBreak();
};

#endif // RSIDOCK_H

