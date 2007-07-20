/* ============================================================
 * Original copied from showfoto:
 *    Copyright 2005 by Gilles Caulier <caulier.gilles@free.fr>
 *
 * Copyright 2005-2006 by Tom Albers <tomalbers@kde.nl>
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

#include <qlayout.h>
#include <q3hbox.h>
#include <q3vgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3whatsthis.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

// KDE includes.

#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kdeversion.h>

#if KDE_IS_VERSION(3,3,91)
#include <kshortcutdialog.h>
#else
#include <kmessagebox.h>
#include <kglobal.h>
#endif

class SetupMaximizedPriv
{
public:
    Q3HBox*            colorBox;
    Q3HBox*            fontBox;
    Q3HBox*            shortcutBox;
    Q3VGroupBox*       slideshowBox;
    QPushButton*      counterFontBut;
    QPushButton*      folderBut;
    QPushButton*      shortcutBut;
    QLineEdit*        imageFolderEdit;
    QString           shortcut;
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

   //--- Vertical to start with
   Q3VBoxLayout *layout = new Q3VBoxLayout( parent );
   layout->setSpacing( KDialog::spacingHint() );
   layout->setAlignment( AlignTop );

   // Counterbox and skipbox next to eachother
   Q3HBox *boxes= new Q3HBox(parent,"mainbox");
   boxes->setSpacing( KDialog::spacingHint() );

   //-------------------- Counterbox
   Q3VGroupBox *counterBox = new Q3VGroupBox(boxes);
   counterBox->setTitle(i18n("Counter"));

   d->hideCounter = new QCheckBox(i18n("H&ide"), counterBox);
   connect(d->hideCounter, SIGNAL(toggled(bool)), SLOT(slotHideCounter()));
   Q3WhatsThis::add( d->hideCounter, i18n("With this checkbox you can indicate "
           "if you want to see a counter during the breaks. It will count "
           "down to zero, so you know how long this break will be. It will "
           "be shown on top of the image, when images are shown.") );

   d->colorBox = new Q3HBox(counterBox,"colorbox");
   QLabel *counterColorlabel = new QLabel( i18n("&Color:")+' ', d->colorBox );
   counterColorlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
   d->counterColor = new KColorButton(d->colorBox);
   counterColorlabel->setBuddy(d->counterColor);
   Q3WhatsThis::add( d->counterColor, i18n("Select here the color to use "
            "for the counter. As this is used on top of the images, you "
            "might want to set it to a color which is visible for most "
            "of the images.") );

   d->fontBox = new Q3HBox(counterBox,"counterbox");
   QLabel *counterFontlabel = new QLabel( i18n("&Font:")+' ', d->fontBox );
   counterFontlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
   d->counterFontBut = new QPushButton("font",d->fontBox);
   counterFontlabel->setBuddy(d->counterFontBut);
   Q3WhatsThis::add( d->counterFontBut, i18n("Select here the font to use "
           "for the counter.") );
   connect(d->counterFontBut, SIGNAL(clicked()), SLOT(slotFontPicker()));

   //layout->addWidget(counterBox);

   //---------------- SKIP BOX
   Q3VGroupBox *skipBox = new Q3VGroupBox(boxes);
   skipBox->setTitle(i18n("Skipping Breaks"));

   d->hideMinimizeButton = new QCheckBox(i18n("H&ide button"), skipBox);
   Q3WhatsThis::add( d->hideMinimizeButton,
                    i18n("Check this option to disable and hide the skip "
                            "button. This way you can prevent skipping the "
                            "break.") );

   d->disableAccel = new QCheckBox(i18n("&Disable shortcut"),
                                   skipBox);
   Q3WhatsThis::add( d->disableAccel,
                    i18n("Check this option to disable the skip shortcut."
                            "This way you can prevent skipping the break.") );
   connect(d->disableAccel, SIGNAL(toggled(bool)), SLOT(slotHideShortcut()));

   d->shortcutBox = new Q3HBox(skipBox,"chortcutbox");
   QLabel *shortcutlabel = new QLabel( i18n("&Shortcut:") + ' ', d->shortcutBox );
   shortcutlabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
   d->shortcutBut = new QPushButton(i18n("&Change Shortcut..."), d->shortcutBox );
   shortcutlabel->setBuddy(d->shortcutBut);
   Q3WhatsThis::add( d->shortcutBut, i18n("Select here the shortcut to use "
           "for aborting the break.") );
   connect(d->shortcutBut, SIGNAL(clicked()),
           this, SLOT(slotShortcutPicker()));

   //-- Add the two boxes.
   layout->addWidget( boxes );

   //------------------ PATH Setup
   d->slideshowBox = new Q3VGroupBox(parent);
   d->slideshowBox->setTitle(i18n("Slideshow"));
   d->slideshowBox->setCheckable( true );
   connect(d->slideshowBox, SIGNAL( toggled(bool)), SLOT(slotUseImages()));

   Q3HBox *imageFolderBox = new Q3HBox(d->slideshowBox,"imagefolderbox");
   d->imageFolderEdit = new QLineEdit(imageFolderBox);
   Q3WhatsThis::add( d->imageFolderEdit,
        i18n("Select the folder from which you want to see images. "
             "These images are randomly shown during the breaks. "
             "It will be searched recursively if you want...") );
   d->changePathButton = new QPushButton(i18n("&Change..."),
           imageFolderBox);
   d->searchRecursiveCheck = new QCheckBox(i18n("Search path recursively"),
                                           d->slideshowBox);

   connect(d->changePathButton, SIGNAL(clicked()),
           this, SLOT(slotFolderPicker()));
   connect( d->imageFolderEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotFolderEdited(const QString&)) );

   layout->addWidget(d->slideshowBox);

   //---------------- Popup setup

   Q3VGroupBox *popupBox = new Q3VGroupBox(parent);
   popupBox->setTitle(i18n("Popup"));

   QLabel *label = new QLabel( i18n("RSIBreak can show a popup near the "
           "systray instead of replacing your whole screen with a picture."),
            popupBox);
   label->setAlignment(Qt::WordBreak);

   d->usePopup = new QCheckBox(i18n("&Use the popup"), popupBox);
   connect(d->usePopup, SIGNAL(toggled(bool)), SLOT(slotHideFlash()));
   Q3WhatsThis::add( d->usePopup, i18n("With this checkbox you can indicate "
           "if you want to see the popup when it "
                   "is time to break. It will count "
                   "down to zero, so you know how long this "
                   "break will be.") );
   label->setBuddy(d->usePopup);

   d->useFlashLabel = new QLabel( '\n' + i18n("The popup can flash when it "
           "detects that you are still active."), popupBox);
   d->useFlashLabel->setAlignment(Qt::WordBreak);

   d->useFlash = new QCheckBox(i18n("&Flash on activity"), popupBox);
   Q3WhatsThis::add( d->useFlash, i18n("With this checkbox you can indicate "
           "if you want to see the popup flash "
                   "when there is activity.") );
   d->useFlashLabel->setBuddy( d->useFlashLabel );

   layout->addWidget(popupBox);

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

void SetupMaximized::slotShortcutPicker()
{
#if KDE_IS_VERSION(3,3,91)
    KShortcutDialog key(d->shortcut,true);
    key.exec();
    d->shortcut=key.shortcut().toString();
    d->shortcutBut->setText(d->shortcut);
#else
    KMessageBox::information(this, i18n("You are using KDE 3.3 or older, "
          "with this version of KDE, you can not change this shortcut."));
#endif
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
    config.writeEntry("MinimizeKey", d->shortcut);

    config = KGlobal::config()->group("Popup Settings");
    config.writeEntry("UsePopup",
                       d->usePopup->isChecked());
    config.writeEntry("UseFlash",
                       d->useFlash->isChecked());

    config.sync();
}

void SetupMaximized::readSettings()
{
    QColor *Black = new QColor(Qt::black);
    QFont *font = new QFont(  QApplication::font().family(), 40, 75, true );
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
    d->shortcut = config.readEntry("MinimizeKey", "Escape");
    d->shortcutBut->setText(d->shortcut);

    config = KGlobal::config()->group("Popup Settings");
    d->usePopup->setChecked(
            config.readEntry("UsePopup", true));
    d->useFlash->setChecked(
            config.readEntry("UseFlash", true));

    delete Black;
    delete font;
}

#include "setupmaximized.moc"
