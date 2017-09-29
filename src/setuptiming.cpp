/* ============================================================
 * Copyright (C) 2005-2007,2009-2010 by Tom Albers <toma@kde.org>
 * Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
 * Copyright (C) 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>
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
 * ============================================================ */

// Local includes.
#include "setuptiming.h"

// QT includes.
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

// KDE includes.
#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KPluralHandlingSpinBox>

class SetupTimingPriv
{
public:
    KPluralHandlingSpinBox*          tinyInterval;
    KPluralHandlingSpinBox*          tinyDuration;
    KPluralHandlingSpinBox*          tinyThreshold;
    KPluralHandlingSpinBox*          bigInterval;
    KPluralHandlingSpinBox*          bigDuration;
    KPluralHandlingSpinBox*          bigThreshold;
    KPluralHandlingSpinBox*          postponeDuration;
    int                    debug;
};

SetupTiming::SetupTiming( QWidget* parent ) : QWidget( parent )
{
    d = new SetupTimingPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // ------------------------ Tinybox

    QGroupBox *tinyBox = new QGroupBox( this );
    tinyBox->setTitle( i18n( "Tiny Breaks" ) );

    QWidget *m1_1 = new QWidget( this );
    QHBoxLayout *mHBoxLayout = new QHBoxLayout( m1_1 );
    mHBoxLayout->setMargin( 0 );
    QLabel *l1_1 = new QLabel( i18n( "Short break every:" ) + ' ', m1_1 );
    mHBoxLayout->addWidget( l1_1 );
    l1_1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1_1->setWhatsThis( i18n( "Here you can set how often you want a short break. " ) );
    d->tinyInterval = new KPluralHandlingSpinBox( m1_1 );
    mHBoxLayout->addWidget( d->tinyInterval );
    d->tinyInterval->setRange( 1, 1000 );
    l1_1->setBuddy( d->tinyInterval );
    connect( d->tinyInterval, static_cast<void ( KPluralHandlingSpinBox::* )( int )>( &KPluralHandlingSpinBox::valueChanged ), this, &SetupTiming::slotTinyValueChanged );

    QWidget *m1_2 = new QWidget( this );
    QHBoxLayout *m1_2HBoxLayout = new QHBoxLayout( m1_2 );
    m1_2HBoxLayout->setMargin( 0 );
    QLabel *l1_2 = new QLabel( i18n( "For a duration of:" ) + ' ', m1_2 );
    m1_2HBoxLayout->addWidget( l1_2 );
    l1_2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1_2->setWhatsThis( i18n( "Here you can set the duration of the short break." ) );
    d->tinyDuration = new KPluralHandlingSpinBox( m1_2 );
    m1_2HBoxLayout->addWidget( d->tinyDuration );
    d->tinyDuration->setRange( 1, 1000 );
    l1_2->setBuddy( d->tinyDuration );

    QWidget *m1_3 = new QWidget( this );
    QHBoxLayout *m1_3HBoxLayout = new QHBoxLayout( m1_3 );
    m1_3HBoxLayout->setMargin( 0 );
    QLabel *l1_3 = new QLabel( i18n( "Skip if no activity for:" ) + ' ', m1_3 );
    m1_3HBoxLayout->addWidget( l1_3 );
    l1_3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1_3->setWhatsThis( i18n( "Here you can set the duration of the short break." ) );
    d->tinyThreshold = new KPluralHandlingSpinBox( m1_3 );
    m1_3HBoxLayout->addWidget( d->tinyThreshold );
    d->tinyThreshold->setRange( 1, 1000 );
    l1_3->setBuddy( d->tinyThreshold );


    QVBoxLayout *vbox0 = new QVBoxLayout( tinyBox );
    vbox0->addWidget( m1_1 );
    vbox0->addWidget( m1_2 );
    vbox0->addWidget( m1_3 );
    vbox0->addStretch( 1 );
    tinyBox->setLayout( vbox0 );

    // ------------------------ Bigbox

    QGroupBox *bigBox = new QGroupBox( this );
    bigBox->setTitle( i18n( "Big Breaks" ) );

    QWidget *m2_1 = new QWidget( this );
    QHBoxLayout *m2_1HBoxLayout = new QHBoxLayout( m2_1 );
    m2_1HBoxLayout->setMargin( 0 );
    QLabel *l2_1 = new QLabel( i18n( "Long break every:" ) + ' ', m2_1 );
    m2_1HBoxLayout->addWidget( l2_1 );
    l2_1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2_1->setWhatsThis( i18n( "Here you can set how often you want a long break. " ) );
    d->bigInterval = new KPluralHandlingSpinBox( m2_1 );
    m2_1HBoxLayout->addWidget( d->bigInterval );
    d->bigInterval->setRange( 1, 1000 );
    l2_1->setBuddy( d->bigInterval );

    QWidget *m2_2 = new QWidget( this );
    QHBoxLayout *m2_2HBoxLayout = new QHBoxLayout( m2_2 );
    m2_2HBoxLayout->setMargin( 0 );
    QLabel *l2_2 = new QLabel( i18n( "For a duration of:" ) + ' ', m2_2 );
    m2_2HBoxLayout->addWidget( l2_2 );
    l2_2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2_2->setWhatsThis( i18n( "Here you can set the duration of the long break." ) );
    d->bigDuration = new KPluralHandlingSpinBox( m2_2 );
    m2_2HBoxLayout->addWidget( d->bigDuration );
    d->bigDuration->setRange( 1, 1000 );
    l2_2->setBuddy( d->bigDuration );

    QWidget *m2_3 = new QWidget( this );
    QHBoxLayout *m2_3HBoxLayout = new QHBoxLayout( m2_3 );
    m2_3HBoxLayout->setMargin( 0 );
    QLabel *l2_3 = new QLabel( i18n( "Skip if no activity for:" ) + ' ', m2_3 );
    m2_3HBoxLayout->addWidget( l2_3 );
    l2_3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2_3->setWhatsThis( i18n( "Here you can set the duration of the long break." ) );
    d->bigThreshold = new KPluralHandlingSpinBox( m2_3 );
    m2_3HBoxLayout->addWidget( d->bigThreshold );
    d->bigThreshold->setRange( 1, 1000 );
    l2_3->setBuddy( d->bigThreshold );

    QVBoxLayout *vbox1 = new QVBoxLayout( bigBox );
    vbox1->addWidget( m2_1 );
    vbox1->addWidget( m2_2 );
    vbox1->addWidget( m2_3 );
    vbox1->addStretch( 1 );
    bigBox->setLayout( vbox1 );

    // ------------------------ Postpone break

    QGroupBox *postponeBox = new QGroupBox( this );
    postponeBox->setTitle( i18n( "Postpone Breaks" ) );

    QWidget *m5 = new QWidget( this );
    QHBoxLayout *m5HBoxLayout = new QHBoxLayout( m5 );
    m5HBoxLayout->setMargin( 0 );
    QLabel *l5 = new QLabel( i18n( "For a duration of:" ) + ' ', m5 );
    m5HBoxLayout->addWidget( l5 );
    l5->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l5->setWhatsThis( i18n( "Here you can set for how much time you want to postpone a break." ) );
    d->postponeDuration = new KPluralHandlingSpinBox( m5 );
    m5HBoxLayout->addWidget( d->postponeDuration );
    d->postponeDuration->setRange( 1, 1000 );
    l5->setBuddy( d->postponeDuration );

    QVBoxLayout *vbox2 = new QVBoxLayout( postponeBox );
    vbox2->addWidget( m5 );
    vbox2->addStretch( 1 );
    postponeBox->setLayout( vbox2 );

    l->addWidget( tinyBox );
    l->addWidget( bigBox );
    l->addWidget( postponeBox );
    setLayout( l );
    readSettings();

    d->debug > 0 ? d->bigInterval->setSuffix( ki18np( " second", " seconds" ) )
    : d->bigInterval->setSuffix( ki18np( " minute", " minutes" ) );
    d->debug > 0 ? d->tinyInterval->setSuffix( ki18np( " second", " seconds" ) )
    : d->tinyInterval->setSuffix( ki18np( " minute", " minutes" ) );
    d->debug > 0 ? d->bigDuration->setSuffix( ki18np( " second", " seconds" ) )
    : d->bigDuration->setSuffix( ki18np( " minute", " minutes" ) );
    d->tinyDuration->setSuffix( ki18np( " second", " seconds" ) );
    d->debug ? d->postponeDuration->setSuffix( ki18np( " second", " seconds" ) )
    : d->postponeDuration->setSuffix( ki18np( " minute", " minutes" ) );

    d->tinyThreshold->setSuffix( ki18np( " second", " seconds" ) );
    d->bigThreshold->setSuffix( ki18np( " minute", " minutes" ) );

    slotTinyValueChanged( d->tinyInterval->value() );

    // Resize to minimum possible.
    d->tinyInterval->setFixedSize( d->tinyInterval->sizeHint() );
    d->bigInterval->setFixedSize( d->tinyInterval->sizeHint() );
    d->tinyDuration->setFixedSize( d->tinyInterval->sizeHint() );
    d->bigDuration->setFixedSize( d->tinyInterval->sizeHint() );
    d->tinyThreshold->setFixedSize( d->tinyThreshold->sizeHint() );
    d->bigThreshold->setFixedSize( d->bigThreshold->sizeHint() );
    d->postponeDuration->setFixedSize( d->tinyInterval->sizeHint() );
}

