/* ============================================================
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

#ifndef SETUPDCOP_H
#define SETUPDCOP_H

class SetupDCOPPriv;
class QListViewItem;

/**
 * @class SetupDCOP
 * In this page you can setup DCOP commands to be executed when a break
 * starts and ends.
 * @author Tom Albers <tomalbers@kde.nl>
 */

class SetupDCOP : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent Widget
     */
    SetupDCOP(QWidget* parent = 0);

    /**
     * Destructor
     */
    ~SetupDCOP();

    /**
     * Call this if you want the settings saved from this page.
     */
    void applySettings();

private slots:
    void slotTableClicked( QListViewItem * item );

private:
    void readSettings();
    SetupDCOPPriv     *d;
};

#endif /* SETUPDCOP_H */
