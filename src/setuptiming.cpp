/*
    SPDX-FileCopyrightText: 2005-2007, 2009-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

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
    QGroupBox*                       tinyBox;
    KPluralHandlingSpinBox*          tinyInterval;
    KPluralHandlingSpinBox*          tinyDuration;
    KPluralHandlingSpinBox*          tinyThreshold;
    KPluralHandlingSpinBox*          bigInterval;
    KPluralHandlingSpinBox*          bigDuration;
    KPluralHandlingSpinBox*          bigThreshold;
    KPluralHandlingSpinBox*          postponeDuration;
    int                    debug;
};

SetupTiming::SetupTiming( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupTimingPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // ------------------------ Tinybox

    d->tinyBox = new QGroupBox( this );
    d->tinyBox->setTitle( i18n( "Tiny Breaks" ) );
    d->tinyBox->setCheckable( true );
    connect( d->tinyBox, &QGroupBox::toggled, this, &SetupTiming::slotTinyValueEnabled );

    QWidget *m = new QWidget( this );
    QHBoxLayout *mHBoxLayout = new QHBoxLayout(m);
    mHBoxLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *l1 = new QLabel( i18n( "Short break every:" ) + ' ', m );
    mHBoxLayout->addWidget(l1);
    l1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l1->setWhatsThis( i18n( "Here you can set how often you want a short break. " ) );
    d->tinyInterval = new KPluralHandlingSpinBox( m );
    mHBoxLayout->addWidget(d->tinyInterval);
    d->tinyInterval->setRange( 1, 1000 );
    l1->setBuddy( d->tinyInterval );
    connect(d->tinyInterval, static_cast<void (KPluralHandlingSpinBox::*)(int)>(&KPluralHandlingSpinBox::valueChanged), this, &SetupTiming::slotTinyValueChanged);

    QWidget *m2 = new QWidget( this );
    QHBoxLayout *m2HBoxLayout = new QHBoxLayout(m2);
    m2HBoxLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *l2 = new QLabel( i18n( "For a duration of:" ) + ' ', m2 );
    m2HBoxLayout->addWidget(l2);
    l2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l2->setWhatsThis( i18n( "Here you can set the duration of the short break." ) );
    d->tinyDuration = new KPluralHandlingSpinBox( m2 );
    m2HBoxLayout->addWidget(d->tinyDuration);
    d->tinyDuration->setRange( 1, 1000 );
    l2->setBuddy( d->tinyDuration );
    connect( d->tinyDuration,  static_cast<void ( KPluralHandlingSpinBox::* )( int )>( &KPluralHandlingSpinBox::valueChanged ),
             this, &SetupTiming::slotTinyDurationValueChanged );

    QWidget *mTinyThreshold = new QWidget( this );
    QHBoxLayout *hTinyThreshold = new QHBoxLayout( mTinyThreshold );
    hTinyThreshold->setContentsMargins( 0, 0, 0, 0 );
    QLabel *lTinyThreshold = new QLabel( i18n( "Skip if no activity for:" ) + ' ', mTinyThreshold );
    hTinyThreshold->addWidget( lTinyThreshold );
    lTinyThreshold->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    lTinyThreshold->setWhatsThis( i18n( "Set the interval you are idle for that is enough to skip the next short break." ) );
    d->tinyThreshold = new KPluralHandlingSpinBox( mTinyThreshold );
    hTinyThreshold->addWidget( d->tinyThreshold );
    d->tinyThreshold->setRange( 1, 1000 );
    lTinyThreshold->setBuddy( d->tinyThreshold );

    QVBoxLayout *vbox0 = new QVBoxLayout( d->tinyBox );
    vbox0->addWidget( m );
    vbox0->addWidget( m2 );
    vbox0->addWidget( mTinyThreshold );
    vbox0->addStretch( 1 );
    d->tinyBox->setLayout( vbox0 );

    // ------------------------ Bigbox

    QGroupBox *bigBox = new QGroupBox( this );
    bigBox->setTitle( i18n( "Big Breaks" ) );

    QWidget *m3 = new QWidget( this );
    QHBoxLayout *m3HBoxLayout = new QHBoxLayout(m3);
    m3HBoxLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *l3 = new QLabel( i18n( "Long break every:" ) + ' ', m3 );
    m3HBoxLayout->addWidget(l3);
    l3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l3->setWhatsThis( i18n( "Here you can set how often you want a long break. " ) );
    d->bigInterval = new KPluralHandlingSpinBox( m3 );
    m3HBoxLayout->addWidget(d->bigInterval);
    d->bigInterval->setRange( 1, 1000 );
    l3->setBuddy( d->bigInterval );

    QWidget *m4 = new QWidget( this );
    QHBoxLayout *m4HBoxLayout = new QHBoxLayout(m4);
    m4HBoxLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *l4 = new QLabel( i18n( "For a duration of:" ) + ' ', m4 );
    m4HBoxLayout->addWidget(l4);
    l4->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l4->setWhatsThis( i18n( "Here you can set the duration of the long break." ) );
    d->bigDuration = new KPluralHandlingSpinBox( m4 );
    m4HBoxLayout->addWidget(d->bigDuration);
    d->bigDuration->setRange( 1, 1000 );
    l4->setBuddy( d->bigDuration );
    connect( d->bigDuration,  static_cast<void ( KPluralHandlingSpinBox::* )( int )>( &KPluralHandlingSpinBox::valueChanged ),
             this, &SetupTiming::slotBigDurationValueChanged );

    QWidget *mBigThreshold = new QWidget( this );
    QHBoxLayout *hBigThreshold = new QHBoxLayout( mBigThreshold );
    hBigThreshold->setContentsMargins( 0, 0, 0, 0 );
    QLabel *lBigThreshold = new QLabel( i18n( "Skip if no activity for:" ) + ' ', mBigThreshold );
    hBigThreshold->addWidget( lBigThreshold );
    lBigThreshold->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    lBigThreshold->setWhatsThis( i18n( "Set the interval you are idle for that is enough to skip the next long break." ) );
    d->bigThreshold = new KPluralHandlingSpinBox( mBigThreshold );
    hBigThreshold->addWidget( d->bigThreshold );
    d->bigThreshold->setRange( 1, 1000 );
    lBigThreshold->setBuddy( d->bigThreshold );

    QVBoxLayout *vbox1 = new QVBoxLayout( bigBox );
    vbox1->addWidget( m3 );
    vbox1->addWidget( m4 );
    vbox1->addWidget( mBigThreshold );
    vbox1->addStretch( 1 );
    bigBox->setLayout( vbox1 );

    // ------------------------ Postpone break
    
    QGroupBox *postponeBox = new QGroupBox( this );
    postponeBox->setTitle( i18n( "Postpone Breaks" ) );

    QWidget *m5 = new QWidget( this );
    QHBoxLayout *m5HBoxLayout = new QHBoxLayout(m5);
    m5HBoxLayout->setContentsMargins(0, 0, 0, 0);
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
    
    l->addWidget( d->tinyBox );
    l->addWidget( bigBox );
    l->addWidget( postponeBox );
    setLayout( l );
    readSettings();

    KLocalizedString sfx = ki18np( " second", " seconds" );
    d->tinyDuration->setSuffix( sfx );
    d->tinyThreshold->setSuffix( sfx );
    if ( !d->debug ) {
        sfx = ki18np( " minute", " minutes" );
    }
    d->tinyInterval->setSuffix( sfx );
    d->bigInterval->setSuffix( sfx );
    d->bigDuration->setSuffix( sfx );
    d->bigThreshold->setSuffix( sfx );
    d->postponeDuration->setSuffix( sfx );

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
    config.writeEntry( "TinyEnabled", d->tinyBox->isChecked() );
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

    d->tinyBox->setChecked( config.readEntry( "TinyEnabled", true ) );
    d->tinyInterval->setValue( config.readEntry( "TinyInterval", 10 ) );
    d->tinyDuration->setValue( config.readEntry( "TinyDuration", 20 ) );
    d->tinyThreshold->setValue( config.readEntry( "TinyThreshold", 40 ) );
    d->bigInterval->setValue( config.readEntry( "BigInterval", 60 ) );
    d->bigDuration->setValue( config.readEntry( "BigDuration", 1 ) );
    d->bigThreshold->setValue( config.readEntry( "BigThreshold", 5 ) );
    d->postponeDuration->setValue( config.readEntry( "PostponeBreakDuration", 5 ) );
}

void SetupTiming::slotTinyValueEnabled( bool enabled )
{
    d->bigInterval->setMinimum( enabled ? d->tinyInterval->value() : 1 );
}

void SetupTiming::slotTinyValueChanged( const int tinyIntervalValue  )
{
    if ( d->tinyBox->isChecked() ) {
        d->bigInterval->setMinimum( tinyIntervalValue );
    }
}

void SetupTiming::slotBigDurationValueChanged( const int bigDurationValue )
{
    d->bigThreshold->setMinimum( bigDurationValue );
}

void SetupTiming::slotTinyDurationValueChanged( const int tinyDurationValue )
{
    d->tinyThreshold->setMinimum( tinyDurationValue );
}

void SetupTiming::slotSetUseIdleTimer( const bool useIdleTimer )
{
    d->bigThreshold->setEnabled( useIdleTimer );
    d->tinyThreshold->setEnabled( useIdleTimer );
}
