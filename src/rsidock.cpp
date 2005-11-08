/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

   Orginal copied from ksynaptics:
      Copyright (C) 2004 Nadeem Hasan <nhasan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "rsidock.h"
#include "setup.h"

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kiconloader.h>
#include <kaboutkde.h>
#include <kaboutapplication.h>
#include <kbugreport.h>
#include <kglobalaccel.h>
#include <kkeydialog.h>

RSIDock::RSIDock( QWidget *parent, const char *name )
    : KSystemTray( parent, name )
{
  kdDebug() << "Entering RSIDock" << endl;
  QPixmap dockPixmap = KSystemTray::loadIcon( "rsibreak0" );
  setPixmap( dockPixmap );

  contextMenu()->insertItem(SmallIcon("configure"),
                         i18n("Configure RSIBreak..."), this,
                         SLOT(slotConfigure()));
  contextMenu()->insertSeparator();
  contextMenu()->insertItem(i18n("Report Bug..."), this,
                         SLOT(slotReportBug()));
  contextMenu()->insertSeparator();
  contextMenu()->insertItem(SmallIcon("about_kde"),
                         i18n("About KDE..."), this,
                         SLOT(slotAboutKDE()));
  contextMenu()->insertItem(SmallIcon("info"),
                         i18n("About RSIBreak..."), this,
                         SLOT(slotAboutRSIBreak()));

  m_accel = new KGlobalAccel(this);
  m_accel->insert("breakRequest", i18n("Take a break now"),
                  i18n("This way you can have a break now"),
                  KKey::QtWIN+Key_B, KKey::QtWIN+CTRL+Key_B,
                  this, SLOT( slotBreakRequest() ));
  m_accel->updateConnections();

}

RSIDock::~RSIDock()
{
kdDebug() << "Entering ~RSIDock" << endl;
}

void RSIDock::slotConfigure()
{
    kdDebug() << "Entering slotConfigure" << endl;
    Setup setup(this);
    emit dialogEntered();
    if (setup.exec() != QDialog::Accepted)
    {
        emit dialogLeft();
        return;
    }
    emit configChanged();
}

void RSIDock::slotAboutKDE()
{
    kdDebug() << "Entering slotAboutKDE" << endl;
    KAboutKDE about;
    emit dialogEntered();
    about.exec();
    emit dialogLeft();
}

void RSIDock::slotAboutRSIBreak()
{
    kdDebug() << "Entering slotAboutRSIBreak" << endl;
    KAboutApplication about;
    emit dialogEntered();
    about.exec();
    emit dialogLeft();
}

void RSIDock::slotReportBug()
{
    kdDebug() << "Entering slotReportBug" << endl;
    KBugReport bug;
    emit dialogEntered();
    bug.exec();
    emit dialogLeft();
}

void RSIDock::slotBreakRequest()
{
    emit breakRequest();
}

#include "rsidock.moc"
