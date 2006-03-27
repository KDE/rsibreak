/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copyright (C) 2006 by Tom Albers <tomalbers@kde.nl>
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

#ifndef SETUPTIMING_H
#define SETUPTIMING_H

class SetupTimingPriv;

/**
 * @class SetupTiming
 * These contain the timings settings of RSIBreak, currently
 * that means settings for tiny and big breaks
 * This file is originally copied from showfoto
 * @author Gilles Caulier <caulier dot gilles at free.fr>
 * @author Tom Albers <tomalbers@kde.nl>
 */

class SetupTiming : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent Widget
         */
    SetupTiming(QWidget* parent = 0);

    /**
     * Destructor
     */
    ~SetupTiming();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

private slots:
    void slotTinyValueChanged(int i);
    void slotBigValueChanged( int i );
    void slotTinyDurationValueChanged( int i );
    void slotBigDurationValueChanged( int i );
    void slotSlideIntervalValueChanged( int i );

private:
    void readSettings();
    SetupTimingPriv     *d;
};

#endif /* SETUPTIMING_H */
