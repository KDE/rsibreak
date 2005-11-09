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
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * ============================================================ */

// QT includes.

#include <qlayout.h>
#include <qcolor.h>
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <kfiledialog.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kapplication.h>

// Local includes.

#include "setuptiming.h"


SetupTiming::SetupTiming(QWidget* parent )
           : QWidget(parent)
{
   kdDebug() << "Entering SetupTiming" << endl;
   QVBoxLayout *layout = new QVBoxLayout( parent );
   layout->setSpacing( KDialog::spacingHint() );

   QVGroupBox *tinyBox = new QVGroupBox(parent);
   tinyBox->setTitle(i18n("Tiny breaks"));

   QHBox *m = new QHBox(tinyBox);
   new QLabel(i18n("Bother you every:"), m);
   m_tinyInterval = new KIntNumInput(m);
   m_tinyInterval->setRange(1,1000,1,false);
   m_tinyInterval->setSuffix( " " + i18n("minutes") );

   QHBox *m2 = new QHBox(tinyBox);
   new QLabel(i18n("For the amount of:"), m2);
   m_tinyDuration = new KIntNumInput(m2);
   m_tinyDuration->setRange(1,1000,1,false);
   m_tinyDuration->setSuffix( " " + i18n("seconds") );

   layout->addWidget(tinyBox);

   QVGroupBox *bigBox = new QVGroupBox(parent);
   bigBox->setTitle(i18n("Big breaks"));

   QHBox *m3 = new QHBox(bigBox);
   new QLabel(i18n("Big break after:"), m3);
   m_bigInterval = new KIntNumInput(m3);
   m_bigInterval->setRange(1,1000,1,false);
   m_bigInterval->setSuffix( " " + i18n("tiny breaks") );

   QHBox *m4 = new QHBox(bigBox);
   new QLabel(i18n("For the amount of:"), m4);
   m_bigDuration = new KIntNumInput(m4);
   m_bigDuration->setRange(1,1000,1,false);
   m_bigDuration->setSuffix( " " + i18n("minutes") );

   layout->addWidget(bigBox);

   QVGroupBox *slideBox = new QVGroupBox(parent);
   slideBox->setTitle(i18n("Slideshow"));

   QHBox *m5 = new QHBox(slideBox);
   new QLabel(i18n("Change images every:"), m5);
   m_slideInterval = new KIntNumInput(m5);
   m_slideInterval->setRange(1,1000,1,false);
   m_slideInterval->setSuffix( " " + i18n("seconds") );

   layout->addWidget(slideBox);
   layout->addStretch(10);
   readSettings();
}

SetupTiming::~SetupTiming()
{
kdDebug() << "Entering ~SetupTiming" << endl;
}

void SetupTiming::applySettings()
{
    kdDebug() <<"save"<< endl;
    KConfig* config = kapp->config();
    config->setGroup("General Settings");
    config->writeEntry("TinyInterval", m_tinyInterval->value());
    config->writeEntry("TinyDuration", m_tinyDuration->value());
    config->writeEntry("BigInterval", m_bigInterval->value());
    config->writeEntry("BigDuration", m_bigDuration->value());
    config->writeEntry("SlideInterval", m_slideInterval->value());
    config->sync();
}

void SetupTiming::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General Settings");
    m_tinyInterval->setValue(config->readNumEntry("TinyInterval", 2));
    m_tinyDuration->setValue(config->readNumEntry("TinyDuration", 20));
    m_bigInterval->setValue(config->readNumEntry("BigInterval", 10));
    m_bigDuration->setValue(config->readNumEntry("BigDuration", 1));
    m_slideInterval->setValue(config->readNumEntry("SlideInterval", 2));

    if (config->readBoolEntry("DEBUG"))
    {
        m_bigDuration->setSuffix( " " + i18n("seconds") );
        m_tinyInterval->setSuffix( " " + i18n("seconds") );
    }
}

#include "setuptiming.moc"
