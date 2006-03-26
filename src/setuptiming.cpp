/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Copyright (C) 2005-2006 by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
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
#include <qhbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kfiledialog.h>

// Local includes.

#include "setuptiming.h"

class SetupTimingPriv
{
public:
    KIntNumInput*          tinyInterval;
    KIntNumInput*          tinyDuration;
    KIntNumInput*          bigInterval;
    KIntNumInput*          bigDuration;
    KIntNumInput*          slideInterval;
};

SetupTiming::SetupTiming(QWidget* parent )
           : QWidget(parent)
{
    kdDebug() << "Entering SetupTiming" << endl;
    d = new SetupTimingPriv;
    
    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );
    
    QVGroupBox *tinyBox = new QVGroupBox(parent);
    tinyBox->setTitle(i18n("Tiny Breaks"));
    
    QHBox *m = new QHBox(tinyBox);
    QLabel *l1 = new QLabel(i18n("Tiny break every:"), m);
    QWhatsThis::add( l1, i18n("Here you can set how often you want a tiny "
                              "break. One minute means 60 seconds of "
                              "movement with the mouse or work on the keyboard.") );
    d->tinyInterval = new KIntNumInput(m);
    d->tinyInterval->setRange(1,1000,1,false);
    d->tinyInterval->setSuffix( " " + i18n("minutes") );
    l1->setBuddy(d->tinyInterval);
    connect(d->tinyInterval, SIGNAL(valueChanged(int)),
            SLOT(slotTinyValueChanged( int )));
    
    QHBox *m2 = new QHBox(tinyBox);
    QLabel *l2 = new QLabel(i18n("For a duration of:"), m2);
    QWhatsThis::add( l2, i18n("Here you can set how long the tiny break is.") );
    d->tinyDuration = new KIntNumInput(m2);
    d->tinyDuration->setRange(1,1000,1,false);
    d->tinyDuration->setSuffix( " " + i18n("seconds") );
    l2->setBuddy(d->tinyDuration);
    layout->addWidget(tinyBox);
    
    QVGroupBox *bigBox = new QVGroupBox(parent);
    bigBox->setTitle(i18n("Big Breaks"));
    
    QHBox *m3 = new QHBox(bigBox);
    QLabel *l3 = new QLabel(i18n("Big break every:"), m3);
    QWhatsThis::add( l3, i18n("Here you can set how often you want a big "
                              "break. One minute means 60 seconds of "
                              "movement with the mouse or work on the keyboard") );
    d->bigInterval = new KIntNumInput(m3);
    d->bigInterval->setRange(1,1000,1,false);
    d->bigInterval->setSuffix( " " + i18n("minutes") );
    l3->setBuddy(d->bigInterval);
    
    QHBox *m4 = new QHBox(bigBox);
    QLabel *l4 = new QLabel(i18n("For a duration of:"), m4);
    QWhatsThis::add( l4, i18n("Here you can set how long the big break is.") );
    d->bigDuration = new KIntNumInput(m4);
    d->bigDuration->setRange(1,1000,1,false);
    d->bigDuration->setSuffix( " " + i18n("minutes") );
    l4->setBuddy(d->bigDuration);
    
    layout->addWidget(bigBox);
    
    QVGroupBox *slideBox = new QVGroupBox(parent);
    slideBox->setTitle(i18n("Slideshow"));
    
    QHBox *m5 = new QHBox(slideBox);
    QLabel *l5 = new QLabel(i18n("Change images every:"), m5);
    QWhatsThis::add( l5, i18n("Here you can set how long one image should be "
                              "shown before it is replaced by the next one."));
    d->slideInterval = new KIntNumInput(m5);
    d->slideInterval->setRange(3,1000,1,false);
    d->slideInterval->setSuffix( " " + i18n("seconds") );
    l5->setBuddy(d->slideInterval);
    
    layout->addWidget(slideBox);
    layout->addStretch(10);
    readSettings();
}

SetupTiming::~SetupTiming()
{
    kdDebug() << "Entering ~SetupTiming" << endl;
    delete d;
}

void SetupTiming::applySettings()
{
    kdDebug() <<"save"<< endl;
    KConfig* config = kapp->config();
    config->setGroup("General Settings");
    config->writeEntry("TinyInterval", d->tinyInterval->value());
    config->writeEntry("TinyDuration", d->tinyDuration->value());
    config->writeEntry("BigInterval", d->bigInterval->value());
    config->writeEntry("BigDuration", d->bigDuration->value());
    config->writeEntry("SlideInterval", d->slideInterval->value());
    config->sync();
}

void SetupTiming::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();

    config->setGroup("General Settings");
    d->tinyInterval->setValue(config->readNumEntry("TinyInterval", 10));
    d->tinyDuration->setValue(config->readNumEntry("TinyDuration", 20));
    d->bigInterval->setValue(config->readNumEntry("BigInterval", 60));
    d->bigInterval->setMinValue( d->tinyInterval->value() );
    d->bigDuration->setValue(config->readNumEntry("BigDuration", 1));
    d->slideInterval->setValue(config->readNumEntry("SlideInterval", 2));

    if (config->readBoolEntry("DEBUG"))
    {
        d->bigDuration->setSuffix( " " + i18n("seconds") );
        d->tinyInterval->setSuffix( " " + i18n("seconds") );
        d->bigInterval->setSuffix( " " + i18n("seconds") );
    }
}

void SetupTiming::slotTinyValueChanged( int i )
{
    kdDebug() << "Entering slotTinyValueChanged " << i << endl;
    d->bigInterval->setMinValue( i );
}
    
#include "setuptiming.moc"
