/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copright 2005 by Tom Albers <tomalbers@kde.nl>
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
#include <kshortcutdialog.h>

// Local includes.

#include "setuppopup.h"

SetupPopup::SetupPopup(QWidget* parent )
           : QWidget(parent)
{
    kdDebug() << "Entering SetupPopup" << endl;
    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );
        
    QLabel *label = new QLabel( i18n("When it is time to break, RSIBreak can\n"
                                     "show a popup near the systemtray, then\n"
                                     "you can take a break without the screen\n"
                                     "being replaced by a picture. If you\n"
                                     "want to see pictures and not the popup,\n"
                                     "turn this function off."), parent);
    layout->addWidget(label);
    
    m_usePopup = new QCheckBox(i18n("&Use the popup"), parent);
    connect(m_usePopup, SIGNAL(toggled(bool)), SLOT(slotHideFlash()));
    QWhatsThis::add( m_usePopup, i18n("With this checkbox you can indicate "
                                      "if you want to see the popup when it "
                                      "is time to break. It will count "
                                      "down to zero, so you know how long this "
                                      "break will be.") );
    layout->addWidget(m_usePopup);
    
    m_useFlashLabel = new QLabel( i18n("When the popup is shown, it can flash\n"
                                       "when there is activity detected. You \n"
                                       "can turn it off when you find it to\n"
                                       "intrusive."), parent);
    layout->addWidget(m_useFlashLabel);
    
    m_useFlash = new QCheckBox(i18n("&Flash on activity"), parent);
    QWhatsThis::add( m_useFlash, i18n("With this checkbox you can indicate "
                                      "if you want to see the popup flash "
                                      "when there is activity.") );
    layout->addWidget(m_useFlash);
    
    readSettings();
    slotHideFlash();
}

SetupPopup::~SetupPopup()
{
    kdDebug() << "Entering ~SetupPopup" << endl;
}

void SetupPopup::slotHideFlash()
{
    kdDebug() << "Entering slotHideFlash" << endl;
    m_useFlash->setEnabled(m_usePopup->isChecked());
    m_useFlashLabel->setEnabled(m_usePopup->isChecked());
}

void SetupPopup::applySettings()
{
    kdDebug() << "Entering applySettings" << endl;
    KConfig* config = kapp->config();
    
    config->setGroup("Popup Settings");
    config->writeEntry("UsePopup",
                       m_usePopup->isChecked());
    config->writeEntry("UseFlash",
                       m_useFlash->isChecked());

    config->sync();

}

void SetupPopup::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    config->setGroup("Popup Settings");
    m_usePopup->setChecked(
            config->readBoolEntry("UsePopup", true));
    m_useFlash->setChecked(
            config->readBoolEntry("UseFlash", true));
}

#include "setuppopup.moc"
