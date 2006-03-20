/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copright 2005-2006 by Tom Albers <tomalbers@kde.nl>
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

// QT includes.

#include <qlayout.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

// KDE includes.

#include <kdebug.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kdeversion.h>

#if KDE_IS_VERSION(3,3,91)
#include <kshortcutdialog.h>
#else
#include <kmessagebox.h>
#endif

// Local includes.

#include "setupmaximized.h"

SetupMaximized::SetupMaximized(QWidget* parent )
           : QWidget(parent)
{
   kdDebug() << "Entering SetupMaximized" << endl;
   QVBoxLayout *layout = new QVBoxLayout( parent );
   layout->setSpacing( KDialog::spacingHint() );
   layout->setAlignment( AlignTop );

   QVGroupBox *counterBox = new QVGroupBox(parent);
   counterBox->setTitle(i18n("Counter"));

   m_hideCounter = new QCheckBox(i18n("H&ide"), counterBox);
   connect(m_hideCounter, SIGNAL(toggled(bool)), SLOT(slotHideCounter()));
   QWhatsThis::add( m_hideCounter, i18n("With this checkbox you can indicate "
           "if you want to see a counter during the breaks. It will count "
           "down to zero, so you know how long this break will be. It will "
           "be shown on top of the image, when images are shown.") );

   m_colorBox = new QHBox(counterBox);
   QLabel *counterColorlabel = new QLabel( i18n("&Color:"), m_colorBox );
   m_counterColor = new KColorButton(m_colorBox);
   counterColorlabel->setBuddy(m_counterColor);
   QWhatsThis::add( m_counterColor, i18n("Select here the color to use "
            "for the counter. As this is used on top of the images, you "
            "might want to set it to a color which is visible for most "
            "of the images.") );

   m_fontBox = new QHBox(counterBox);
   QLabel *counterFontlabel = new QLabel( i18n("&Font:"), m_fontBox );
   m_counterFontBut = new QPushButton("font",m_fontBox);
   counterFontlabel->setBuddy(m_counterFontBut);
   QWhatsThis::add( m_counterFontBut, i18n("Select here the font to use "
           "for the counter.") );
   connect(m_counterFontBut, SIGNAL(clicked()), SLOT(slotFontPicker()));

   layout->addWidget(counterBox);

   QVGroupBox *imageBox = new QVGroupBox(parent);
   imageBox->setTitle(i18n("Image Folder Path"));

   QHBox *imageFolderBox = new QHBox(imageBox);
   m_imageFolderEdit = new QLineEdit(imageFolderBox);
   QWhatsThis::add( m_imageFolderEdit,
        i18n("Select the folder from which you want to see images. "
             "These images are randomly shown during the breaks. "
             "It will be searched recursively if you want...") );

   QPushButton *changePathButton = new QPushButton(i18n("&Change..."),
           imageFolderBox);
   connect(changePathButton, SIGNAL(clicked()),
           this, SLOT(slotFolderPicker()));
   connect( m_imageFolderEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotFolderEdited(const QString&)) );

   m_searchRecursiveCheck = new QCheckBox(i18n("Search recursively"),
                                          imageBox);
   layout->addWidget(imageBox);

   m_hideMinimizeButton = new QCheckBox(i18n("H&ide skip button"), parent);
   QWhatsThis::add( m_hideMinimizeButton,
                    i18n("Check this option to disable and hide the skip "
                          "button. This way you can prevent skipping the "
                          "break.") );
   layout->addWidget(m_hideMinimizeButton);

   m_disableAccel = new QCheckBox(i18n("&Disable shortcut to skip"),
                                  parent);
   QWhatsThis::add( m_disableAccel,
                    i18n("Check this option to disable the skip shortcut."
                         "This way you can prevent skipping the break.") );
   connect(m_disableAccel, SIGNAL(toggled(bool)), SLOT(slotHideShortcut()));
   layout->addWidget(m_disableAccel);

   m_shortcutBox = new QHBox(parent);
   QLabel *shortcutlabel = new QLabel( i18n("&Shortcut:"), m_shortcutBox );
   shortcutlabel->setIndent( 22 );
   m_shortcutBut = new QPushButton(i18n("&Change Shortcut..."), m_shortcutBox );
   shortcutlabel->setBuddy(m_shortcutBut);
   QWhatsThis::add( m_shortcutBut, i18n("Select here the shortcut to use "
           "for aborting the break.") );
   connect(m_shortcutBut, SIGNAL(clicked()),
           this, SLOT(slotShortcutPicker()));
   layout->addWidget( m_shortcutBox );

   readSettings();
   slotHideCounter();
   slotHideShortcut();
}

