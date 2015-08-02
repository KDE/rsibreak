/*
   Copyright (C) 2005-2006 Tom Albers <toma@kde.org>
   Copyright (C) 2011 Aurélien Gâteau <agateau@kde.org>

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

#include <QPointer>
#include <QTextDocument>

#include <KAboutData>
#include <KLocalizedString>
#include <KIconLoader>
#include <QMenu>
#include <KNotifyConfigWidget>
#include <KHelpMenu>
#include <KStandardShortcut>
#include <KMessageBox>
#include <KWindowSystem>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

RSIDock::RSIDock( QObject *parent )
        : KStatusNotifierItem( parent ), m_suspended( false )
        , m_statsDialog( 0 ), m_statsWidget( 0 )
{
    setCategory(ApplicationStatus);
    setStatus(Active);

    const KAboutData &aboutData = KAboutData::applicationData();
    setTitle( aboutData.displayName() );
    setToolTipTitle( aboutData.displayName() );

    m_help = new KHelpMenu( 0, aboutData );

    QMenu* menu = contextMenu();
    qDebug() << "NNNNNNNNNE" << menu;
    menu->addAction( QIcon::fromTheme( "kde" ), i18n( "About &KDE" ), m_help,
                     SLOT( aboutKDE() ) );
    menu->addAction( i18n( "&About RSIBreak" ), m_help,
                     SLOT( aboutApplication() ) );
    menu->addAction( QIcon::fromTheme( "help-contents" ),
                     i18n( "RSIBreak &Handbook" ), m_help, SLOT( appHelpActivated() ),
                     KStandardShortcut::shortcut( KStandardShortcut::Help )[0] );

    menu->addSeparator();
    menu->addAction( QIcon::fromTheme( "tools-report-bug" ), i18n( "&Report Bug..." ), m_help, SLOT( reportBug() ) );
    menu->addAction( i18n( "Switch application &language..." ), m_help,
                     SLOT( switchApplicationLanguage() ) );

    menu->addSeparator();
    m_suspendItem = menu->addAction( SmallIcon( "media-playback-pause" ),
                                     i18n( "&Suspend RSIBreak" ), this,
                                     SLOT( slotSuspend() ) );
    menu->addAction( SmallIcon( "view-statistics" ),
                     i18n( "&Usage Statistics" ), this,
                     SLOT( slotShowStatistics() ) );
    menu->addAction( SmallIcon( "preferences-desktop-notification" ),
                     i18n( "Configure &Notifications..." ), this,
                     SLOT( slotConfigureNotifications() ) );
    menu->addAction( QIcon::fromTheme( "configure" ), i18n( "&Configure RSIBreak..." ),
                     this, SLOT( slotConfigure() ) );

    connect(this, &RSIDock::activateRequested, this, &RSIDock::slotShowStatistics);
}

RSIDock::~RSIDock()
{
    delete m_help;
    delete m_statsWidget;
    delete m_statsDialog;
    m_statsWidget = 0;
}

void RSIDock::slotConfigureNotifications()
{
    KNotifyConfigWidget::configure( 0 );
}

void RSIDock::slotConfigure()
{
    // don't think it is needed, because setup is not accessed after the
    // exec call, but better safe than crash.
    QPointer<Setup> setup = new Setup( 0 );
    emit dialogEntered();
    if ( setup->exec() == QDialog::Accepted )
        emit configChanged( !m_suspended );
    delete setup;

    if ( !m_suspended )
        emit dialogLeft();
}

void RSIDock::slotBreakRequest()
{
    emit breakRequest();
}

void RSIDock::slotSuspend()
{
    if ( m_suspended ) {
        emit suspend( false );

        setIconByName( "rsibreak0" );
        m_suspendItem->setIcon( SmallIcon( "media-playback-pause" ) );
        m_suspendItem->setText( i18n( "&Suspend RSIBreak" ) );
    } else {
        emit suspend( true );

        setIconByName( "rsibreakx" );
        m_suspendItem->setIcon( SmallIcon( "media-playback-start" ) );
        m_suspendItem->setText( i18n( "&Resume RSIBreak" ) );
    }

    m_suspended = !m_suspended;
}

void RSIDock::slotShowStatistics()
{
    if ( !m_statsDialog ) {
        m_statsDialog = new QDialog( 0 );
        m_statsDialog->setWindowTitle( i18n( "Usage Statistics" ) );
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        QWidget *mainWidget = new QWidget(m_statsDialog);
        QVBoxLayout *mainLayout = new QVBoxLayout;
        m_statsDialog->setLayout(mainLayout);
        mainLayout->addWidget(mainWidget);
        QPushButton *user1Button = new QPushButton;
        buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
        connect(buttonBox, &QDialogButtonBox::accepted, m_statsDialog, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, m_statsDialog, &QDialog::reject);
        
        user1Button->setText(i18n( "Reset"  ));

        m_statsWidget = new RSIStatWidget( m_statsDialog );
        connect(user1Button, &QPushButton::clicked, this, &RSIDock::slotResetStats);

        mainLayout->addWidget(m_statsWidget);
        mainLayout->addWidget(buttonBox);
    }

    if ( m_statsDialog->isVisible() &&
            KWindowSystem::windowInfo(
                m_statsDialog->winId(), NET::CurrentDesktop ).desktop() ==
            KWindowSystem::currentDesktop() ) {
        m_statsDialog->hide();
    } else {
        m_statsDialog->show();

        if ( !m_statsDialog->isActiveWindow() )
            KWindowSystem::forceActiveWindow( m_statsDialog->winId() );

        m_statsDialog->raise();
    }
}

void RSIDock::slotResetStats()
{
    int i = KMessageBox::warningContinueCancel( 0,
            i18n( "This will reset all statistics to zero. "
                  "Is that what you want?" ),
            i18n( "Reset the statistics" ),
            KGuiItem( i18n( "Reset" ) ),
            KStandardGuiItem::cancel(),
            "resetStatistics" );

    if ( i == KMessageBox::Continue )
        RSIGlobals::instance()->stats()->reset();
}

static QString colorizedText( const QString& text, const QColor& color )
{
    return QString("<font color='%1'>&#9679;</font> %2")
        .arg(color.name())
        .arg(text.toHtmlEscaped())
        ;
}

void RSIDock::setCounters( int tiny_left, int big_left )
{
    if ( m_suspended )
        setToolTipSubTitle( i18n( "Suspended" ) );
    else {
        QColor tinyColor = RSIGlobals::instance()->getTinyBreakColor( tiny_left );
        RSIGlobals::instance()->stats()->setColor( LAST_TINY_BREAK, tinyColor );

        QColor bigColor = RSIGlobals::instance()-> getBigBreakColor( big_left );
        RSIGlobals::instance()->stats()->setColor( LAST_BIG_BREAK, bigColor );

        // Only add the line for the tiny break when there is not
        // a big break planned at the same time.

        QStringList lines;
        if ( tiny_left != big_left ) {
            QString formattedText = RSIGlobals::instance()->formatSeconds( tiny_left );
            if ( !formattedText.isNull() ) {
                lines << colorizedText(
                    i18n( "%1 remaining until next short break", formattedText ),
                    tinyColor
                    );
            }
        }

        // do the same for the big break
        if ( big_left > 0 )
            lines << colorizedText(
                i18n( "%1 remaining until next long break",
                    RSIGlobals::instance()->formatSeconds( big_left ) ),
                bigColor
                );
        setToolTipSubTitle(lines.join("<br>"));
    }
}
