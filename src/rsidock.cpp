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

#include <QMenu>

#include <KComponentData>
#include <KLocale>
#include <KNotifyConfigWidget>
#include <KIconLoader>
#include <KHelpMenu>
#include <KGlobalAccel>
#include <KStandardShortcut>
#include <KMessageBox>
#include <KWindowSystem>

RSIDock::RSIDock( QWidget *parent)
    : KSystemTrayIcon( parent ), m_suspended( false )
    , m_statsDialog( 0 ), m_statsWidget( 0 )
{
    m_help = new KHelpMenu( parent, KGlobal::mainComponent().aboutData());

    QMenu* menu = new QMenu(parent);
    menu->addAction( KIcon("about-kde"), i18n( "About &KDE" ), m_help,
                SLOT( aboutKDE() ) );
    menu->addAction( i18n( "&About RSIBreak"), m_help,
                SLOT( aboutApplication() ) );
    menu->addAction( KIcon("help-contents"),
                i18n("RSIBreak &Handbook"), m_help, SLOT(appHelpActivated()),
                KStandardShortcut::shortcut(KStandardShortcut::Help).primary());

    menu->addSeparator();
    menu->addAction( i18n( "&Report Bug..." ), m_help, SLOT(reportBug()) );
    menu->addAction( i18n( "Switch application &language..." ), m_help,
                SLOT(switchApplicationLanguage()) );

    menu->addSeparator();
    m_suspendItem = menu->addAction(SmallIcon("media-playback-pause"),
                              i18n("&Suspend RSIBreak"), this,
                              SLOT(slotSuspend()));
    menu->addAction(i18n("&Usage Statistics"),
                    this, SLOT( slotShowStatistics() ) );
    menu->addAction(SmallIcon("knotify"), i18n("Configure &Notifications..."),
                    this, SLOT(slotConfigureNotifications()));
    menu->addAction(KIcon("configure"), i18n("&Configure RSIBreak..."),
                    this, SLOT(slotConfigure()));

    menu->addSeparator();
    menu->addAction(KIcon("application-exit"),i18n("Quit"), this, SLOT(slotQuit()),
                    KStandardShortcut::shortcut(KStandardShortcut::Quit).primary());


    setContextMenu(menu);

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));

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
    delete m_statsDialog;
    m_statsWidget = 0;
}

void RSIDock::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
      slotShowStatistics();
}

void RSIDock::slotConfigureNotifications()
{
    KNotifyConfigWidget::configure(0);
}

void RSIDock::slotConfigure()
{
    Setup setup(0);
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
        m_suspendItem->setIcon(SmallIcon( "media-playback-pause" ) );
        m_suspendItem->setText(i18n("&Suspend RSIBreak") );
    }
    else
    {
        emit suspend( true );

        setIcon( KSystemTrayIcon::loadIcon( "rsibreakx" ) );
        m_suspendItem->setIcon(SmallIcon( "media-playback-start" ) );
        m_suspendItem->setText(i18n( "&Resume RSIBreak" ) );
    }

    m_suspended = !m_suspended;
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
    kDebug() << event->type() << endl;
    if (event->type() == QEvent::ToolTip)
    {
      emit showToolTip();
      return true;
    }
    return false;
}

void RSIDock::slotShowStatistics()
{
    if ( !m_statsDialog )
    {
      m_statsDialog = new KDialog( 0 );
      m_statsDialog->setCaption( i18n("Usage Statistics") );
      m_statsDialog->setButtons( KDialog::Ok | KDialog::User1 );
      m_statsDialog->setButtonText( KDialog::User1, i18n("Reset"));

      m_statsWidget = new RSIStatWidget(m_statsDialog);
      connect(m_statsDialog, SIGNAL(user1Clicked()),
              this, SLOT(slotResetStats()));

      m_statsDialog->setMainWidget( m_statsWidget );
    }

    if ( m_statsDialog->isVisible() &&
         KWindowSystem::windowInfo(
                  m_statsDialog->winId(),NET::CurrentDesktop ).desktop() ==
                  KWindowSystem::currentDesktop() )
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
}

void RSIDock::slotResetStats()
{
    int i = KMessageBox::warningContinueCancel( 0,
                i18n("This will reset all statistics to zero. "
                     "Is that what you want?"),
                i18n("Reset the statistics"),
                KGuiItem("Reset"),
                KStandardGuiItem::cancel(),
                "resetStatistics");

    if (i == KMessageBox::Continue)
        RSIGlobals::instance()->stats()->reset();
}

void RSIDock::slotQuit()
{
  exit(0);
}

#include "rsidock.moc"
