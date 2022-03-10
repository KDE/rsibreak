/*
    SPDX-FileCopyrightText: 2005-2007 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

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
    QCheckBox*        suppressable;
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
                                           "you want to break at fixed intervals" ) );
    d->useIdleTimer = new QRadioButton( i18n( "Take activity into account" ), this );
    d->useIdleTimer->setWhatsThis( i18n( "With this option you indicate that "
                                         "you want to use idle detection. This means that only the time you are "
                                         "active (when you use the keyboard or the mouse) will be counted "
                                         "to calculate when to break next." ) );
    QVBoxLayout *vbox = new QVBoxLayout( d->breakTimerSettings );
    vbox->addWidget( d->useNoIdleTimer );
    vbox->addWidget( d->useIdleTimer );
    d->breakTimerSettings->setLayout( vbox );
    connect(d->useIdleTimer, &QRadioButton::toggled, this, &SetupGeneral::useIdleTimerChanged);

    d->suppressable = new QCheckBox(
        i18n( "&Suppress if fullscreen windows present" ), this );
    d->suppressable->setWhatsThis( i18n( "With this option you can indicate that "
                                      "you do not want RSIBreak to interfere with presentations, games, "
                                      "video playback, and any other fullscreen application on the "
                                      "current virtual desktop." ) );

    l->addWidget( d->autoStart );
    l->addWidget( d->breakTimerSettings );
    l->addWidget( d->suppressable );
    l->addStretch( 1 );

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
    config.writeEntry( "SuppressIfPresenting", d->suppressable->isChecked() );

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
    d->suppressable->setChecked( config.readEntry( "SuppressIfPresenting", true ) );
}
