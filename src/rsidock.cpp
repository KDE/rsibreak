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
#include <kaction.h>
#include <kaboutapplication.h>
#include <kbugreport.h>
#include <kglobalaccel.h>
#include <kkeydialog.h>

RSIDock::RSIDock( QWidget *parent, const char *name )
    : KSystemTray( parent, name ), m_suspended( false ), m_tooltiptimer( 0 )
{
  kdDebug() << "Entering RSIDock" << endl;

  contextMenu()->insertItem(SmallIcon("configure"),
                         i18n("Configure RSIBreak..."), this,
                         SLOT(slotConfigure()));
  mSuspendItem = contextMenu()->insertItem(SmallIcon("player_pause"),
                         i18n("Suspend RSIBreak"), this,
                         SLOT(slotSuspend()));
  contextMenu()->insertSeparator();
  contextMenu()->insertItem(i18n("Report Bug..."), this,
                         SLOT(slotReportBug()));
  contextMenu()->insertSeparator();
  contextMenu()->insertItem(SmallIcon("about_kde"),
                         i18n("About KDE"), this,
                         SLOT(slotAboutKDE()));
  contextMenu()->insertItem(SmallIcon("info"),
                         i18n("About RSIBreak"), this,
                         SLOT(slotAboutRSIBreak()));

  m_accel = new KGlobalAccel(this);
  m_accel->insert("breakRequest", i18n("This is where the user can request a "
                       "break", "Take a break now"),
                  i18n("This way you can have a break now"),
                  KKey::QtWIN+SHIFT+Key_B, KKey::QtWIN+CTRL+Key_B,
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

void RSIDock::slotSuspend()
{
    kdDebug() << "Entering RSIDock::slotSuspend" << endl;

    if( m_suspended )
    {
        emit unsuspend();

        setPixmap( KSystemTray::loadIcon( "rsibreak0" ) );
        contextMenu()->changeItem( mSuspendItem, SmallIcon( "player_pause" ),
                                   i18n("Suspend RSIBreak") );
    }
    else
    {
        emit suspend();

        setPixmap( KSystemTray::loadIcon( "rsibreakx" ) );
        contextMenu()->changeItem( mSuspendItem, SmallIcon( "player_play" ),
                                   i18n( "Resume RSIBreak" ) );
    }

    m_suspended = !m_suspended;
}

void RSIDock::showEvent( QShowEvent * )
{
    kdDebug() << "Entering RSIDock::showEvent" << endl;

    contextMenu()->insertSeparator();
    KAction* action = actionCollection()->
            action(KStdAction::name(KStdAction::Quit));
    if (action)
        action->plug(contextMenu());
}

void RSIDock::mousePressEvent( QMouseEvent *e )
{
    kdDebug() << "Entering RSIDock::mousePressEvent" << endl;

    emit hideToolTip();

    if (e->button() == RightButton)
        contextMenu()->exec( e->globalPos() );
}

void RSIDock::enterEvent( QEvent * )
{
  kdDebug() << "Entering RSIDock::enterEvent" << endl;

  if( !m_tooltiptimer )
  {
    m_tooltiptimer = new QTimer( this );
    connect( m_tooltiptimer, SIGNAL( timeout() ), SIGNAL( showToolTip() ) );
  }
  /* FIXME: Find a nice timeout. Bram finds 1500 too long */
  m_tooltiptimer->start( 500, true );
}

void RSIDock::leaveEvent( QEvent * )
{
  kdDebug() << "Entering RSIDock::leaveEvent" << endl;

  if( m_tooltiptimer )
    m_tooltiptimer->stop();

  emit hideToolTip();
}

#include "rsidock.moc"
