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

// Qt includes.

#include <qtabwidget.h>
#include <qapplication.h>
#include <qframe.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

// Local includes.

#include "setupgeneral.h"
#include "setuptiming.h"
#include "setup.h"

Setup::Setup(QWidget* parent, const char* name)
     : KDialogBase(IconList, i18n("Configure"), Help|Ok|Cancel, Ok, parent,
                   name, true, true )
{
    kdDebug() << "Entering Setup" << endl;
    page_general = addPage(i18n("General"), i18n("General Settings"),
                        BarIcon("configure", KIcon::SizeMedium));
    m_generalPage = new SetupGeneral(page_general);

    page_timing = addPage(i18n("Timings"), i18n("Timings"),
                            BarIcon("rsibreak3", KIcon::SizeMedium));
    m_timingPage = new SetupTiming(page_timing);

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOkClicked()) );

    showPage(0);
    resize( configDialogSize("Settings") );
    show();
}

Setup::~Setup()
{
    kdDebug() << "Entering ~Setup" << endl;
    saveDialogSize("Settings");
}

void Setup::slotOkClicked()
{
    kdDebug() << "Entering slotOkClicked" << endl;
    m_generalPage->applySettings();
    m_timingPage->applySettings();
    close();
}

#include "setup.moc"
