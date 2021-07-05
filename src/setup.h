/*
    SPDX-FileCopyrightText: 2005, 2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

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
 * @author Tom Albers <toma.org>
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
    explicit Setup( QWidget* parent = 0 );

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
