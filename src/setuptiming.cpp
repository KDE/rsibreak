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
	// TODO Check all this spinboxes are good
    KPluralHandlingSpinBox*          tinyInterval;
    KPluralHandlingSpinBox*          tinyDuration;
    KPluralHandlingSpinBox*          bigInterval;
    KPluralHandlingSpinBox*          bigDuration;
    KPluralHandlingSpinBox*          postponeDuration;
    int                    debug;
};

SetupTiming::SetupTiming( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupTimingPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // ------------------------ Tinybox

    QGroupBox *tinyBox = new QGroupBox( this );
    tinyBox->setTitle( i18n( "Tiny Breaks" ) );

    QWidget *m = new QWidget( this );
    QHBoxLayout *mHBoxLayout = new QHBoxLayout(m);
    mHBoxLayout->setMargin(0);
    QLabel *l1 = new QLabel( i18n( "Short break every:" ) + ' ', m );
    mHBoxLayout->addWidget(l1);
    l1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1->setWhatsThis( i18n( "Here you can set how often you want a short break. "
                            "One minute means 60 seconds of movement with the mouse or typing on "
                            "the keyboard." ) );
    d->tinyInterval = new KPluralHandlingSpinBox( m );
    mHBoxLayout->addWidget(d->tinyInterval);
    d->tinyInterval->setRange( 1, 1000 );
    l1->setBuddy( d->tinyInterval );
    connect(d->tinyInterval, static_cast<void (KPluralHandlingSpinBox::*)(int)>(&KPluralHandlingSpinBox::valueChanged), this, &SetupTiming::slotTinyValueChanged);

    QWidget *m2 = new QWidget( this );
    QHBoxLayout *m2HBoxLayout = new QHBoxLayout(m2);
    m2HBoxLayout->setMargin(0);
    QLabel *l2 = new QLabel( i18n( "For a duration of:" ) + ' ', m2 );
    m2HBoxLayout->addWidget(l2);
    l2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2->setWhatsThis( i18n( "Here you can set the duration of the short break." ) );
    d->tinyDuration = new KPluralHandlingSpinBox( m2 );
    m2HBoxLayout->addWidget(d->tinyDuration);
    d->tinyDuration->setRange( 1, 1000 );
    l2->setBuddy( d->tinyDuration );

    QVBoxLayout *vbox0 = new QVBoxLayout( tinyBox );
    vbox0->addWidget( m );
    vbox0->addWidget( m2 );
    vbox0->addStretch( 1 );
    tinyBox->setLayout( vbox0 );

    // ------------------------ Bigbox

    QGroupBox *bigBox = new QGroupBox( this );
    bigBox->setTitle( i18n( "Big Breaks" ) );

    QWidget *m3 = new QWidget( this );
    QHBoxLayout *m3HBoxLayout = new QHBoxLayout(m3);
    m3HBoxLayout->setMargin(0);
    QLabel *l3 = new QLabel( i18n( "Long break every:" ) + ' ', m3 );
    m3HBoxLayout->addWidget(l3);
    l3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l3->setWhatsThis( i18n( "Here you can set how often you want a long break. "
                            "One minute means 60 seconds of movement with the mouse or typing on "
                            "the keyboard" ) );
    d->bigInterval = new KPluralHandlingSpinBox( m3 );
    m3HBoxLayout->addWidget(d->bigInterval);
    d->bigInterval->setRange( 1, 1000 );
    l3->setBuddy( d->bigInterval );

    QWidget *m4 = new QWidget( this );
    QHBoxLayout *m4HBoxLayout = new QHBoxLayout(m4);
    m4HBoxLayout->setMargin(0);
    QLabel *l4 = new QLabel( i18n( "For a duration of:" ) + ' ', m4 );
    m4HBoxLayout->addWidget(l4);
    l4->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l4->setWhatsThis( i18n( "Here you can set the duration of the long break." ) );
    d->bigDuration = new KPluralHandlingSpinBox( m4 );
    m4HBoxLayout->addWidget(d->bigDuration);
    d->bigDuration->setRange( 1, 1000 );
    l4->setBuddy( d->bigDuration );

    QVBoxLayout *vbox1 = new QVBoxLayout( bigBox );
    vbox1->addWidget( m3 );
    vbox1->addWidget( m4 );
    vbox1->addStretch( 1 );
    bigBox->setLayout( vbox1 );

    // ------------------------ Postpone break
    
    QGroupBox *postponeBox = new QGroupBox( this );
    postponeBox->setTitle( i18n( "Postpone Breaks" ) );

    QWidget *m5 = new QWidget( this );
    QHBoxLayout *m5HBoxLayout = new QHBoxLayout(m5);
    m5HBoxLayout->setMargin(0);
    QLabel *l5 = new QLabel( i18n( "For a duration of:" ) + ' ', m5 );
    m5HBoxLayout->addWidget(l5);
    l5->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l5->setWhatsThis( i18n( "Here you can set for how much time you want to postpone a break." ) );
    d->postponeDuration = new KPluralHandlingSpinBox( m5 );
    m5HBoxLayout->addWidget(d->postponeDuration);
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

    slotTinyValueChanged( d->tinyInterval->value() );

    // Resize to minimum possible.
    d->tinyInterval->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->bigInterval->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->tinyDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->bigDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->postponeDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
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
    config.writeEntry( "BigInterval", d->bigInterval->value() );
    config.writeEntry( "BigDuration", d->bigDuration->value() );
    config.writeEntry( "PostponeBreakDuration", d->postponeDuration->value() );
    config.sync();
}

void SetupTiming::readSettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    d->debug = config.readEntry( "DEBUG", 0 );

    d->tinyInterval->setValue( config.readEntry( "TinyInterval", 10 ) );
    d->tinyDuration->setValue( config.readEntry( "TinyDuration", 20 ) );
    d->bigInterval->setValue( config.readEntry( "BigInterval", 60 ) );
    d->bigInterval->setMinimum( d->tinyInterval->value() );
    d->bigDuration->setValue( config.readEntry( "BigDuration", 1 ) );
    d->postponeDuration->setValue( config.readEntry( "PostponeBreakDuration", 5 ) );
}

void SetupTiming::slotTinyValueChanged( int i )
{
    d->bigInterval->setMinimum( i );
}
