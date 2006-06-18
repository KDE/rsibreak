/* ============================================================
 * Original copied from showfoto:
 *    Copyright 2005 by Gilles Caulier <caulier.gilles@free.fr>
 *
 * Copyright 2005-2006 by Tom Albers <tomalbers@kde.nl>
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
#include "setupmaximized.h"
#include "setupdcop.h"
#include "setup.h"

class SetupPriv
{
public:
    SetupGeneral     *generalPage;
    SetupTiming      *timingPage;
    SetupPopup       *popupPage;
    SetupMaximized   *maximizedPage;
    SetupDCOP        *DCOPPage;
};

Setup::Setup(QWidget* parent, const char* name)
     : KDialogBase(IconList, i18n("Configure"), Help|Ok|Cancel, Ok, parent,
                   name, true, true )
{
    d = new SetupPriv;

    QFrame *page_general = addPage(i18n("General"), i18n("General Settings"),
                        BarIcon("configure", KIcon::SizeMedium));
    d->generalPage = new SetupGeneral(page_general);

    QFrame *page_timing = addPage(i18n("Timings"), i18n("Timings"),
                              BarIcon("timings", KIcon::SizeMedium));
    d->timingPage = new SetupTiming(page_timing);

    QFrame *page_maximized = addPage(i18n("During Breaks"),
                              i18n("During Breaks"),
                              BarIcon("duringbreaks", KIcon::SizeMedium));
    d->maximizedPage = new SetupMaximized(page_maximized);

    QFrame *page_dcop = addPage(i18n("DCOP"), i18n("DCOP"),
                              BarIcon("dcop", KIcon::SizeMedium));
    d->DCOPPage = new SetupDCOP(page_dcop);

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOkClicked()) );

    showPage(0);
    resize( configDialogSize("Settings") );
    show();
}

Setup::~Setup()
{
    saveDialogSize("Settings");
    delete d;
}

void Setup::slotOkClicked()
{
    d->generalPage->applySettings();
    d->timingPage->applySettings();
    d->maximizedPage->applySettings();
    d->DCOPPage->applySettings();
    close();
}

#include "setup.moc"
