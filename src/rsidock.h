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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/


#ifndef RSIDOCK_H
#define RSIDOCK_H

#include <qpixmap.h>

#include <ksystemtray.h>

/** 
 * @class RSIDock
 * This class is resonsible for putting rsibreak in the system tray
 * and provide a proper menu when right clicked on the icon.
 * 
 * Originaly this file was copied from KSynaptics
 * @author Nadeem Hasan <nhasan@kde.org>
 * @author Tom Albers <tomalbers@kde.nl>
 */
class RSIDock : public KSystemTray
{
  Q_OBJECT

  public:
    /**
     * Contructor
     * @param parent Parent Widget
     * @param name Name
     */
    RSIDock( QWidget *parent, const char *name );

    /**
     * Destructor
     */
    ~RSIDock();

    signals:
        /**
         * This signal is emitted when the user has left
         * the settings.
         */
        void configChanged();

        /**
         * This signal is emitted when the user enters a dialog
         * this can be an about X screen or the settings.
         */
        void dialogEntered();

        /**
         * This signal is emitted when the user leaves one of the
         * about X dialogs, it will not beemitted when the settings
         * dialog is left, the configChanged() is emitted.
         */
        void dialogLeft();

    private slots:
        void slotConfigure();
        void slotAboutKDE();
        void slotAboutRSIBreak();
};

#endif // RSIDOCK_H

