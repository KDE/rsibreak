/* ============================================================
 * Copyright 2005-2007 by Tom Albers <tomalbers@kde.nl>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ============================================================ */

// Local includes.
#include "setupmaximized.h"

// QT includes.
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>

// KDE includes.
#include <KLocale>
#include <KColorButton>
#include <KFontDialog>
#include <KApplication>
#include <KFileDialog>
#include <KVBox>
#include <kkeysequencewidget.h>

class SetupMaximizedPriv
{
public:
    KHBox*            colorBox;
    KHBox*            fontBox;
    KHBox*            shortcutBox;
    QGroupBox*        slideshowBox;
    QPushButton*      counterFontBut;
    QPushButton*      folderBut;
    KKeySequenceWidget*  shortcutBut;
    QLineEdit*        imageFolderEdit;
    QFont             counterFont;
    KColorButton*     counterColor;
    QCheckBox*        searchRecursiveCheck;
    QCheckBox*        hideMinimizeButton;
    QCheckBox*        hideCounter;
    QCheckBox*        disableAccel;
    QPushButton*      changePathButton;
    QCheckBox*        usePopup;
    QCheckBox*        useFlash;
    QLabel*           useFlashLabel;

};

SetupMaximized::SetupMaximized(QWidget* parent )
           : QWidget(parent)
{
   d = new SetupMaximizedPriv;

   QVBoxLayout *layout = new QVBoxLayout( parent );
   KVBox *l = new KVBox( this );

   // Counterbox and skipbox next to eachother
   KHBox *boxes= new KHBox(l);
   boxes->setSpacing( KDialog::spacingHint() );

   //-------------------- Counterbox
   QGroupBox *counterBox = new QGroupBox(boxes);
   counterBox->setTitle(i18n("Counter"));

   d->hideCounter = new QCheckBox(i18n("H&ide"), this);
   connect(d->hideCounter, SIGNAL(toggled(bool)), SLOT(slotHideCounter()));
   d->hideCounter->setWhatsThis( i18n("With this checkbox you can indicate "
       "if you want to see a counter during the breaks. It will count down to "
       "zero, so you know how long this break will be. It will be shown on top "
       "of the image, when images are shown.") );

   d->colorBox = new KHBox(this);
   QLabel *counterColorlabel = new QLabel( i18n("&Color:")+' ', d->colorBox );
   counterColorlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
   d->counterColor = new KColorButton(d->colorBox);
   counterColorlabel->setBuddy(d->counterColor);
   d->counterColor->setWhatsThis( i18n("Select here the color to use for the "
       "counter. As this is used on top of the images, you might want to set "
       "it to a color which is visible for most of the images.") );

   d->fontBox = new KHBox(this);
   QLabel *counterFontlabel = new QLabel( i18n("&Font:")+' ', d->fontBox );
   counterFontlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
   d->counterFontBut = new QPushButton("font",d->fontBox);
   counterFontlabel->setBuddy(d->counterFontBut);
   d->counterFontBut->setWhatsThis( i18n("Select here the font to use for the "
       "counter.") );
   connect(d->counterFontBut, SIGNAL(clicked()), SLOT(slotFontPicker()));

   QVBoxLayout *vbox0 = new QVBoxLayout(counterBox);
   vbox0->addWidget(d->hideCounter);
   vbox0->addWidget(d->colorBox);
   vbox0->addWidget(d->fontBox);
   vbox0->addStretch(1);
   counterBox->setLayout(vbox0);

   //---------------- SKIP BOX
   QGroupBox *skipBox = new QGroupBox(boxes);
   skipBox->setTitle(i18n("Skipping Breaks"));

   d->hideMinimizeButton = new QCheckBox(i18n("H&ide button"), this);
   d->hideMinimizeButton->setWhatsThis( i18n("Check this option to disable and "
       "hide the skip button. This way you can prevent skipping the break.") );

   d->disableAccel = new QCheckBox(i18n("&Disable shortcut"), this);
   d->disableAccel->setWhatsThis( i18n("Check this option to disable the skip "
       "shortcut. This way you can prevent skipping the break.") );
   connect(d->disableAccel, SIGNAL(toggled(bool)), SLOT(slotHideShortcut()));

   d->shortcutBox = new KHBox(this);
   QLabel *shortcutlabel = new QLabel( i18n("&Shortcut:") + ' ', d->shortcutBox );
   shortcutlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

   d->shortcutBut = new KKeySequenceWidget(d->shortcutBox);
   d->shortcutBut->setClearButtonShown( false );
   d->shortcutBut->setModifierlessAllowed( true );
   shortcutlabel->setBuddy(d->shortcutBut);
   shortcutlabel->setWhatsThis( i18n("Select here the shortcut to use for "
        "aborting the break.") );
   connect(d->shortcutBut, SIGNAL( keySequenceChanged(const QKeySequence&)),
           this, SLOT(slotShortcutChanged(const QKeySequence&s)));

   QVBoxLayout *vbox1 = new QVBoxLayout(skipBox);
   vbox1->addWidget(d->hideMinimizeButton);
   vbox1->addWidget(d->disableAccel);
   vbox1->addWidget(d->shortcutBox);
   vbox1->addStretch(1);
   skipBox->setLayout(vbox1);

   //------------------ PATH Setup
   d->slideshowBox = new QGroupBox(l);
   d->slideshowBox->setTitle(i18n("Slideshow"));
   d->slideshowBox->setCheckable( true );
   connect(d->slideshowBox, SIGNAL( toggled(bool)), SLOT(slotUseImages()));

   KHBox *imageFolderBox = new KHBox(this);
   d->imageFolderEdit = new QLineEdit(imageFolderBox);
   d->imageFolderEdit->setWhatsThis( i18n("Select the folder from which you "
       "want to see images. These images are randomly shown during the breaks. "
       "It will be searched recursively if you want...") );
   d->changePathButton = new QPushButton(i18n("&Change..."),
           imageFolderBox);
   d->searchRecursiveCheck = new QCheckBox(i18n("Search path recursively"),
                                           this);

   connect(d->changePathButton, SIGNAL(clicked()),
           this, SLOT(slotFolderPicker()));
   connect( d->imageFolderEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotFolderEdited(const QString&)) );

   QVBoxLayout *vbox2 = new QVBoxLayout(d->slideshowBox);
   vbox2->addWidget(imageFolderBox);
   vbox2->addWidget(d->searchRecursiveCheck);
   d->slideshowBox->setLayout(vbox2);

   //---------------- Popup setup

   QGroupBox *popupBox = new QGroupBox(l);
   popupBox->setTitle(i18n("Popup"));

   QLabel *label = new QLabel( i18n("RSIBreak can show a popup near the "
       "systray instead of replacing your whole screen with a picture."), this);
   label->setWordWrap( true );

   d->usePopup = new QCheckBox(i18n("&Use the popup"), this);
   connect(d->usePopup, SIGNAL(toggled(bool)), SLOT(slotHideFlash()));
   d->usePopup->setWhatsThis( i18n("With this checkbox you can indicate if you "
       "want to see the popup when it is time to break. It will count down to "
       "zero, so you know how long this break will be.") );
   label->setBuddy(d->usePopup);

   d->useFlashLabel = new QLabel( '\n' + i18n("The popup can flash when it "
           "detects that you are still active."), this);
   d->useFlashLabel->setWordWrap( true );

   d->useFlash = new QCheckBox(i18n("&Flash on activity"), this);
   d->useFlash->setWhatsThis( i18n("With this checkbox you can indicate "
           "if you want to see the popup flash when there is activity.") );
   d->useFlashLabel->setBuddy( d->useFlashLabel );

   QVBoxLayout *vbox3 = new QVBoxLayout(popupBox);
   vbox3->addWidget(label);
   vbox3->addWidget(d->usePopup);
   vbox3->addWidget(d->useFlashLabel);
   vbox3->addWidget(d->useFlash);
   vbox3->addStretch(1);
   popupBox->setLayout(vbox3);

   layout->addWidget(this);

   readSettings();
   slotHideCounter();
   slotHideShortcut();
   slotHideFlash();
   slotUseImages();
}

