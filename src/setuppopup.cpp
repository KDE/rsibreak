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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ============================================================ */

// QT includes.

#include <qlayout.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kshortcutdialog.h>

// Local includes.

#include "setuppopup.h"

SetupPopup::SetupPopup(QWidget* parent )
           : QWidget(parent)
{
   kdDebug() << "Entering SetupPopup" << endl;
   QVBoxLayout *layout = new QVBoxLayout( parent );
   layout->setSpacing( KDialog::spacingHint() );
   layout->setAlignment( AlignTop );

   readSettings();

}

SetupPopup::~SetupPopup()
{
    kdDebug() << "Entering ~SetupPopup" << endl;
}


void SetupPopup::applySettings()
{
    kdDebug() << "Entering applySettings" << endl;
    KConfig* config = kapp->config();

    config->sync();

}

void SetupPopup::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
}

#include "setuppopup.moc"
