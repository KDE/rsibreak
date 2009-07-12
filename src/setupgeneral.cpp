/* ============================================================
 * Copyright 2005-2007 by Tom Albers <toma@kde.org>
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
 *
 * ============================================================ */

// Local includes.

#include "setupgeneral.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>

#include <KLocale>
#include <KConfig>
#include <KVBox>
#include <KGlobal>
#include <KConfigGroup>

class SetupGeneralPriv
{
public:
    QCheckBox*        autoStart;
    QGroupBox*        breakTimerSettings;
    QRadioButton*     useNoIdleTimer;
    QRadioButton*     useIdleTimer;
    QGroupBox*        idleSettings;
    QCheckBox*        resetTimersAfterIdle;
    QCheckBox*        ignoreIdleForTinyBreaks;
    QCheckBox*        showTimerReset;
};

SetupGeneral::SetupGeneral( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupGeneralPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    d->autoStart = new QCheckBox(
        i18n( "&Automatically start RSIBreak at startup" ), this );
    d->autoStart->setWhatsThis( i18n( "With this option you can indicate that "
                                      "you want RSIBreak to start when KDE starts." ) );


    d->breakTimerSettings = new QGroupBox( i18n( "Break Timer Settings" ), this );
    d->useNoIdleTimer = new QRadioButton( i18n( "Break at &fixed times and "
                                          "ignore movement during breaks" ), this );
    d->useNoIdleTimer->setWhatsThis( i18n( "With this option you indicate that "
                                           "you want to ignore movements during breaks and want to break at fixed "
                                           "intervals" ) );
    d->useIdleTimer = new QRadioButton( i18n( "Take into account the &idle "
                                        "settings" ), this );
    d->useIdleTimer->setWhatsThis( i18n( "With this option you indicate that "
                                         "you want to use idle detection. This means that only the time you are "
                                         "active (when you use the keyboard or the mouse) will be counted "
                                         "when evaluating the position of the next break." ) );
    QVBoxLayout *vbox = new QVBoxLayout( d->breakTimerSettings );
    vbox->addWidget( d->useNoIdleTimer );
    vbox->addWidget( d->useIdleTimer );
    vbox->addStretch( 1 );
    d->breakTimerSettings->setLayout( vbox );
    connect( d->useNoIdleTimer , SIGNAL( toggled( bool ) ),
             SLOT( slotUseNoIdleTimer() ) );

    // ---------------- IDLE Settings ------------------------

    d->idleSettings = new QGroupBox( i18n( "Idle Settings" ), this );

    d->resetTimersAfterIdle = new QCheckBox(
        i18n( "&Reset timers after period of idleness" ), this );
    d->resetTimersAfterIdle->setWhatsThis( i18n( "With this checkbox you "
                                           "indicate that you want the timers for the next break to be reset if "
                                           "you are sufficiently idle (you are idle more that the duration of "
                                           "the corresponding break)" ) );
    d->showTimerReset = new QCheckBox( i18n( "&Show when timers are reset" ),
                                       this );
    d->showTimerReset->setWhatsThis( i18n( "With this checkbox you indicate "
                                           "that you want to see when the timers are reset. This happens when you "
                                           "have been idle for a while." ) );

    d->ignoreIdleForTinyBreaks = new QCheckBox( i18n( "&Ignore idle detection "
            "for tiny breaks" ), this );
    d->ignoreIdleForTinyBreaks->setWhatsThis(
        i18n( "With this option you select that the <b>short</b> breaks "
              "should not be reset when idle. This is useful, for example, "
              "when you are reading something and you do not touch the mouse "
              "or keyboard for the duration of a short break, but you still "
              "want to have a short break every once in a while." ) );
    QVBoxLayout *vbox2 = new QVBoxLayout( d->idleSettings );
    vbox2->addWidget( d->resetTimersAfterIdle );
    vbox2->addWidget( d->showTimerReset );
    vbox2->addWidget( d->ignoreIdleForTinyBreaks );
    vbox2->addStretch( 1 );
    d->idleSettings->setLayout( vbox2 );
    connect( d->resetTimersAfterIdle , SIGNAL( toggled( bool ) ),
             SLOT( slotShowTimer() ) );
    l->addWidget( d->autoStart );
    l->addWidget( d->breakTimerSettings );
    l->addWidget( d->idleSettings );

    setLayout( l );
    readSettings();
    slotUseNoIdleTimer();
}

SetupGeneral::~SetupGeneral()
{
    delete d;
}

void SetupGeneral::slotShowTimer()
{
    d->showTimerReset->setEnabled( d->resetTimersAfterIdle->isChecked() &&
                                   !d->useNoIdleTimer->isChecked() );
    d->ignoreIdleForTinyBreaks->setEnabled( d->resetTimersAfterIdle->isChecked() &&
                                            !d->useNoIdleTimer->isChecked() );
}

void SetupGeneral::slotUseNoIdleTimer()
{
    d->idleSettings->setEnabled( !d->useNoIdleTimer->isChecked() );
    d->resetTimersAfterIdle->setEnabled( !d->useNoIdleTimer->isChecked() );
    d->ignoreIdleForTinyBreaks->setEnabled( !d->useNoIdleTimer->isChecked() &&
                                            d->resetTimersAfterIdle->isChecked() );
    d->showTimerReset->setEnabled( !d->useNoIdleTimer->isChecked() &&
                                   d->resetTimersAfterIdle->isChecked() );
    slotShowTimer();
}

void SetupGeneral::applySettings()
{
    KConfigGroup config = KGlobal::config()->group( "General" );
    config.writeEntry( "AutoStart", d->autoStart->isChecked() );

    config = KGlobal::config()->group( "General Settings" );
    config.writeEntry( "ResetTimersAfterIdle", d->resetTimersAfterIdle->isChecked() );
    config.writeEntry( "ShowTimerReset", d->showTimerReset->isChecked() );
    config.writeEntry( "IgnoreIdleForTinyBreaks", d->ignoreIdleForTinyBreaks->isChecked() );
    config.writeEntry( "UseNoIdleTimer", d->useNoIdleTimer->isChecked() );
    config.sync();
}

void SetupGeneral::readSettings()
{
    KConfigGroup config = KGlobal::config()->group( "General" );
    d->autoStart->setChecked( config.readEntry( "AutoStart", false ) );

    config = KGlobal::config()->group( "General Settings" );
    d->resetTimersAfterIdle->setChecked( config.readEntry( "ResetTimersAfterIdle", true ) );
    d->showTimerReset->setChecked( config.readEntry( "ShowTimerReset", false ) );
    d->ignoreIdleForTinyBreaks->setChecked( config.readEntry( "IgnoreIdleForTinyBreaks", false ) );
    d->useNoIdleTimer->setChecked( config.readEntry( "UseNoIdleTimer", false ) );
    d->useIdleTimer->setChecked( !d->useNoIdleTimer->isChecked() );
}

#include "setupgeneral.moc"
