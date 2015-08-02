/* ============================================================
 * Copyright 2005-2007,2010 by Tom Albers <toma@kde.org>
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
#include "setupmaximized.h"
#include "rsiwidget.h" // effects enum

// QT includes.
#include <QApplication>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QGroupBox>

// KDE includes.
#include <KLocalizedString>
#include <QVBoxLayout>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowSystem>
#include <QHBoxLayout>
#include <KPluralHandlingSpinBox>

class SetupMaximizedPriv
{
public:
    QWidget *            colorBox;
    QWidget *            fontBox;
    QGroupBox*        slideshowBox;
    QGroupBox*        plasmaBox;
    QGroupBox*        grayBox;
    QPushButton*      counterFontBut;
    QPushButton*      folderBut;
    QLineEdit*        imageFolderEdit;
    QCheckBox*        searchRecursiveCheck;
    QCheckBox*        hideMinimizeButton;
    QCheckBox*        hideLockButton;
    QCheckBox*        hidePostponeButton;    
    QCheckBox*        disableAccel;
    QPushButton*      changePathButton;
    KPluralHandlingSpinBox*     slideInterval;
    KPluralHandlingSpinBox*     popupDuration;
    QCheckBox*        usePopup;
    QCheckBox*        useFlash;
    QLabel*           useFlashLabel;
    QCheckBox*        readOnlyPlasma;
    QComboBox*        effectBox;
    QSlider*          graySlider;
    QCheckBox* showSmallImagesCheck;
};

SetupMaximized::SetupMaximized( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupMaximizedPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // Counterbox and skipbox next to eachother

    QLabel* effectLabel = new QLabel( i18n( "Choose the effect you want during breaks" ) );
    d->effectBox = new QComboBox( this );
    if ( KWindowSystem::compositingActive() )
        d->effectBox->addItem( i18n( "Simple Gray Effect" ), QVariant( RSIObject::SimpleGray ) );
    else
        d->effectBox->addItem( i18n( "Complete Black Effect" ), QVariant( RSIObject::SimpleGray ) );
    d->effectBox->addItem( i18n( "Show Plasma Dashboard" ), QVariant( RSIObject::Plasma ) );
    d->effectBox->addItem( i18n( "Show Slide Show of Images" ), QVariant( RSIObject::SlideShow ) );
    d->effectBox->addItem( i18n( "Show a Passive Popup" ), QVariant( RSIObject::Popup ) );

    connect(d->effectBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SetupMaximized::slotEffectChanged);

    //---------------- SKIP BOX
    QGroupBox *skipBox = new QGroupBox( this );
    skipBox->setTitle( i18n( "Skipping Breaks" ) );

    d->hideMinimizeButton = new QCheckBox( i18n( "&Hide skip button" ), this );
    d->hideMinimizeButton->setWhatsThis( i18n( "Check this option to disable and "
                                         "hide the skip button. This way you can prevent skipping the break." ) );

    d->hideLockButton = new QCheckBox( i18n( "&Hide lock button" ), this );
    d->hideLockButton->setWhatsThis( i18n( "Check this option to disable and "
                                           "hide the lock button. This way you can prevent pressing the wrong button." ) );
    d->hidePostponeButton = new QCheckBox( i18n( "&Hide postpone button" ), this );
    d->hidePostponeButton->setWhatsThis( i18n( "Check this option to disable and "
                                           "hide the postpone break button. This way you can prevent pressing the wrong button." ) );
    
    d->disableAccel = new QCheckBox( i18n( "&Disable shortcut" ), this );
    d->disableAccel->setWhatsThis( i18n( "Check this option to disable the skip "
                                         "shortcut. This way you can prevent skipping the break." ) );

    QVBoxLayout *vbox1 = new QVBoxLayout( skipBox );
    vbox1->addWidget( d->hideMinimizeButton );
    vbox1->addWidget( d->hideLockButton );
    vbox1->addWidget( d->hidePostponeButton );
    vbox1->addWidget( d->disableAccel );
    vbox1->addStretch( 1 );
    skipBox->setLayout( vbox1 );

    //------------------ Plasma Setup
    d->plasmaBox = new QGroupBox( this );
    d->plasmaBox->setTitle( i18n( "Plasma Dashboard" ) );

    d->readOnlyPlasma = new QCheckBox( i18n( "Read Only" ), this );

    QVBoxLayout *vbox5 = new QVBoxLayout( d->plasmaBox );
    vbox5->addWidget( d->readOnlyPlasma );
    d->plasmaBox->setLayout( vbox5 );

    //------------------ Gray Setup
    d->grayBox = new QGroupBox( this );
    d->grayBox->setTitle( i18n( "Transparency level" ) );

    QWidget* hboxslider = new QWidget( this );
    QHBoxLayout *hboxsliderHBoxLayout = new QHBoxLayout(hboxslider);
    hboxsliderHBoxLayout->setMargin(0);
    hboxsliderHBoxLayout->setSpacing( 30 );
    QLabel *lt = new QLabel( i18n( "Transparent" ), hboxslider );
    hboxsliderHBoxLayout->addWidget(lt);
    d->graySlider = new QSlider( hboxslider );
    hboxsliderHBoxLayout->addWidget(d->graySlider);
    d->graySlider->setOrientation( Qt::Horizontal );
    d->graySlider->setMinimum( 0 );
    d->graySlider->setPageStep( 10 );
    d->graySlider->setSingleStep( 5 );
    d->graySlider->setTickPosition( QSlider::TicksBelow );
    d->graySlider->setMaximum( 100 );
    QLabel *ls = new QLabel( i18n( "Solid" ), hboxslider );
    hboxsliderHBoxLayout->addWidget(ls);

    QVBoxLayout *vbox8 = new QVBoxLayout( d->grayBox );
    vbox8->addWidget( hboxslider );
    d->plasmaBox->setLayout( vbox8 );

    //------------------ PATH Setup
    d->slideshowBox = new QGroupBox( this );
    d->slideshowBox->setTitle( i18n( "Slideshow" ) );

    QWidget *imageFolderBox = new QWidget( this );
    QHBoxLayout *imageFolderBoxHBoxLayout = new QHBoxLayout(imageFolderBox);
    imageFolderBoxHBoxLayout->setMargin(0);
    d->imageFolderEdit = new QLineEdit( imageFolderBox );
    d->imageFolderEdit->setWhatsThis( i18n( "Select the folder from which you "
                                            "want to see images. These images are randomly shown during the breaks. "
                                            "It will be searched recursively if you want..." ) );
    d->changePathButton = new QPushButton( i18n( "&Change..." ),
                                           imageFolderBox );
    imageFolderBoxHBoxLayout->addWidget(d->changePathButton);
    imageFolderBoxHBoxLayout->addWidget(d->imageFolderEdit);
    d->searchRecursiveCheck = new QCheckBox( i18n( "Search path recursively" ),
            this );
    d->showSmallImagesCheck = new QCheckBox( i18n( "Show small images" ),
            this );

    QWidget *m5 = new QWidget( this );
    QHBoxLayout *m5HBoxLayout = new QHBoxLayout(m5);
    m5HBoxLayout->setMargin(0);
    QLabel *l5 = new QLabel( i18n( "Change images every:" ) + ' ', m5 );
    m5HBoxLayout->addWidget(l5);
    l5->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l5->setWhatsThis( i18n( "Here you can set how long one image should be "
                            "shown before it is replaced by the next one." ) );
    d->slideInterval = new KPluralHandlingSpinBox( m5 );
    m5HBoxLayout->addWidget(d->slideInterval);
    d->slideInterval->setRange( 3, 1000 );
    d->slideInterval->setSuffix( ki18np( " second", " seconds" ) );

    connect(d->changePathButton, &QPushButton::clicked, this, &SetupMaximized::slotFolderPicker);
    connect(d->imageFolderEdit, &QLineEdit::textChanged, this, &SetupMaximized::slotFolderEdited);

    QVBoxLayout *vboxg = new QVBoxLayout( d->slideshowBox );
    vboxg->addWidget( imageFolderBox );
    vboxg->addWidget( d->searchRecursiveCheck );
    vboxg->addWidget( d->showSmallImagesCheck );
    vboxg->addWidget( m5 );
    d->slideshowBox->setLayout( vboxg );

    //---------------- Popup setup

    QGroupBox *popupBox = new QGroupBox( this );
    popupBox->setTitle( i18n( "Popup" ) );

    QLabel *label = new QLabel( i18n( "RSIBreak can show a popup near the "
                                      "systray before replacing your whole screen with the effect chosen." ), this );
    label->setWordWrap( true );

    d->usePopup = new QCheckBox( i18n( "&Use the popup" ), this );
    connect(d->usePopup, &QCheckBox::toggled, this, &SetupMaximized::slotHideFlash);
    d->usePopup->setWhatsThis( i18n( "With this checkbox you can indicate if you "
                                     "want to see the popup when it is time to break. It will count down to "
                                     "zero, so you know how long this break will be." ) );
    label->setBuddy( d->usePopup );

    QWidget *m6 = new QWidget( this );
    QHBoxLayout *m6HBoxLayout = new QHBoxLayout(m6);
    m6HBoxLayout->setMargin(0);
    QLabel *l6 = new QLabel( i18n( "Show popup for a maximum of:" ) + ' ', m6 );
    m6HBoxLayout->addWidget(l6);
    l6->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    l6->setWhatsThis( i18n( "Here you can set how long the popup will be shown "
                            "before the effect will kick in." ) );
    d->popupDuration = new KPluralHandlingSpinBox( m6 );
    m6HBoxLayout->addWidget(d->popupDuration);
    d->popupDuration->setRange( 3, 1000 );
    d->popupDuration->setSuffix( ki18np( " second", " seconds" ) );

    d->useFlashLabel = new QLabel( '\n' + i18n( "The popup can flash when it "
                                   "detects that you are still active." ), this );
    d->useFlashLabel->setWordWrap( true );

    d->useFlash = new QCheckBox( i18n( "&Flash on activity" ), this );
    d->useFlash->setWhatsThis( i18n( "With this checkbox you can indicate "
                                     "if you want to see the popup flash when there is activity." ) );
    d->useFlashLabel->setBuddy( d->useFlashLabel );

    QVBoxLayout *vbox3 = new QVBoxLayout( popupBox );
    vbox3->addWidget( label );
    vbox3->addWidget( d->usePopup );
    vbox3->addWidget( m6 );
    vbox3->addWidget( d->useFlashLabel );
    vbox3->addWidget( d->useFlash );
    vbox3->addStretch( 1 );
    popupBox->setLayout( vbox3 );

    l->addWidget( effectLabel );
    l->addWidget( d->effectBox );
    l->addWidget( d->grayBox );
    l->addWidget( d->plasmaBox );
    l->addWidget( d->slideshowBox );
    l->addWidget( skipBox );
    l->addWidget( popupBox );
    setLayout( l );
    readSettings();
    slotHideFlash();
}

SetupMaximized::~SetupMaximized()
{
    delete d;
}

void SetupMaximized::slotEffectChanged( int current )
{
    int effect = d->effectBox->itemData( current ).toInt();
    switch ( effect ) {
    case RSIObject::SlideShow:
        d->slideshowBox->setVisible( true );
        d->plasmaBox->setVisible( false );
        d->grayBox->setVisible( false );
        break;
    case RSIObject::Plasma:
        d->slideshowBox->setVisible( false );
        d->plasmaBox->setVisible( true );
        d->grayBox->setVisible( false );
        break;
    case RSIObject::SimpleGray:
        d->slideshowBox->setVisible( false );
        d->plasmaBox->setVisible( false );
        d->grayBox->setVisible( KWindowSystem::compositingActive() );
        break;
    case RSIObject::Popup:
    default:
        d->slideshowBox->setVisible( false );
        d->plasmaBox->setVisible( false );
        break;
    }
}

void SetupMaximized::slotFolderPicker()
{
    QString  result =
        QFileDialog::getExistingDirectory( this, d->imageFolderEdit->text());

    if ( !result.isEmpty() )
        d->imageFolderEdit->setText( result );
}

void SetupMaximized::slotFolderEdited( const QString& newPath )
{
    if ( newPath.isEmpty() ) {
        d->imageFolderEdit->setText( QDir::homePath() );
        return;
    }

    if ( !newPath.startsWith( '/' ) ) {
        d->imageFolderEdit->setText( QDir::homePath() + '/' + newPath );
    }
}

void SetupMaximized::slotHideFlash()
{
    d->useFlash->setEnabled( d->usePopup->isChecked() );
    d->useFlashLabel->setEnabled( d->usePopup->isChecked() );
}

void SetupMaximized::applySettings()
{
    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    config.writeEntry( "ImageFolder", d->imageFolderEdit->text() );
    config.writeEntry( "SlideInterval", d->slideInterval->value() );
    config.writeEntry( "Patience", d->popupDuration->value() );

    config.writeEntry( "HideMinimizeButton",
                       d->hideMinimizeButton->isChecked() );
    config.writeEntry( "HideLockButton",
                       d->hideLockButton->isChecked() );
    config.writeEntry( "HidePostponeButton",
                       d->hidePostponeButton->isChecked() );    
    config.writeEntry( "SearchRecursiveCheck",
                       d->searchRecursiveCheck->isChecked() );
    config.writeEntry( "ShowSmallImagesCheck",
                       d->showSmallImagesCheck->isChecked() );
    config.writeEntry( "Effect",
                       d->effectBox->itemData( d->effectBox->currentIndex() ) );

    config.writeEntry( "DisableAccel", d->disableAccel->isChecked() );
    config.writeEntry( "UsePlasmaReadOnly",
                       d->readOnlyPlasma->isChecked() );
    config.writeEntry( "Graylevel", d->graySlider->value() );
    config = KSharedConfig::openConfig()->group( "Popup Settings" );
    config.writeEntry( "UsePopup",
                       d->usePopup->isChecked() );
    config.writeEntry( "UseFlash",
                       d->useFlash->isChecked() );

    config.sync();
}

void SetupMaximized::readSettings()
{
    QColor Black( Qt::black );
    QFont font( QApplication::font().family(), 40, 75, true );
    QString dir = QDir::home().path();

    KConfigGroup config = KSharedConfig::openConfig()->group( "General Settings" );
    d->imageFolderEdit->setText( config.readEntry( "ImageFolder", dir ) );
    d->slideInterval->setValue( config.readEntry( "SlideInterval", 10 ) );
    d->popupDuration->setValue( config.readEntry( "Patience", 30 ) );

    d->hideMinimizeButton->setChecked(
        config.readEntry( "HideMinimizeButton", false ) );
    d->hideLockButton->setChecked(
        config.readEntry( "HideLockButton", false ) );
    d->hidePostponeButton->setChecked(
        config.readEntry( "HidePostponeButton", false ) );

    const int effect = d->effectBox->findData( QVariant(
                           config.readEntry( "Effect", 0 ) ) );
    d->effectBox->setCurrentIndex( effect );
    slotEffectChanged( effect );

    d->searchRecursiveCheck->setChecked(
        config.readEntry( "SearchRecursiveCheck", false ) );
    d->showSmallImagesCheck->setChecked(
        config.readEntry( "ShowSmallImagesCheck", true ) );
    d->disableAccel->setChecked( config.readEntry( "DisableAccel", false ) );
    d->readOnlyPlasma->setChecked(
        config.readEntry( "UsePlasmaReadOnly", true ) );
    d->graySlider->setValue( config.readEntry( "Graylevel", 80 ) );
    config = KSharedConfig::openConfig()->group( "Popup Settings" );
    d->usePopup->setChecked(
        config.readEntry( "UsePopup", true ) );
    d->useFlash->setChecked(
        config.readEntry( "UseFlash", true ) );
}
