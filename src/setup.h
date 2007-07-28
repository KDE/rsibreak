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

#ifndef SETUP_H
#define SETUP_H

// KDE includes.
#include <kpagedialog.h>

class SetupPriv;

/**
 * @class Setup
 * This class manages the dialog chown in which the user
 * can make all the necessary settings. Each part of the config
 * is located in separate files, See SetupGeneral and SetupTimings
 * for example
 * @author Tom Albers <tomalbers@kde.nl>
 */
class Setup : public KPageDialog
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent Parent Widget
     * @param name Name
     */
    explicit Setup(QWidget* parent=0);

    /**
     * Destructor
     */
    ~Setup();

private:
    SetupPriv        *d;

private slots:
    void slotOkClicked();
};

#endif  /* SETUP_H  */
