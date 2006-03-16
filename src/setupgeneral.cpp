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
#include <kshortcutdialog.h>

// Local includes.

#include "setupgeneral.h"

SetupGeneral::SetupGeneral(QWidget* parent )
           : QWidget(parent)
{
   kdDebug() << "Entering SetupGeneral" << endl;
   QVBoxLayout *layout = new QVBoxLayout( parent );
   layout->setSpacing( KDialog::spacingHint() );
   layout->setAlignment( AlignTop );

   m_autoStart = new QCheckBox(i18n("&Automatically start RSIBreak at startup"), parent);
   QWhatsThis::add( m_autoStart, i18n("With this checkbox you can indicate "
           "that you want RSIBreak to start when KDE starts.") );
   layout->addWidget(m_autoStart);
   
   m_useIdleDetection = new QCheckBox(i18n("&Use idle-detection"), parent);
   QWhatsThis::add( m_useIdleDetection, i18n("With this checkbox you indicate "
           "that you want to use idle detection. Unchecked RSIBreak will break "
	   "at fixed times.") );
   layout->addWidget(m_useIdleDetection);
   connect(m_useIdleDetection , SIGNAL(toggled(bool)), SLOT(slotShowTimer()));
   
   m_showTimerReset = new QCheckBox(i18n("&Show when timers are reset"), parent);
   QWhatsThis::add( m_showTimerReset, i18n("With this checkbox you indicate "
           "that you want to see when the timers are reset. This happens when you "
           "have been idle for a while.") );
   layout->addWidget(m_showTimerReset);
   
   readSettings();
   slotShowTimer();
}

SetupGeneral::~SetupGeneral()
{
    kdDebug() << "Entering ~SetupGeneral" << endl;
}

void SetupGeneral::slotShowTimer()
{
    m_showTimerReset->setEnabled(m_useIdleDetection->isChecked());
}

void SetupGeneral::applySettings()
{
    kdDebug() << "Entering applySettings" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General");
    config->writeEntry("AutoStart", m_autoStart->isChecked());
    
    config->setGroup("General Settings");
    config->writeEntry("UseIdleDetection", m_useIdleDetection->isChecked());
    config->writeEntry("ShowTimerReset", m_showTimerReset->isChecked());
    config->sync();
}

void SetupGeneral::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    
    config->setGroup("General");
    m_autoStart->setChecked(config->readBoolEntry("AutoStart", false));
    
    config->setGroup("General Settings");
    m_useIdleDetection->setChecked(config->readBoolEntry("UseIdleDetection", true));
    m_showTimerReset->setChecked(config->readBoolEntry("ShowTimerReset", true));
}

#include "setupgeneral.moc"
