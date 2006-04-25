/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier.gilles@free.fr>
 *
 * Copyright 2005 by Tom Albers <tomalbers@kde.nl>
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

// Local includes.

#include "setuppopup.h"

class SetupPopupPriv
{
public:
    QCheckBox*        usePopup;
    QCheckBox*        useFlash;
    QLabel*           useFlashLabel;
};

SetupPopup::SetupPopup(QWidget* parent )
           : QWidget(parent)
{
    kdDebug() << "Entering SetupPopup" << endl;
    d = new SetupPopupPriv;

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

    d->usePopup = new QCheckBox(i18n("&Use the popup"), parent);
    connect(d->usePopup, SIGNAL(toggled(bool)), SLOT(slotHideFlash()));
    QWhatsThis::add( d->usePopup, i18n("With this checkbox you can indicate "
                                      "if you want to see the popup when it "
                                      "is time to break. It will count "
                                      "down to zero, so you know how long this "
                                      "break will be.") );
    layout->addWidget(d->usePopup);
    label->setBuddy(d->usePopup);

    d->useFlashLabel = new QLabel( i18n("When the popup is shown, it can flash\n"
                                       "when there is activity detected. You \n"
                                       "can turn it off when you find it too\n"
                                       "intrusive."), parent);
    layout->addWidget(d->useFlashLabel);

    d->useFlash = new QCheckBox(i18n("&Flash on activity"), parent);
    QWhatsThis::add( d->useFlash, i18n("With this checkbox you can indicate "
                                      "if you want to see the popup flash "
                                      "when there is activity.") );
    layout->addWidget(d->useFlash);
    d->useFlashLabel->setBuddy( d->useFlashLabel );

    readSettings();
    slotHideFlash();
}

SetupPopup::~SetupPopup()
{
    kdDebug() << "Entering ~SetupPopup" << endl;
    delete d;
}

void SetupPopup::slotHideFlash()
{
    kdDebug() << "Entering slotHideFlash" << endl;
    d->useFlash->setEnabled(d->usePopup->isChecked());
    d->useFlashLabel->setEnabled(d->usePopup->isChecked());
}

void SetupPopup::applySettings()
{
    kdDebug() << "Entering applySettings" << endl;
    KConfig* config = kapp->config();

    config->setGroup("Popup Settings");
    config->writeEntry("UsePopup",
                       d->usePopup->isChecked());
    config->writeEntry("UseFlash",
                       d->useFlash->isChecked());

    config->sync();

}

void SetupPopup::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    config->setGroup("Popup Settings");
    d->usePopup->setChecked(
            config->readBoolEntry("UsePopup", true));
    d->useFlash->setChecked(
            config->readBoolEntry("UseFlash", true));
}

#include "setuppopup.moc"
