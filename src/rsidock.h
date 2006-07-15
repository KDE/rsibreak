/* This file is part of the KDE project

   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>

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
#include <qtimer.h>

#include <kdialogbase.h>
#include <ksystemtray.h>

class KGlobalAccel;
class KHelpMenu;

class RSIStatWidget;

/**
 * @class RSIDock
 * This class is responsible for putting rsibreak in the system tray
 * and provide a proper menu when right clicked on the icon.
 *
 * Originally this file was copied from KSynaptics
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

    /**
     * With this function you can set the icon used in the tray.
     * @param level Indicates the icon, you can currently choose from 0-4
     */
    void setIcon(int level);

    public slots:
      /**
        Notifies the docker that RSIBreak is in relax state.
        Used to hide the tooltip while showing relax moments.
      */
      void relaxEntered( int i, bool );

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
         * Suspend RSIBreak on user's request.
        */
        void suspend( bool );

        /**
          Shows the tooltip.
        */
        void showToolTip();

        /**
          Hides the tooltip.
        */
        void hideToolTip();

    protected:
        /**
         * Reimplemented because we do not want an action on left click
        */

        void mousePressEvent( QMouseEvent *e );
        /**
         * Reimplemented because we do not want a minimize action in the menu
        */
        void showEvent( QShowEvent * );

        /** Triggered when the mouse enters the icon in the docker. */
        virtual void enterEvent( QEvent * );

        /** Triggered when the mouse leaves the icon in the docker. */
        virtual void leaveEvent( QEvent * );

    private slots:
        void slotConfigure();
        void slotConfigureNotifications();
        void slotBreakRequest();
        void slotSuspend();
        void slotShowToolTip();
        void slotShowStatistics();
        void slotResetStats();

    private:
        KGlobalAccel* m_accel;
        KHelpMenu*    m_help;

        int m_suspendItem;
        bool m_suspended;
        bool m_tooltiphidden;

        /** This bool is needed to determine if Quit is added to the context
            menu. Although not needed for KDE, GNome will plug the Quit item
            twice in the contextmenu. */
        bool m_hasQuit;

        QTimer *m_tooltiptimer;
        KDialogBase *m_statsDialog;
        RSIStatWidget *m_statsWidget;
};

#endif // RSIDOCK_H