SetupMaximized::~SetupMaximized()
{
kdDebug() << "Entering ~SetupMaximized" << endl;
}

void SetupMaximized::slotHideCounter()
{
    kdDebug() << "Entering slotHideCounter" << endl;
    m_colorBox->setEnabled(!m_hideCounter->isChecked());
    m_fontBox->setEnabled(!m_hideCounter->isChecked());
}

void SetupMaximized::slotHideShortcut()
{
    kdDebug() << "Entering slotHideShortcut" << endl;
    m_shortcutBox->setEnabled(!m_disableAccel->isChecked());
}

void SetupMaximized::slotFontPicker()
{
    kdDebug() << "Entering slotFontPicker" << endl;
    KFontDialog::getFont( m_counterFont );
    m_counterFontBut->setText(m_counterFont.family());
}

void SetupMaximized::slotShortcutPicker()
{
    kdDebug() << "Entering slotShortcutPicker" << endl;

#if KDE_IS_VERSION(3,3,91)
    KShortcutDialog key(m_shortcut,true);
    key.exec();
    m_shortcut=key.shortcut().toString();
    m_shortcutBut->setText(m_shortcut);
#else
    KMessageBox::information(this, i18n("You are using KDE 3.3 or older, "
          "with this version of KDE, you can not change this shortcut."));
#endif
}

void SetupMaximized::slotFolderPicker()
{
     kdDebug() << "Entering slotFolderPicker" << endl;
     QString  result =
             KFileDialog::getExistingDirectory( m_imageFolderEdit->text(), this);

    if (!result.isEmpty())
       m_imageFolderEdit->setText(result);
}

void SetupMaximized::slotFolderEdited(const QString& newPath)
{
    kdDebug() << "Entering slotFolderEdited" << endl;
    if (newPath.isEmpty()) {
        m_imageFolderEdit->setText(QDir::homeDirPath());
        return;
    }

    if (!newPath.startsWith("/")) {
        m_imageFolderEdit->setText(QDir::homeDirPath()+"/"+newPath);
    }

}

void SetupMaximized::applySettings()
{
    kdDebug() << "Entering applySettings" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General Settings");
    config->writeEntry("CounterColor", m_counterColor->color());
    config->writeEntry("CounterFont", m_counterFont);
    config->writeEntry("ImageFolder", m_imageFolderEdit->text());
    config->writeEntry("HideMinimizeButton",
                       m_hideMinimizeButton->isChecked());
    config->writeEntry("SearchRecursiveCheck",
                       m_searchRecursiveCheck->isChecked());
    config->writeEntry("HideCounter", m_hideCounter->isChecked());
    config->writeEntry("DisableAccel", m_disableAccel->isChecked());
    config->writeEntry("MinimizeKey", m_shortcut);
    config->sync();
}

void SetupMaximized::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    QColor *Black = new QColor(Qt::black);
    QFont *t = new QFont(  QApplication::font().family(), 40, 75, true );
    QString d = QDir::home().path();

    config->setGroup("General Settings");
    m_counterColor->setColor( config->readColorEntry("CounterColor", Black ) );
    m_counterFont= config->readFontEntry("CounterFont", t ) ;
    m_imageFolderEdit->setText(config->readEntry("ImageFolder", d ));
    m_counterFontBut->setText(m_counterFont.family());
    m_hideMinimizeButton->setChecked(
            config->readBoolEntry("HideMinimizeButton", false));
    m_searchRecursiveCheck->setChecked(
            config->readBoolEntry("SearchRecursiveCheck", false));
    m_hideCounter->setChecked(config->readBoolEntry("HideCounter", false));
    m_disableAccel->setChecked(config->readBoolEntry("DisableAccel", false));
    m_shortcut = config->readEntry("MinimizeKey", "Escape");
    m_shortcutBut->setText(m_shortcut);

    delete Black;
}

#include "setupmaximized.moc"
