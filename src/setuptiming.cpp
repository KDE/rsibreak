/* ============================================================
 * Copyright (C) 2005-2007,2009-2010 by Tom Albers <toma@kde.org>
 * Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
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
#include <QVBoxLayout>
#include <QLabel>

// KDE includes.
#include <KLocale>
#include <KNumInput>
#include <KVBox>
#include <KConfigGroup>
#include <KGlobal>

class SetupTimingPriv
{
public:
    KIntNumInput*          tinyInterval;
    KIntNumInput*          tinyDuration;
    KIntNumInput*          bigInterval;
    KIntNumInput*          bigDuration;
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

    KHBox *m = new KHBox( this );
    QLabel *l1 = new QLabel( i18n( "Short break every:" ) + ' ', m );
    l1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1->setWhatsThis( i18n( "Here you can set how often you want a short break. "
                            "One minute means 60 seconds of movement with the mouse or typing on "
                            "the keyboard." ) );
    d->tinyInterval = new KIntNumInput( m );
    d->tinyInterval->setRange( 1, 1000, 1 );
    d->tinyInterval->setSliderEnabled( false );
    l1->setBuddy( d->tinyInterval );
    connect( d->tinyInterval, SIGNAL( valueChanged( int ) ),
             SLOT( slotTinyValueChanged( int ) ) );

    KHBox *m2 = new KHBox( this );
    QLabel *l2 = new QLabel( i18n( "For a duration of:" ) + ' ', m2 );
    l2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2->setWhatsThis( i18n( "Here you can set the duration of the short break." ) );
    d->tinyDuration = new KIntNumInput( m2 );
    d->tinyDuration->setRange( 1, 1000, 1 );
    d->tinyDuration->setSliderEnabled( false );
    l2->setBuddy( d->tinyDuration );

    QVBoxLayout *vbox0 = new QVBoxLayout( tinyBox );
    vbox0->addWidget( m );
    vbox0->addWidget( m2 );
    vbox0->addStretch( 1 );
    tinyBox->setLayout( vbox0 );

    // ------------------------ Bigbox

    QGroupBox *bigBox = new QGroupBox( this );
    bigBox->setTitle( i18n( "Big Breaks" ) );

    KHBox *m3 = new KHBox( this );
    QLabel *l3 = new QLabel( i18n( "Long break every:" ) + ' ', m3 );
    l3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l3->setWhatsThis( i18n( "Here you can set how often you want a long break. "
                            "One minute means 60 seconds of movement with the mouse or typing on "
                            "the keyboard" ) );
    d->bigInterval = new KIntNumInput( m3 );
    d->bigInterval->setRange( 1, 1000, 1 );
    d->bigInterval->setSliderEnabled( false );
    l3->setBuddy( d->bigInterval );

    KHBox *m4 = new KHBox( this );
    QLabel *l4 = new QLabel( i18n( "For a duration of:" ) + ' ', m4 );
    l4->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l4->setWhatsThis( i18n( "Here you can set the duration of the long break." ) );
    d->bigDuration = new KIntNumInput( m4 );
    d->bigDuration->setRange( 1, 1000, 1 );
    d->bigDuration->setSliderEnabled( false );
    l4->setBuddy( d->bigDuration );

    QVBoxLayout *vbox1 = new QVBoxLayout( bigBox );
    vbox1->addWidget( m3 );
    vbox1->addWidget( m4 );
    vbox1->addStretch( 1 );
    bigBox->setLayout( vbox1 );

    l->addWidget( tinyBox );
    l->addWidget( bigBox );
    setLayout( l );
    readSettings();

    d->debug > 0 ? d->bigInterval->setSuffix( ki18np( " second", " seconds" ) )
    : d->bigInterval->setSuffix( ki18np( " minute", " minutes" ) );
    d->debug > 0 ? d->tinyInterval->setSuffix( ki18np( " second", " seconds" ) )
    : d->tinyInterval->setSuffix( ki18np( " minute", " minutes" ) );
    d->debug > 0 ? d->bigDuration->setSuffix( ki18np( " second", " seconds" ) )
    : d->bigDuration->setSuffix( ki18np( " minute", " minutes" ) );
    d->tinyDuration->setSuffix( ki18np( " second", " seconds" ) );

    slotTinyValueChanged( d->tinyInterval->value() );

    // Resize to minimum possible.
    d->tinyInterval->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->bigInterval->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->tinyDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->bigDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
}

SetupTiming::~SetupTiming()
{
    delete d;
}

void SetupTiming::applySettings()
{
    KConfigGroup config = KGlobal::config()->group( "General Settings" );
    config.writeEntry( "TinyInterval", d->tinyInterval->value() );
    config.writeEntry( "TinyDuration", d->tinyDuration->value() );
    config.writeEntry( "BigInterval", d->bigInterval->value() );
    config.writeEntry( "BigDuration", d->bigDuration->value() );
    config.sync();
}

void SetupTiming::readSettings()
{
    KConfigGroup config = KGlobal::config()->group( "General Settings" );
    d->debug = config.readEntry( "DEBUG", 0 );

    d->tinyInterval->setValue( config.readEntry( "TinyInterval", 10 ) );
    d->tinyDuration->setValue( config.readEntry( "TinyDuration", 20 ) );
    d->bigInterval->setValue( config.readEntry( "BigInterval", 60 ) );
    d->bigInterval->setMinimum( d->tinyInterval->value() );
    d->bigDuration->setValue( config.readEntry( "BigDuration", 1 ) );
}

void SetupTiming::slotTinyValueChanged( int i )
{
    d->bigInterval->setMinimum( i );
}

#include "setuptiming.moc"