SetupMaximized::~SetupMaximized()
{
    delete d;
}

void SetupMaximized::slotUseImages()
{
    d->searchRecursiveCheck->setEnabled(d->slideshowBox->isChecked());
    d->imageFolderEdit->setEnabled(d->slideshowBox->isChecked());
    d->changePathButton->setEnabled(d->slideshowBox->isChecked());
}

void SetupMaximized::slotHideCounter()
{
    d->colorBox->setEnabled(!d->hideCounter->isChecked());
    d->fontBox->setEnabled(!d->hideCounter->isChecked());
}

void SetupMaximized::slotHideShortcut()
{
    d->shortcutBox->setEnabled(!d->disableAccel->isChecked());
}

void SetupMaximized::slotFontPicker()
{
    KFontDialog::getFont( d->counterFont );
    d->counterFontBut->setText(d->counterFont.family());
}

void SetupMaximized::slotShortcutChanged(const QKeySequence &seq)
{
     d->shortcutBut->setKeySequence(seq);
}

void SetupMaximized::slotFolderPicker()
{
     QString  result =
             KFileDialog::getExistingDirectory( d->imageFolderEdit->text(), this);

    if (!result.isEmpty())
       d->imageFolderEdit->setText(result);
}

void SetupMaximized::slotFolderEdited(const QString& newPath)
{
    if (newPath.isEmpty()) {
        d->imageFolderEdit->setText(QDir::homePath());
        return;
    }

    if (!newPath.startsWith("/")) {
        d->imageFolderEdit->setText(QDir::homePath()+'/'+newPath);
    }

}

