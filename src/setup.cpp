/* ============================================================
 * Copyright 2005-2007,2010 by Tom Albers <toma@kde.org>
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

#include "setup.h"

// Qt includes.
#include <QIcon>

// KDE includes.
#include <KConfigGroup>
#include <KWindowConfig>
#include <KSharedConfig>
#include <klocalizedstring.h>

// Local includes.
#include "setupgeneral.h"
#include "setuptiming.h"
#include "setupmaximized.h"
#include "setupnotifications.h"


class SetupPriv
{
public:
    SetupGeneral  *generalPage;
    SetupTiming  *timingPage;
    SetupMaximized  *maximizedPage;
    SetupNotifications *notificationsPage;
};

Setup::Setup( QWidget* parent )
        : KPageDialog( parent )
{
    setFaceType( List );
    d = new SetupPriv;

    d->generalPage = new SetupGeneral( this );
    KPageWidgetItem* page1 = addPage( d->generalPage, i18n( "General Settings" ) );
    page1->setIcon( QIcon::fromTheme( "configure" ) );

    d->timingPage = new SetupTiming( this );
    KPageWidgetItem* page2 = addPage( d->timingPage, i18n( "Timings" ) );
    page2->setIcon( QIcon::fromTheme( "timings" ) );

    d->maximizedPage = new SetupMaximized( this );
    KPageWidgetItem* page3 = addPage( d->maximizedPage, i18n( "During Breaks" ) );
    page3->setIcon( QIcon::fromTheme( "duringbreaks" ) ); // krazy:exclude=iconnames

    d->notificationsPage = new SetupNotifications( this );
    KPageWidgetItem* page4 = addPage( d->notificationsPage, i18n( "Actions" ) );
    page4->setIcon( QIcon::fromTheme( "configure" ) ); // krazy:exclude=iconnames

    connect(this, &Setup::accepted, this, &Setup::slotOkClicked);

    connect(d->generalPage, &SetupGeneral::useIdleTimerChanged, d->maximizedPage, &SetupMaximized::slotSetUseIdleTimer);
    d->maximizedPage->slotSetUseIdleTimer(d->generalPage->useIdleTimer());
    connect(d->generalPage, &SetupGeneral::useIdleTimerChanged, d->timingPage, &SetupTiming::slotSetUseIdleTimer);
    d->timingPage->slotSetUseIdleTimer(d->generalPage->useIdleTimer());

    KConfigGroup config = KSharedConfig::openConfig()->group( "SetupDimensions" );
    KWindowConfig::restoreWindowSize( windowHandle(), config );
    show();
}

Setup::~Setup()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "SetupDimensions" );
    KWindowConfig::saveWindowSize( windowHandle(), config );
    delete d;
}

void Setup::slotOkClicked()
{
    d->generalPage->applySettings();
    d->timingPage->applySettings();
    d->maximizedPage->applySettings();
    d->notificationsPage->save();
    close();
}
