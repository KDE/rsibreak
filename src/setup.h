/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copright 2005 by Tom Albers <tomalbers@kde.nl>
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
 * ============================================================ */
 
#ifndef SETUP_H
#define SETUP_H

// KDE includes.

#include <kdialogbase.h>

class QFrame;

class SetupEditor;
class SetupTiming;

class Setup : public KDialogBase 
{
    Q_OBJECT

public:

    enum Page 
    {
        EditorPage=0,
        PluginsPage,
        SlideshowPage
    };

    Setup(QWidget* parent=0, const char* name=0, Page page=EditorPage);
    ~Setup();

private:

    QFrame           *page_general;
    QFrame           *page_timing;
    SetupEditor     *m_generalPage;
    SetupTiming    *m_timingPage;

private slots:

    void slotOkClicked();
};

#endif  /* SETUP_H  */
