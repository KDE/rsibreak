/* ============================================================
 * Copyright 2005-2007 by Tom Albers <tomalbers@kde.nl>
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

// QT includes.
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>

// KDE includes.
#include <KLocale>
#include <KApplication>
#include <KFileDialog>
#include <KLineEdit>
#include <KVBox>
#include <kkeysequencewidget.h>

class SetupMaximizedPriv
{
public:
    KHBox*            colorBox;
    KHBox*            fontBox;
    QGroupBox*        slideshowBox;
    QGroupBox*        plasmaBox;
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



};

SetupMaximized::SetupMaximized( QWidget* parent )
        : QWidget( parent )
{
    d = new SetupMaximizedPriv;

    QVBoxLayout *l = new QVBoxLayout( this );

    // Counterbox and skipbox next to eachother

    //---------------- SKIP BOX
    QGroupBox *skipBox = new QGroupBox( this );
    skipBox->setTitle( i18n( "Skipping Breaks" ) );

    d->hideMinimizeButton = new QCheckBox( i18n( "H&ide button" ), this );
    d->hideMinimizeButton->setWhatsThis( i18n( "Check this option to disable and "
                                         "hide the skip button. This way you can prevent skipping the break." ) );

    d->disableAccel = new QCheckBox( i18n( "&Shortcut Locks" ), this );
    d->disableAccel->setWhatsThis( i18n( "Check this option to disable the skip "
                                         "shortcut. This way you can prevent skipping the break. Instead it "
                                         "will lock the screen." ) );
    connect( d->disableAccel, SIGNAL( toggled( bool ) ), SLOT( slotHideShortcut() ) );

    QVBoxLayout *vbox1 = new QVBoxLayout( skipBox );
    vbox1->addWidget( d->hideMinimizeButton );
    vbox1->addWidget( d->disableAccel );
    vbox1->addStretch( 1 );
    skipBox->setLayout( vbox1 );

    //------------------ Plasma Setup
    d->plasmaBox = new QGroupBox( this );
    d->plasmaBox->setTitle( i18n( "Plasma Dashboard" ) );
    d->plasmaBox->setCheckable( true );
    connect( d->plasmaBox, SIGNAL( toggled( bool ) ), SLOT( slotUsePlasma() ) );

    d->readOnlyPlasma = new QCheckBox( i18n( "Read Only" ), this );

    QVBoxLayout *vbox5 = new QVBoxLayout( d->plasmaBox );
    vbox5->addWidget( d->readOnlyPlasma );
    d->plasmaBox->setLayout( vbox5 );

    //------------------ PATH Setup
    d->slideshowBox = new QGroupBox( this );
    d->slideshowBox->setTitle( i18n( "Slideshow" ) );
    d->slideshowBox->setCheckable( true );
    connect( d->slideshowBox, SIGNAL( toggled( bool ) ), SLOT( slotUseImages() ) );

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

    l->addWidget( skipBox );
    l->addWidget( d->plasmaBox );
    l->addWidget( d->slideshowBox );
    l->addWidget( popupBox );
    setLayout( l );
    readSettings();
    slotHideFlash();
    slotUsePlasma();
    slotUseImages();
}

SetupMaximized::~SetupMaximized()
{
    delete d;
}

void SetupMaximized::slotUsePlasma()
{
    d->slideshowBox->setEnabled( !d->plasmaBox->isChecked() );
    d->readOnlyPlasma->setEnabled( d->plasmaBox->isChecked() );
}

void SetupMaximized::slotUseImages()
{
    d->searchRecursiveCheck->setEnabled( d->slideshowBox->isChecked() );
    d->imageFolderEdit->setEnabled( d->slideshowBox->isChecked() );
    d->changePathButton->setEnabled( d->slideshowBox->isChecked() );
    d->plasmaBox->setEnabled( !d->slideshowBox->isChecked() );
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
    config.writeEntry( "ShowImages",
                       d->slideshowBox->isChecked() );

    config.writeEntry( "DisableAccel", d->disableAccel->isChecked() );
    config.writeEntry( "UsePlasma",
                       d->plasmaBox->isChecked() );
    config.writeEntry( "UsePlasmaReadOnly",
                       d->readOnlyPlasma->isChecked() );

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
    d->slideshowBox->setChecked(
        config.readEntry( "ShowImages", false ) );
    d->searchRecursiveCheck->setChecked(
        config.readEntry( "SearchRecursiveCheck", false ) );
    d->disableAccel->setChecked( config.readEntry( "DisableAccel", false ) );
    d->plasmaBox->setChecked(
        config.readEntry( "UsePlasma", false ) );
    d->readOnlyPlasma->setChecked(
        config.readEntry( "UsePlasmaReadOnly", true ) );

    config = KGlobal::config()->group( "Popup Settings" );
    d->usePopup->setChecked(
        config.readEntry( "UsePopup", true ) );
    d->useFlash->setChecked(
        config.readEntry( "UseFlash", true ) );
}

#include "setupmaximized.moc"
