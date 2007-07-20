/* ============================================================
 * Original copied from showfoto:
 *     Copyright 2005 by Gilles Caulier <caulier.gilles@free.fr>
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

// Local includes.

#include "setuptiming.h"

// QT includes.

#include <qlayout.h>
#include <q3hbox.h>
#include <q3vgroupbox.h>
#include <q3hgroupbox.h>
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
#include <knuminput.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kglobal.h>

class SetupTimingPriv
{
public:
    KIntNumInput*          tinyInterval;
    KIntNumInput*          tinyDuration;
    KIntNumInput*          bigInterval;
    KIntNumInput*          bigDuration;
    KIntNumInput*          slideInterval;
    bool                   debug;
};

SetupTiming::SetupTiming(QWidget* parent )
           : QWidget(parent)
{
    d = new SetupTimingPriv;

    Q3VBoxLayout *layout = new Q3VBoxLayout( parent );
    //TODO
    //layout->setSpacing( KDialog::spacingHint() );
    //layout->setAlignment( AlignTop );

    Q3VGroupBox *tinyBox = new Q3VGroupBox(parent);
    tinyBox->setTitle(i18n("Tiny Breaks"));

    Q3HBox *m = new Q3HBox(tinyBox);
    QLabel *l1 = new QLabel(i18n("Short break every:") + ' ', m);
    l1->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    Q3WhatsThis::add( l1, i18n("Here you can set how often you want a short "
                              "break. One minute means 60 seconds of "
                              "movement with the mouse or typing on the keyboard.") );
    d->tinyInterval = new KIntNumInput(m);
    d->tinyInterval->setRange(1,1000,1,false);
    l1->setBuddy(d->tinyInterval);
    connect(d->tinyInterval, SIGNAL(valueChanged(int)),
            SLOT(slotTinyValueChanged( int )));

    Q3HBox *m2 = new Q3HBox(tinyBox);
    QLabel *l2 = new QLabel(i18n("For a duration of:") + ' ', m2);
    l2->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    Q3WhatsThis::add( l2, i18n("Here you can set the duration of the short break.") );
    d->tinyDuration = new KIntNumInput(m2);
    d->tinyDuration->setRange(1,1000,1,false);
    l2->setBuddy(d->tinyDuration);
    layout->addWidget(tinyBox);
    connect(d->tinyDuration, SIGNAL(valueChanged(int)),
            SLOT(slotTinyDurationValueChanged( int )));

    Q3VGroupBox *bigBox = new Q3VGroupBox(parent);
    bigBox->setTitle(i18n("Big Breaks"));

    Q3HBox *m3 = new Q3HBox(bigBox);
    QLabel *l3 = new QLabel(i18n("Long break every:") + ' ', m3);
    l3->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    Q3WhatsThis::add( l3, i18n("Here you can set how often you want a long "
                              "break. One minute means 60 seconds of "
                              "movement with the mouse or typing on the keyboard") );
    d->bigInterval = new KIntNumInput(m3);
    d->bigInterval->setRange(1,1000,1,false);
    l3->setBuddy(d->bigInterval);
    connect(d->bigInterval, SIGNAL(valueChanged(int)),
            SLOT(slotBigValueChanged( int )));


    Q3HBox *m4 = new Q3HBox(bigBox);
    QLabel *l4 = new QLabel(i18n("For a duration of:") + ' ', m4);
    l4->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    Q3WhatsThis::add( l4, i18n("Here you can set the duration of the long break.") );
    d->bigDuration = new KIntNumInput(m4);
    d->bigDuration->setRange(1,1000,1,false);
    l4->setBuddy(d->bigDuration);
    connect(d->bigDuration, SIGNAL(valueChanged(int)),
            SLOT(slotBigDurationValueChanged( int )));

    layout->addWidget(bigBox);

    Q3VGroupBox *slideBox = new Q3VGroupBox(parent);
    slideBox->setTitle(i18n("Slideshow"));

    Q3HBox *m5 = new Q3HBox(slideBox);
    QLabel *l5 = new QLabel(i18n("Change images every:") + ' ', m5);
    l5->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    Q3WhatsThis::add( l5, i18n("Here you can set how long one image should be "
                              "shown before it is replaced by the next one."));
    d->slideInterval = new KIntNumInput(m5);
    d->slideInterval->setRange(3,1000,1,false);
    l5->setBuddy(d->slideInterval);
    connect(d->slideInterval, SIGNAL(valueChanged(int)),
            SLOT(slotSlideIntervalValueChanged( int )));

    layout->addWidget(slideBox);
    layout->addStretch(10);
    readSettings();

    // set the suffix
    slotTinyValueChanged( d->tinyInterval->value() );
    slotBigValueChanged( d->bigInterval->value() );
    slotTinyDurationValueChanged( d->tinyDuration->value() );
    slotBigDurationValueChanged( d->bigInterval->value() );
    slotSlideIntervalValueChanged( d->slideInterval->value() );

    // Resize to minimum possible.
    d->tinyInterval->setFixedSize( d->tinyInterval->minimumSizeHint()  );
    d->bigInterval->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->tinyDuration->setFixedSize( d->tinyInterval->minimumSizeHint() );
    d->bigDuration->setFixedSize( d->tinyInterval->minimumSizeHint()  );
    d->slideInterval->setFixedSize( d->tinyInterval->minimumSizeHint()  );
}

SetupTiming::~SetupTiming()
{
    delete d;
}

void SetupTiming::applySettings()
{
    KConfigGroup config = KGlobal::config()->group("General Settings");
    config.writeEntry("TinyInterval", d->tinyInterval->value());
    config.writeEntry("TinyDuration", d->tinyDuration->value());
    config.writeEntry("BigInterval", d->bigInterval->value());
    config.writeEntry("BigDuration", d->bigDuration->value());
    config.writeEntry("SlideInterval", d->slideInterval->value());
    config.sync();
}

void SetupTiming::readSettings()
{
    KConfigGroup config = KGlobal::config()->group("General Settings");
    d->debug = config.readEntry("DEBUG", false);

    d->tinyInterval->setValue(config.readEntry("TinyInterval", 10));
    d->tinyDuration->setValue(config.readEntry("TinyDuration", 20));
    d->bigInterval->setValue(config.readEntry("BigInterval", 60));
    d->bigInterval->setMinimum( d->tinyInterval->value() );
    d->bigDuration->setValue(config.readEntry("BigDuration", 1));
    d->slideInterval->setValue(config.readEntry("SlideInterval", 10));
}

void SetupTiming::slotTinyValueChanged( int i )
{
    d->bigInterval->setMinimum( i );

    d->debug ? d->tinyInterval->setSuffix( ' ' + i18np("second","seconds",i) )
             : d->tinyInterval->setSuffix( ' ' + i18np("minute","minutes",i) );

}

void SetupTiming::slotBigValueChanged( int i )
{
    d->debug ? d->bigInterval->setSuffix( ' ' + i18np("second","seconds",i) )
             : d->bigInterval->setSuffix( ' ' + i18np("minute","minutes",i) );
}

void SetupTiming::slotTinyDurationValueChanged( int i )
{
    d->tinyDuration->setSuffix( ' ' + i18np("second","seconds",i) );
}

void SetupTiming::slotBigDurationValueChanged( int i )
{
    d->debug ? d->bigDuration->setSuffix( ' ' + i18np("second","seconds",i) )
             : d->bigDuration->setSuffix( ' ' + i18np("minute","minutes",i) );

}

void SetupTiming::slotSlideIntervalValueChanged( int i )
{
    d->slideInterval->setSuffix( ' ' + i18np("second","seconds",i) );
}

#include "setuptiming.moc"
