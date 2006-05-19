/* ============================================================
 * Copyright (C) 2005-2006 by Tom Albers <tomalbers@kde.nl>
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
#include <qlistview.h>

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

#include "setupdcop.h"

class SetupDCOPPriv
{
public:
    QListView   *table;
    QLineEdit   *start;
    QLineEdit   *end;
    QLineEdit   *desc;
    QCheckBox   *active;
};

SetupDCOP::SetupDCOP(QWidget* parent )
           : QWidget(parent)
{
    d = new SetupDCOPPriv;

    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );

    QLabel *l1 = new QLabel(i18n("Here you can tell RSIBreak which\n"
            "applications should stop when a break is starting\n and be "
            "restarted when the break is over"), parent);
    layout->addWidget(l1);

    d->table = new QListView( parent,0, QListView::Single);
    d->table->addColumn( i18n("Application") );
    d->table->addColumn( i18n("Start") );
    d->table->addColumn( i18n("End") );

    QCheckListItem* itemTwo = new QCheckListItem(d->table,"KArm",
            QCheckListItem::CheckBox);
    itemTwo->setText(1,"dcop karm stop");
    itemTwo->setText(2,"dcop karm start");
    d->table->insertItem(itemTwo);

    QCheckListItem* item3 = new QCheckListItem(d->table,"KOpete",
            QCheckListItem::CheckBox);
    item3->setText(1,"dcop kopete stop");
    item3->setText(2,"dcop kopete start");
    d->table->insertItem(item3);

    layout->addWidget(d->table);

    connect( d->table, SIGNAL(clicked ( QListViewItem * )),
             SLOT( slotTableClicked( QListViewItem *) ) );

    //--- settings ---//
    // TODO: put it into a box

    QGridLayout *gbox = new QGridLayout( 4, 2 );

    QLabel *l2 = new QLabel(i18n("Name:"), parent);
    QWhatsThis::add( l2, i18n("What is the name of the application") );
    d->desc = new QLineEdit(parent);
    l2->setBuddy(d->desc);

    QLabel *l3 = new QLabel(i18n("At Break Start:"), parent);
    QWhatsThis::add( l3, i18n("Here you can set the command to issue when "
            "the break begins") );
    d->start = new QLineEdit(parent);
    l3->setBuddy(d->start);

    QLabel *l4 = new QLabel(i18n("At Break End:"), parent);
    QWhatsThis::add( l3, i18n("Here you can set the command to issue when "
            "the break ends") );
    d->end = new QLineEdit(parent);
    l4->setBuddy(d->end);

    QLabel *l5 = new QLabel(i18n("Activated:"), parent);
    QWhatsThis::add( l5, i18n("Here you can activate or deactivate the "
            "command" ) );
    d->active = new QCheckBox(parent);
    l4->setBuddy(d->active);

    gbox->addWidget(l2,0,0);
    gbox->addWidget(d->desc,0,1);
    gbox->addWidget(l3,2,0);
    gbox->addWidget(d->start,2,1);
    gbox->addWidget(l4,3,0);
    gbox->addWidget(d->end,3,1);
    gbox->addWidget(l5,1,0);
    gbox->addWidget(d->active,1,1);

    layout->addLayout(gbox);
    readSettings();

}

SetupDCOP::~SetupDCOP()
{
    kdDebug() << "Entering ~SetupDCOP" << endl;
    delete d;
}

void SetupDCOP::applySettings()
{
    kdDebug() <<"save"<< endl;
    KConfig* config = kapp->config();
    config->setGroup("General Settings");
    config->sync();
}

void SetupDCOP::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    config->setGroup("General Settings");
}

void SetupDCOP::slotTableClicked( QListViewItem * item )
{
    kdDebug() << "KLIK" << endl;
    //TODO: Someone please check this....
    QCheckListItem *i = dynamic_cast<QCheckListItem*>(item);
    if (!i)
        return;

    d->desc->setText( i->text(0));
    d->active->setChecked( i->isOn() );
    d->start->setText( i->text(1));
    d->end->setText( i->text(2));
}

#include "setupdcop.moc"
