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
 * ============================================================ */

// Local includes.
#include "setupmaximized.h"
#include "rsiwidget.h" // effects enum

// QT includes.
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>

// KDE includes.
#include <KComboBox>
#include <KLocale>
#include <KApplication>
#include <KFileDialog>
#include <KLineEdit>
#include <KVBox>
#include <KWindowSystem>
#include <kkeysequencewidget.h>

class SetupMaximizedPriv
{
public:
    KHBox*            colorBox;
    KHBox*            fontBox;
    QGroupBox*        slideshowBox;
    QGroupBox*        plasmaBox;
    QGroupBox*        grayBox;
    QPushButton*      counterFontBut;
    QPushButton*      folderBut;
    KLineEdit*        imageFolderEdit;
    QCheckBox*        searchRecursiveCheck;
    QCheckBox*        hideMinimizeButton;
    QCheckBox*        disableAccel;
    QPushButton*      changePathButton;
    QCheckBox*        usePopup;
    QCheckBox*        useFlash;
    QLabel*           useFlashLabel;
    QCheckBox*        readOnlyPlasma;
    KComboBox*        effectBox;
    QSlider*          graySlider;
};

SetupMaximized::SetupMaximized( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupMaximizedPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // Counterbox and skipbox next to eachother

    QLabel* effectLabel = new QLabel( i18n( "Chose the effect you want to during breaks" ) );
    d->effectBox = new KComboBox( this );
    if ( KWindowSystem::compositingActive() )
        d->effectBox->addItem( i18n("Simple Gray Effect"), QVariant( RSIObject::SimpleGray ) );
    else
        d->effectBox->addItem( i18n("Complete Black Effect"), QVariant( RSIObject::SimpleGray ) );
    d->effectBox->addItem( i18n("Show Plasma Dashboard"), QVariant( RSIObject::Plasma ) );
    d->effectBox->addItem( i18n("Show Slide Show of Images"), QVariant( RSIObject::SlideShow ) );
    d->effectBox->addItem( i18n("Show a Passive Popup"), QVariant( RSIObject::Popup ) );
    connect( d->effectBox, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotEffectChanged( int ) ) );

    //---------------- SKIP BOX
    QGroupBox *skipBox = new QGroupBox( this );
    skipBox->setTitle( i18n( "Skipping Breaks" ) );

    d->hideMinimizeButton = new QCheckBox( i18n( "&Hide skip button" ), this );
    d->hideMinimizeButton->setWhatsThis( i18n( "Check this option to disable and "
                                         "hide the skip button. This way you can prevent skipping the break." ) );

    d->disableAccel = new QCheckBox( i18n( "&Disable shortcut" ), this );
    d->disableAccel->setWhatsThis( i18n( "Check this option to disable the skip "
                                         "shortcut. This way you can prevent skipping the break." ) );

    QVBoxLayout *vbox1 = new QVBoxLayout( skipBox );
    vbox1->addWidget( d->hideMinimizeButton );
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

    KHBox* hboxslider = new KHBox( this );
    hboxslider->setSpacing( 30 );
    new QLabel( i18n( "Transparent" ), hboxslider );
    d->graySlider = new QSlider( hboxslider );
    d->graySlider->setOrientation( Qt::Horizontal );
    d->graySlider->setMinimum( 0 );
    d->graySlider->setPageStep( 10 );
    d->graySlider->setSingleStep( 5 );
    d->graySlider->setTickPosition( QSlider::TicksBelow );
    d->graySlider->setMaximum( 100 );
    new QLabel( i18n( "Solid" ), hboxslider );

    QVBoxLayout *vbox8 = new QVBoxLayout( d->grayBox );
    vbox8->addWidget( hboxslider );
    d->plasmaBox->setLayout( vbox8 );

    //------------------ PATH Setup
    d->slideshowBox = new QGroupBox( this );
    d->slideshowBox->setTitle( i18n( "Slideshow" ) );

    KHBox *imageFolderBox = new KHBox( this );
    d->imageFolderEdit = new KLineEdit( imageFolderBox );
    d->imageFolderEdit->setWhatsThis( i18n( "Select the folder from which you "
                                            "want to see images. These images are randomly shown during the breaks. "
                                            "It will be searched recursively if you want..." ) );
    d->changePathButton = new QPushButton( i18n( "&Change..." ),
                                           imageFolderBox );
    d->searchRecursiveCheck = new QCheckBox( i18n( "Search path recursively" ),
            this );

    connect( d->changePathButton, SIGNAL( clicked() ),
             this, SLOT( slotFolderPicker() ) );
    connect( d->imageFolderEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotFolderEdited( const QString& ) ) );

    QVBoxLayout *vbox2 = new QVBoxLayout( d->slideshowBox );
    vbox2->addWidget( imageFolderBox );
    vbox2->addWidget( d->searchRecursiveCheck );
    d->slideshowBox->setLayout( vbox2 );

    //---------------- Popup setup

    QGroupBox *popupBox = new QGroupBox( this );
    popupBox->setTitle( i18n( "Popup" ) );

    QLabel *label = new QLabel( i18n( "RSIBreak can show a popup near the "
                                      "systray instead of replacing your whole screen with a picture." ), this );
    label->setWordWrap( true );

    d->usePopup = new QCheckBox( i18n( "&Use the popup" ), this );
    connect( d->usePopup, SIGNAL( toggled( bool ) ), SLOT( slotHideFlash() ) );
    d->usePopup->setWhatsThis( i18n( "With this checkbox you can indicate if you "
                                     "want to see the popup when it is time to break. It will count down to "
                                     "zero, so you know how long this break will be." ) );
    label->setBuddy( d->usePopup );

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
        KFileDialog::getExistingDirectory( d->imageFolderEdit->text(), this );

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
    KConfigGroup config = KGlobal::config()->group( "General Settings" );
    config.writeEntry( "ImageFolder", d->imageFolderEdit->text() );
    config.writeEntry( "HideMinimizeButton",
                       d->hideMinimizeButton->isChecked() );
    config.writeEntry( "SearchRecursiveCheck",
                       d->searchRecursiveCheck->isChecked() );
    config.writeEntry( "Effect",
                       d->effectBox->itemData( d->effectBox->currentIndex() ) );

    config.writeEntry( "DisableAccel", d->disableAccel->isChecked() );
    config.writeEntry( "UsePlasmaReadOnly",
                       d->readOnlyPlasma->isChecked() );
    config.writeEntry( "Graylevel", d->graySlider->value() );
    config = KGlobal::config()->group( "Popup Settings" );
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

    KConfigGroup config = KGlobal::config()->group( "General Settings" );
    d->imageFolderEdit->setText( config.readEntry( "ImageFolder", dir ) );
    d->hideMinimizeButton->setChecked(
        config.readEntry( "HideMinimizeButton", false ) );

    const int effect = d->effectBox->findData( QVariant(
                           config.readEntry( "Effect", 0 ) ) );
    d->effectBox->setCurrentIndex( effect );
    slotEffectChanged( effect );

    d->searchRecursiveCheck->setChecked(
        config.readEntry( "SearchRecursiveCheck", false ) );
    d->disableAccel->setChecked( config.readEntry( "DisableAccel", false ) );
    d->readOnlyPlasma->setChecked(
        config.readEntry( "UsePlasmaReadOnly", true ) );
    d->graySlider->setValue( config.readEntry( "Graylevel", 80 ) );
    config = KGlobal::config()->group( "Popup Settings" );
    d->usePopup->setChecked(
        config.readEntry( "UsePopup", true ) );
    d->useFlash->setChecked(
        config.readEntry( "UseFlash", true ) );
}

#include "setupmaximized.moc"
