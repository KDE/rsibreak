/*

   Copyright (C) 2005-2006 Tom Albers <toma@kde.org>

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

#include <kdialog.h>
#include <kstatusnotifieritem.h>

class KHelpMenu;

class RSIStatWidget;

/**
 * @class RSIDock
 * This class is responsible for putting rsibreak in the system tray
 * and provide a proper menu when right clicked on the icon.
 *
 * Originally this file was copied from KSynaptics
 * @author Nadeem Hasan <nhasan@kde.org>
 * @author Tom Albers <toma.org>
 */
class RSIDock : public KStatusNotifierItem
{
    Q_OBJECT

public:
    RSIDock( QObject *parent );

    /**
     * Destructor
     */
    ~RSIDock();

public slots:
    void setCounters( int tiny_left, int big_left );

signals:
    /**
     * This signal is emitted when the user has left
     * the settings.
     * @param restart Defines whether the timer should
     * immediately restarted. This should not happen
     * when RSIBreak is suspended.
     */
    void configChanged( bool restart );

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

    /**
     * This signal is emitted when the user wishes to take the break
     * at this moment, either via the shortcut or the menu (lmb)
     */
    void breakRequest();

    /**
     * This signal is emitted when the user wishes to see all
     * debug timing info. Needs a --enable-debug comiled RSIBreak
     */
    void debugRequest();

    /**
     * Suspend RSIBreak on user's request.
    */
    void suspend( bool );

private slots:
    void slotConfigure();
    void slotConfigureNotifications();
    void slotSuspend();
    void slotBreakRequest();
    void slotShowStatistics();
    void slotResetStats();

private:
    KHelpMenu*    m_help;

    QAction* m_suspendItem;
    bool m_suspended;

    KDialog *m_statsDialog;
    RSIStatWidget *m_statsWidget;
};

#endif // RSIDOCK_H

