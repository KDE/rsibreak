/* ============================================================
 * Copyright 2005-2006 by Tom Albers <toma@kde.org>
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
 *
 * ============================================================ */

#ifndef SETUPGENERAL_H
#define SETUPGENERAL_H

#include <qwidget.h>

class SetupGeneralPriv;

/**
 * @class SetupGeneral
 * These contain the general settings of RSIBreak
* @author Tom Albers <toma.org>
 */

class SetupGeneral : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent Widget
     */
    explicit SetupGeneral( QWidget* parent = 0 );

    /**
     * Destructor
     */
    ~SetupGeneral();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

    bool useIdleTimer() const;

signals:
    void useIdleTimerChanged( bool useIdleTimer );

private:
    void readSettings();

    SetupGeneralPriv    *d;
};

#endif /* SETUPGENERAL_H */