SetupTiming::~SetupTiming()
{
    delete d;
}

void SetupTiming::applySettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    config.writeEntry( "TinyInterval", d->tinyInterval->value() );
    config.writeEntry( "TinyDuration", d->tinyDuration->value() );
    config.writeEntry( "TinyThreshold", d->tinyThreshold->value() );
    config.writeEntry( "BigInterval", d->bigInterval->value() );
    config.writeEntry( "BigDuration", d->bigDuration->value() );
    config.writeEntry( "BigThreshold", d->bigThreshold->value() );
    config.writeEntry( "PostponeBreakDuration", d->postponeDuration->value() );
    config.sync();
}

void SetupTiming::readSettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    d->debug = config.readEntry( "DEBUG", 0 );

    d->tinyInterval->setValue( config.readEntry( "TinyInterval", 10 ) );
    d->tinyDuration->setValue( config.readEntry( "TinyDuration", 20 ) );
    d->tinyThreshold->setValue( config.readEntry( "TinyThreshold", 40 ) );
    d->tinyThreshold->setMinimum( d->tinyDuration->value() );
    d->bigInterval->setValue( config.readEntry( "BigInterval", 60 ) );
    d->bigInterval->setMinimum( d->tinyInterval->value() );
    d->bigDuration->setValue( config.readEntry( "BigDuration", 1 ) );
    d->bigThreshold->setValue( config.readEntry( "BigThreshold", 5 ) );
    d->bigThreshold->setMinimum( d->bigDuration->value() );
    d->postponeDuration->setValue( config.readEntry( "PostponeBreakDuration", 5 ) );
}

void SetupTiming::slotTinyValueChanged( int i )
{
    d->bigInterval->setMinimum( i );
}
