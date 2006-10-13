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
#include <qradiobutton.h>
#include <qvbuttongroup.h>

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
    QVButtonGroup*    breakTimerSettings;
    QRadioButton*     useNoIdleTimer;
    QRadioButton*     useIdleTimer;
    QVButtonGroup*    idleSettings;
    QCheckBox*        resetTimersAfterIdle;
    QCheckBox*        ignoreIdleForTinyBreaks;
    QCheckBox*        showTimerReset;
};

SetupGeneral::SetupGeneral(QWidget* parent )
           : QWidget(parent)
{
    d = new SetupGeneralPriv;

    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );

    d->autoStart = new QCheckBox(
            i18n("&Automatically start RSIBreak at startup"), parent);
    QWhatsThis::add( d->autoStart,
            i18n("With this option you can indicate that you want RSIBreak to "
                 "start when KDE starts.") );
    layout->addWidget(d->autoStart);

    d->breakTimerSettings = new QVButtonGroup(
            i18n("Break Timer Settings"), parent);
    layout->addWidget(d->breakTimerSettings);

    d->useNoIdleTimer = new QRadioButton(
            i18n("Break at &fixed times and ignore movement during breaks"),
            d->breakTimerSettings);
    QWhatsThis::add( d->useNoIdleTimer,
            i18n("With this option you indicate that you want to ignore "
                 "movements during breaks and want to break at fixed intervals"));
    connect(d->useNoIdleTimer , SIGNAL(toggled(bool)),
            SLOT(slotUseNoIdleTimer()));

    d->useIdleTimer = new QRadioButton (
            i18n("Take into account the &idle Settings"),d->breakTimerSettings);
    QWhatsThis::add( d->useIdleTimer,
            i18n("With this option you indicate that you want to use idle "
                 "detection. This means that only the time you are active "
                 "(when you use the keyboard or the mouse) will be counted "
                 "when evaluating the position of the next break.") );

    // ---------------- IDLE Settings ------------------------

    d->idleSettings = new QVButtonGroup (i18n("Idle settings"), parent);
    layout->addWidget(d->idleSettings);

    d->resetTimersAfterIdle = new QCheckBox(
            i18n("&Reset timers after period of idleness"), d->idleSettings);
    QWhatsThis::add( d->resetTimersAfterIdle,
            i18n("With this checkbox you indicate that you want the timers for "
                 "the next break to be reset if you are sufficiently idle "
                 "(you are idle more that the duration of the corresponding "
                 "break)") );
    connect(d->resetTimersAfterIdle , SIGNAL(toggled(bool)),
            SLOT(slotShowTimer()));

    d->showTimerReset = new QCheckBox(
            i18n("&Show when timers are reset"), d->idleSettings);
    QWhatsThis::add( d->showTimerReset,
            i18n("With this checkbox you indicate that you want to see when "
                 "the timers are reset. This happens when you have been idle "
                 "for a while.") );

    d->ignoreIdleForTinyBreaks = new QCheckBox(
            i18n("&Ignore idle detection for tiny breaks."), d->idleSettings);
    QWhatsThis::add( d->ignoreIdleForTinyBreaks,
            i18n("With this option you select that the <b>short</b> breaks "
                 "shouldn't be reset when idle. This is useful, for example, "
                 "when you are reading something and you don't touch the mouse "
                 "or keyboard for the duration of a short break, but you still "
                 "want to have a short break every once in a while.") );


    readSettings();
    slotUseNoIdleTimer();
}

SetupGeneral::~SetupGeneral()
{
    delete d;
}

void SetupGeneral::slotShowTimer()
{
    d->showTimerReset->setEnabled(d->resetTimersAfterIdle->isChecked() &&
                                    !d->useNoIdleTimer->isChecked());
    d->ignoreIdleForTinyBreaks->setEnabled(d->resetTimersAfterIdle->isChecked() &&
                                    !d->useNoIdleTimer->isChecked());
}

void SetupGeneral::slotUseNoIdleTimer()
{
    d->idleSettings->setEnabled (!d->useNoIdleTimer->isChecked());
    d->resetTimersAfterIdle->setEnabled(!d->useNoIdleTimer->isChecked());
    d->ignoreIdleForTinyBreaks->setEnabled(!d->useNoIdleTimer->isChecked() &&
                                    d->resetTimersAfterIdle->isChecked());
    d->showTimerReset->setEnabled(!d->useNoIdleTimer->isChecked() &&
                                    d->resetTimersAfterIdle->isChecked());
    slotShowTimer();
}

void SetupGeneral::applySettings()
{
    KConfig* config = kapp->config();

    config->setGroup("General");
    config->writeEntry("AutoStart", d->autoStart->isChecked());

    config->setGroup("General Settings");
    config->writeEntry("ResetTimersAfterIdle", d->resetTimersAfterIdle->isChecked());
    config->writeEntry("ShowTimerReset", d->showTimerReset->isChecked());
    config->writeEntry("IgnoreIdleForTinyBreaks", d->ignoreIdleForTinyBreaks->isChecked());
    config->writeEntry("UseNoIdleTimer", d->useNoIdleTimer->isChecked());
    config->sync();
}

void SetupGeneral::readSettings()
{
    KConfig* config = kapp->config();

    config->setGroup("General");
    d->autoStart->setChecked(config->readBoolEntry("AutoStart", false));

    config->setGroup("General Settings");
    d->resetTimersAfterIdle->setChecked(config->readBoolEntry("ResetTimersAfterIdle", true));
    d->showTimerReset->setChecked(config->readBoolEntry("ShowTimerReset", false));
    d->ignoreIdleForTinyBreaks->setChecked(config->readBoolEntry("IgnoreIdleForTinyBreaks", false));
    d->useNoIdleTimer->setChecked(config->readBoolEntry("UseNoIdleTimer", false));
    d->useIdleTimer->setChecked(!d->useNoIdleTimer->isChecked());
}

#include "setupgeneral.moc"
