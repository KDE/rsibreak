/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier.gilles@free.fr>
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

#include "setupgeneral.h"

class SetupGeneralPriv
{
public:
    QCheckBox*        autoStart;
    QCheckBox*        useIdleDetection;
    QCheckBox*        useNoIdleTimer;
    QCheckBox*        showTimerReset;
};

SetupGeneral::SetupGeneral(QWidget* parent )
           : QWidget(parent)
{
    d = new SetupGeneralPriv;

    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );

    d->autoStart = new QCheckBox(i18n("&Automatically start RSIBreak at startup"), parent);
    QWhatsThis::add( d->autoStart, i18n("With this checkbox you can indicate "
            "that you want RSIBreak to start when KDE starts.") );
    layout->addWidget(d->autoStart);

    d->useNoIdleTimer = new QCheckBox(i18n("&Break at fixed times and ignore "
            "movement during breaks"), parent);
    QWhatsThis::add( d->useNoIdleTimer, i18n("With this checkbox you indicate "
            "that you want to ignore movements during breaks and want to break "
            "at fixed intervals") );
    layout->addWidget(d->useNoIdleTimer);
    connect(d->useNoIdleTimer , SIGNAL(toggled(bool)), SLOT(slotUseNoIdleTimer()));

    d->useIdleDetection = new QCheckBox(
            i18n("&Reset timers after period of idleness"), parent);
    QWhatsThis::add( d->useIdleDetection, i18n("With this checkbox you indicate "
            "that you want to use idle detection. Unchecked, RSIBreak will not "
            "reset the timers when you are idle for the duration of a break, but "
            "will take into account the periods you are working or idle.") );
    layout->addWidget(d->useIdleDetection);
    connect(d->useIdleDetection , SIGNAL(toggled(bool)), SLOT(slotShowTimer()));

    d->showTimerReset = new QCheckBox(i18n("&Show when timers are reset"), parent);
    QWhatsThis::add( d->showTimerReset, i18n("With this checkbox you indicate "
            "that you want to see when the timers are reset. This happens when you "
            "have been idle for a while.") );
    layout->addWidget(d->showTimerReset);

    readSettings();
    slotUseNoIdleTimer();
}

SetupGeneral::~SetupGeneral()
{
    delete d;
}

void SetupGeneral::slotShowTimer()
{
    d->showTimerReset->setEnabled(d->useIdleDetection->isChecked() &&
                                  !d->useNoIdleTimer->isChecked());
}

void SetupGeneral::slotUseNoIdleTimer()
{
    d->useIdleDetection->setEnabled(!d->useNoIdleTimer->isChecked());
    d->showTimerReset->setEnabled(!d->useNoIdleTimer->isChecked());
    slotShowTimer();
}

void SetupGeneral::applySettings()
{
    KConfig* config = kapp->config();

    config->setGroup("General");
    config->writeEntry("AutoStart", d->autoStart->isChecked());

    config->setGroup("General Settings");
    config->writeEntry("UseIdleDetection", d->useIdleDetection->isChecked());
    config->writeEntry("ShowTimerReset", d->showTimerReset->isChecked());
    config->writeEntry("UseNoIdleTimer", d->useNoIdleTimer->isChecked());
    config->sync();
}

void SetupGeneral::readSettings()
{
    KConfig* config = kapp->config();

    config->setGroup("General");
    d->autoStart->setChecked(config->readBoolEntry("AutoStart", false));

    config->setGroup("General Settings");
    d->useIdleDetection->setChecked(config->readBoolEntry("UseIdleDetection", true));
    d->showTimerReset->setChecked(config->readBoolEntry("ShowTimerReset", false));
    d->useNoIdleTimer->setChecked(config->readBoolEntry("UseNoIdleTimer", false));
}

#include "setupgeneral.moc"
