/* ============================================================
 * Copyright 2005-2007 by Tom Albers <tomalbers@kde.nl>
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
#include <qtabwidget.h>
#include <qlabel.h>
#include <qapplication.h>
//Added by qt3to4:

// KDE includes.
#include <kiconloader.h>
#include <klocale.h>

// Local includes.
#include "setupgeneral.h"
#include "setuptiming.h"
#include "setupmaximized.h"

class SetupPriv
{
public:
    SetupGeneral  *generalPage;
    SetupTiming  *timingPage;
    SetupMaximized  *maximizedPage;
};

Setup::Setup( QWidget* parent )
        : KPageDialog( parent )
{
    setFaceType( List );
    d = new SetupPriv;

    d->generalPage = new SetupGeneral( this );
    KPageWidgetItem* page1 = addPage( d->generalPage, i18n( "General Settings" ) );
    page1->setIcon( KIcon( "configure" ) );

    d->timingPage = new SetupTiming( this );
    KPageWidgetItem* page2 = addPage( d->timingPage, i18n( "Timings" ) );
    page2->setIcon( KIcon( "timings" ) );

    d->maximizedPage = new SetupMaximized( this );
    KPageWidgetItem* page3 = addPage( d->maximizedPage, i18n( "During Breaks" ) );
    page3->setIcon( KIcon( "duringbreaks" ) );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOkClicked() ) );

    setInitialSize( QSize( 625, 575 ) );
    KConfigGroup config = KGlobal::config()->group( "SetupDimensions" );
    restoreDialogSize( config );
    show();
}

Setup::~Setup()
{
    KConfigGroup config = KGlobal::config()->group( "SetupDimensions" );
    saveDialogSize( config );
    delete d;
}

void Setup::slotOkClicked()
{
    d->generalPage->applySettings();
    d->timingPage->applySettings();
    d->maximizedPage->applySettings();
    close();
}

#include "setup.moc"
