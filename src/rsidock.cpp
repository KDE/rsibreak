/* This file is part of the KDE project
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>

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
#include "rsistatwidget.h"
#include "rsistats.h"

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
#include <kmessagebox.h>

RSIDock::RSIDock( QWidget *parent, const char *name )
    : KSystemTray( parent, name ), m_suspended( false ), m_tooltiphidden( false )
    , m_hasQuit ( false ), m_tooltiptimer( 0 ), m_statsDialog( 0 )
    , m_statsWidget( 0 )

{

    contextMenu()->insertItem(SmallIcon("configure"),
                            i18n("Configure RSIBreak..."), this,
                            SLOT(slotConfigure()));
    m_suspendItem = contextMenu()->insertItem(SmallIcon("player_pause"),
                            i18n("Suspend RSIBreak"), this,
                            SLOT(slotSuspend()));
    contextMenu()->insertItem(SmallIcon("gear"),
                            i18n("Usage Statistics"), this,
                            SLOT( slotShowStatistics() ) );
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
    delete m_statsWidget;
    m_statsWidget = 0;
}

void RSIDock::slotConfigure()
{
    kdDebug() << "Entering slotConfigure" << endl;
    Setup setup(this);
    emit dialogEntered();
    if (setup.exec() != QDialog::Accepted)
    {
        if ( !m_suspended )
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

    if ( !m_suspended ) // don't start the timer!
      emit dialogLeft();
}

void RSIDock::slotAboutRSIBreak()
{
    kdDebug() << "Entering slotAboutRSIBreak" << endl;
    KAboutApplication about;
    emit dialogEntered();
    about.exec();

    if ( !m_suspended ) // don't start the timer!
      emit dialogLeft();
}

void RSIDock::slotReportBug()
{
    kdDebug() << "Entering slotReportBug" << endl;
    KBugReport bug;
    emit dialogEntered();
    bug.exec();

    if ( !m_suspended ) // don't start the timer!
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
        emit suspend( false );

        setPixmap( KSystemTray::loadIcon( "rsibreak0" ) );
        contextMenu()->changeItem( m_suspendItem, SmallIcon( "player_pause" ),
                                   i18n("Suspend RSIBreak") );
    }
    else
    {
        emit suspend( true );

        setPixmap( KSystemTray::loadIcon( "rsibreakx" ) );
        contextMenu()->changeItem( m_suspendItem, SmallIcon( "player_play" ),
                                   i18n( "Resume RSIBreak" ) );
    }

    m_suspended = !m_suspended;
}

void RSIDock::showEvent( QShowEvent * )
{
    kdDebug() << "Entering RSIDock::showEvent" << endl;

    if (!m_hasQuit)
    {
        contextMenu()->insertSeparator();

        KAction* action = actionCollection()->
                action(KStdAction::name(KStdAction::Quit));
        if (action)
            action->plug(contextMenu());

        m_hasQuit=true;
    }
}

void RSIDock::mousePressEvent( QMouseEvent *e )
{
    kdDebug() << "Entering RSIDock::mousePressEvent" << endl;

    m_tooltiptimer->stop();
    emit hideToolTip();

    if (e->button() == RightButton)
        contextMenu()->exec( e->globalPos() );
}

void RSIDock::enterEvent( QEvent * )
{
    if( !m_tooltiptimer )
    {
        m_tooltiptimer = new QTimer( this );
        connect( m_tooltiptimer, SIGNAL( timeout() ), SLOT( slotShowToolTip() ) );
    }

    m_tooltiptimer->start( 500, true );
}

void RSIDock::leaveEvent( QEvent * )
{
    if( m_tooltiptimer )
        m_tooltiptimer->stop();

    emit hideToolTip();
}

void RSIDock::relaxEntered( int i )
{
    m_tooltiphidden = i > 0;
}

void RSIDock::slotShowToolTip()
{
    kdDebug() << "Entering RSIDock::slotShowToolTip" << endl;

    if( !m_tooltiphidden )
      emit showToolTip();
}

void RSIDock::slotShowStatistics()
{
    kdDebug() << "Entering RSIDock::showStatistics()" << endl;

    if ( !m_statsDialog )
    {
      m_statsDialog = new KDialogBase( this, 0, false,
                                       i18n("Usage Statistics"),
                                       KDialogBase::Ok|KDialogBase::User1,
                                       KDialogBase::Ok, false,
                                       i18n("Reset"));
      m_statsWidget = new RSIStatWidget(m_statsDialog);
      connect(m_statsDialog, SIGNAL(user1Clicked()),
              this, SLOT(slotResetStats()));

      m_statsDialog->setMainWidget( m_statsWidget );
    }

    m_statsDialog->show();
}

void RSIDock::slotResetStats()
{
    kdDebug() << "Entering RSIDock::slotResetStats" << endl;
    int i = KMessageBox::warningContinueCancel( this,
                i18n("This will reset all statistics to zero. "
                     "Is that what you want?"),
                i18n("Reset the statistics"),
                i18n("Reset"),
                "resetStatistics");

    if (i == KMessageBox::Continue)
        RSIStats::instance()->reset();
}

#include "rsidock.moc"