void SetupMaximized::slotHideFlash()
{
    d->useFlash->setEnabled(d->usePopup->isChecked());
    d->useFlashLabel->setEnabled(d->usePopup->isChecked());
}

void SetupMaximized::applySettings()
{
    KConfigGroup config = KGlobal::config()->group("General Settings");
    config.writeEntry("CounterColor", d->counterColor->color());
    config.writeEntry("CounterFont", d->counterFont);
    config.writeEntry("ImageFolder", d->imageFolderEdit->text());
    config.writeEntry("HideMinimizeButton",
                       d->hideMinimizeButton->isChecked());
    config.writeEntry("SearchRecursiveCheck",
                       d->searchRecursiveCheck->isChecked());
    config.writeEntry("ShowImages",
                       d->slideshowBox->isChecked());

    config.writeEntry("HideCounter", d->hideCounter->isChecked());
    config.writeEntry("DisableAccel", d->disableAccel->isChecked());
    config.writeEntry("MinimizeKey", d->shortcutBut->keySequence().toString());

    config = KGlobal::config()->group("Popup Settings");
    config.writeEntry("UsePopup",
                       d->usePopup->isChecked());
    config.writeEntry("UseFlash",
                       d->useFlash->isChecked());

    config.sync();
}

void SetupMaximized::readSettings()
{
    QColor Black(Qt::black);
    QFont font( QApplication::font().family(), 40, 75, true );
    QString dir = QDir::home().path();

    KConfigGroup config = KGlobal::config()->group("General Settings");
    d->counterColor->setColor( config.readEntry("CounterColor", Black ) );
    d->counterFont = config.readEntry("CounterFont", font ) ;
    d->imageFolderEdit->setText(config.readEntry("ImageFolder", dir ));
    d->counterFontBut->setText(d->counterFont.family());
    d->hideMinimizeButton->setChecked(
            config.readEntry("HideMinimizeButton", false));
    d->slideshowBox->setChecked(
            config.readEntry("ShowImages", false));
    d->searchRecursiveCheck->setChecked(
            config.readEntry("SearchRecursiveCheck", false));
    d->hideCounter->setChecked(config.readEntry("HideCounter", false));
    d->disableAccel->setChecked(config.readEntry("DisableAccel", false));
    d->shortcutBut->setKeySequence(
            QKeySequence(config.readEntry("MinimizeKey", "Esc")));

    config = KGlobal::config()->group("Popup Settings");
    d->usePopup->setChecked(
            config.readEntry("UsePopup", true));
    d->useFlash->setChecked(
            config.readEntry("UseFlash", true));
}

#include "setupmaximized.moc"
