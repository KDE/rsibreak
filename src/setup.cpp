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
#include "setuppopup.h"
#include "setupmaximized.h"
#include "setup.h"

class SetupPriv
{
public:
    SetupGeneral     *generalPage;
    SetupTiming      *timingPage;
    SetupPopup       *popupPage;
    SetupMaximized   *maximizedPage;
};

Setup::Setup(QWidget* parent, const char* name)
     : KDialogBase(IconList, i18n("Configure"), Help|Ok|Cancel, Ok, parent,
                   name, true, true )
{
    kdDebug() << "Entering Setup" << endl;
    d = new SetupPriv;
    
    QFrame *page_general = addPage(i18n("General"), i18n("General Settings"),
                        BarIcon("configure", KIcon::SizeMedium));
    d->generalPage = new SetupGeneral(page_general);

    QFrame *page_timing = addPage(i18n("Timings"), i18n("Timings"),
                            BarIcon("rsibreak3", KIcon::SizeMedium));
    d->timingPage = new SetupTiming(page_timing);

    QFrame *page_popup = addPage(i18n("Popup"), i18n("Popup"),
                         BarIcon("misc", KIcon::SizeMedium));
    d->popupPage = new SetupPopup(page_popup);

    QFrame *page_maximized = addPage(i18n("Maximized"), i18n("Maximized"),
                             BarIcon("background", KIcon::SizeMedium));
    d->maximizedPage = new SetupMaximized(page_maximized);

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
    delete d;
}

void Setup::slotOkClicked()
{
    kdDebug() << "Entering slotOkClicked" << endl;
    d->generalPage->applySettings();
    d->timingPage->applySettings();
    d->popupPage->applySettings();
    d->maximizedPage->applySettings();
    close();
}

#include "setup.moc"
