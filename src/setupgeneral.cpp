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

#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class SetupGeneralPriv
{
public:
    QCheckBox*        autoStart;
    QGroupBox*        breakTimerSettings;
    QRadioButton*     useNoIdleTimer;
    QRadioButton*     useIdleTimer;
};

SetupGeneral::SetupGeneral( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupGeneralPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    d->autoStart = new QCheckBox(
        i18n( "&Automatically start RSIBreak at startup" ), this );
    d->autoStart->setWhatsThis( i18n( "With this option you can indicate that "
                                      "you want RSIBreak to start on Desktop Environment start." ) );


    d->breakTimerSettings = new QGroupBox( i18n( "Timer Settings" ), this );
    d->useNoIdleTimer = new QRadioButton( i18n( "Break at &fixed times" ), this );
    d->useNoIdleTimer->setWhatsThis( i18n( "With this option you indicate that "
                                           "you want to to break at fixed intervals" ) );
    d->useIdleTimer = new QRadioButton( i18n( "Take activity into account" ), this );
    d->useIdleTimer->setWhatsThis( i18n( "With this option you indicate that "
                                         "you want to use idle detection. This means that only the time you are "
                                         "active (when you use the keyboard or the mouse) will be counted "
                                         "to calculate when to break next." ) );
    QVBoxLayout *vbox = new QVBoxLayout( d->breakTimerSettings );
    vbox->addWidget( d->useNoIdleTimer );
    vbox->addWidget( d->useIdleTimer );
    vbox->addStretch( 1 );
    d->breakTimerSettings->setLayout( vbox );
    connect(d->useIdleTimer, &QRadioButton::toggled, this, &SetupGeneral::useIdleTimerChanged);

    l->addWidget( d->autoStart );
    l->addWidget( d->breakTimerSettings );

    setLayout( l );
    readSettings();
}

SetupGeneral::~SetupGeneral()
{
    delete d;
}

void SetupGeneral::applySettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General" );
    config.writeEntry( "AutoStart", d->autoStart->isChecked() );

    config = KSharedConfig::openConfig()->group( "General Settings" );
    config.writeEntry( "UseNoIdleTimer", d->useNoIdleTimer->isChecked() );
    config.sync();
}

bool SetupGeneral::useIdleTimer() const
{
    return d->useIdleTimer->isChecked();
}

void SetupGeneral::readSettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General" );
    d->autoStart->setChecked( config.readEntry( "AutoStart", false ) );

    config = KSharedConfig::openConfig()->group( "General Settings" );
    d->useNoIdleTimer->setChecked( config.readEntry( "UseNoIdleTimer", false ) );
    d->useIdleTimer->setChecked( !d->useNoIdleTimer->isChecked() );
}
