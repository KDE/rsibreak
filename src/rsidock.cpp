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
#include "rsiglobals.h"

#include <qtimer.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QEvent>
#include <QMouseEvent>

#include <KAboutData>
#include <KComponentData>
#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>
#include <knotifyconfigwidget.h>
#include <kiconloader.h>
#include <kaction.h>
#include <khelpmenu.h>
#include <kglobalaccel.h>
#include <kshortcutsdialog.h>
#include <kmessagebox.h>
#include <kwindowsystem.h>

RSIDock::RSIDock( QWidget *parent, const char *name )
    : KSystemTrayIcon( parent ), m_suspended( false )
    , m_hasQuit ( false ), m_statsDialog( 0 ), m_statsWidget( 0 )
{

   // TODO:
    //m_help = new KHelpMenu( this, KGlobal::mainComponent().aboutData());

//     contextMenu()->insertItem(SmallIcon("about_kde"), m_help->menu()->text(KHelpMenu::menuAboutKDE),
//                               m_help, SLOT(aboutKDE()));
//     contextMenu()->insertItem(SmallIcon("info"),  m_help->menu()->text(KHelpMenu::menuAboutApp),
//                               m_help, SLOT(aboutApplication()));
//     contextMenu()->insertItem(SmallIcon("contents"),  m_help->menu()->text(KHelpMenu::menuHelpContents),
//                               m_help, SLOT(appHelpActivated()));
/*    contextMenu()->insertSeparator();
    contextMenu()->insertItem(m_help->menu()->text(KHelpMenu::menuReportBug),
                              m_help, SLOT(reportBug()));*/
    contextMenu()->insertSeparator();
    m_suspendItem = contextMenu()->insertItem(SmallIcon("player_pause"),
                              i18n("&Suspend RSIBreak"), this,
                              SLOT(slotSuspend()));
    contextMenu()->insertItem(i18n("&Usage Statistics"),
                              this, SLOT( slotShowStatistics() ) );
    contextMenu()->insertItem(SmallIcon("knotify"),
                              i18n("Configure &Notifications..."),
                              this, SLOT(slotConfigureNotifications()));
    contextMenu()->insertItem(SmallIcon("configure"),
                              i18n("&Configure RSIBreak..."),
                              this, SLOT(slotConfigure()));

    //TODO: we can move this or somethjing?
    m_accel = KGlobalAccel::self();
//     m_accel->insert("breakRequest", i18n("This is where the user can request a "
//                         "break", "Take a break now"),
//                     i18n("This way you can have a break now"),
//                     KKey::QtWIN+SHIFT+Key_B, KKey::QtWIN+CTRL+Key_B,
//                     this, SLOT( slotBreakRequest() ));
//     m_accel->insert("debugRequest", "This is where the user can request a "
//             "continues dump of timings",
//             i18n("This way you can have a break now"),
//             KKey::QtWIN+SHIFT+Key_F12, KKey::QtWIN+CTRL+Key_F12,
//             this, SLOT( slotDebugRequest() ));
//     m_accel->updateConnections();
}

RSIDock::~RSIDock()
{
    delete m_statsWidget;
    m_statsWidget = 0;
}


void RSIDock::slotConfigureNotifications()
{
    KNotifyConfigWidget::configure(/* TODO: this */ 0);
}

void RSIDock::slotConfigure()
{
    Setup setup(/* TODO: this */ 0);
    emit dialogEntered();
    if (setup.exec() == QDialog::Accepted)
      emit configChanged( !m_suspended );

    if ( !m_suspended )
      emit dialogLeft();
}

void RSIDock::slotBreakRequest()
{
    emit breakRequest();
}

void RSIDock::slotDebugRequest()
{
    emit debugRequest();
}

void RSIDock::slotSuspend()
{
    if( m_suspended )
    {
        emit suspend( false );

        setIcon( KSystemTrayIcon::loadIcon( "rsibreak0" ) );
        contextMenu()->changeItem( m_suspendItem, SmallIcon( "player_pause" ),
                                   i18n("&Suspend RSIBreak") );
    }
    else
    {
        emit suspend( true );

        setIcon( KSystemTrayIcon::loadIcon( "rsibreakx" ) );
        contextMenu()->changeItem( m_suspendItem, SmallIcon( "player_play" ),
                                   i18n( "&Resume RSIBreak" ) );
    }

    m_suspended = !m_suspended;
}

void RSIDock::showEvent( QShowEvent * )
{
// TODO: still needed?
/*    if (!m_hasQuit)
    {
        contextMenu()->insertSeparator();

        KAction* action = actionCollection()->
                action(KStandardAction::name(KStandardAction::Quit));
        if (action)
            action->plug(contextMenu());

        m_hasQuit=true;
    }*/
}

void RSIDock::mousePressEvent( QMouseEvent *e )
{
    if (e->button() == Qt::RightButton)
        contextMenu()->exec( e->globalPos() );

    if (e->button() == Qt::LeftButton)
            slotShowStatistics();
}

bool RSIDock::event ( QEvent * event )
{
    if (event->type() == QEvent::ToolTip)
    {
      emit showToolTip();
      return true;
    }
    return false;
}

void RSIDock::slotShowStatistics()
{
/* TODO:
    if ( !m_statsDialog )
    {
      m_statsDialog = new KDialogBase( this );

                                       i18n("Usage Statistics"),
                                       KDialogBase::Ok|KDialogBase::User1,
                                       KDialogBase::Ok, false,
                                       i18n("Reset"));
      m_statsWidget = new RSIStatWidget(m_statsDialog);
      connect(m_statsDialog, SIGNAL(user1Clicked()),
              this, SLOT(slotResetStats()));

      m_statsDialog->setMainWidget( m_statsWidget );
    }

    if ( m_statsDialog->isVisible() &&
         KWindowSystem::windowInfo( m_statsDialog->winId() ).desktop() == KWindowSystem::currentDesktop() )
    {
      m_statsDialog->hide();
    }
    else
    {
      m_statsDialog->show();

      if (!m_statsDialog->isActiveWindow())
        KWindowSystem::forceActiveWindow(m_statsDialog->winId());

      m_statsDialog->raise();
    }
*/
}

void RSIDock::slotResetStats()
{
    int i = KMessageBox::warningContinueCancel( /*TODO  this */ 0,
                i18n("This will reset all statistics to zero. "
                     "Is that what you want?"),
                i18n("Reset the statistics"),
                KGuiItem("Reset"),
                KStandardGuiItem::cancel(),
                "resetStatistics");

    if (i == KMessageBox::Continue)
        RSIGlobals::instance()->stats()->reset();
}

#include "rsidock.moc"
