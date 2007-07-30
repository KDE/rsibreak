/* ============================================================
 * Copyright 2005,2007 by Tom Albers <tomalbers@kde.nl>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * ============================================================ */

#ifndef SETUPMAXIMIZED_H
#define SETUPMAXIMIZED_H

#include <qwidget.h>

class SetupMaximizedPriv;

/**
 * @class SetupMaximized
 * These contain the general settings of RSIBreak when maximized
 * that means settings for the Counter, Image Path and
 * minimize button
 * @author Tom Albers <tomalbers@kde.nl>
 */

class SetupMaximized : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent Widget
     */
    SetupMaximized(QWidget* parent = 0);

    /**
     * Destructor
     */
    ~SetupMaximized();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

private slots:
    void slotHideCounter();
    void slotFontPicker();
    void slotFolderPicker();
    void slotFolderEdited(const QString& newPath);
    void slotHideFlash();
    void slotUseImages();

private:
    void readSettings();
    SetupMaximizedPriv  *d;
};

#endif /* SETUPMAXIMIZED_H */
