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

#include <qcstring.h> // FIXME
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
#include <dcopclient.h>

// Local includes.

#include "setupdcop.h"

class SetupDCOPPriv
{
public:
    QListView       *table;
    QLineEdit       *start;
    QLineEdit       *end;
    QLineEdit       *desc;
    QCheckBox       *active;
    QCheckListItem  *current;
    QPushButton     *startButton;
    QPushButton     *endButton;
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
    itemTwo->setText(1,"karm KarmDCOPIface stoptimerfor dfdfdf");
    itemTwo->setText(2,"karm KarmDCOPIface starttimerfor dfdfdf");
    d->table->insertItem(itemTwo);

    QCheckListItem* item3 = new QCheckListItem(d->table,"Kopete",
            QCheckListItem::CheckBox);
    item3->setText(1,"kopete in stop");
    item3->setText(2,"kopete in start");
    d->table->insertItem(item3);

    layout->addWidget(d->table);

    connect( d->table, SIGNAL(clicked ( QListViewItem * )),
             SLOT( slotTableClicked( QListViewItem *) ) );

    //--- settings ---//
    // TODO: put it into a box

    QGridLayout *gbox = new QGridLayout( 4, 3 );

    QLabel *l2 = new QLabel(i18n("Name:"), parent);
    QWhatsThis::add( l2, i18n("What is the name of the application") );
    d->desc = new QLineEdit(parent);
    l2->setBuddy(d->desc);
    connect( d->desc, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDescChanged(const QString&) ) );

    QLabel *l3 = new QLabel(i18n("DCOP At Break Start:"), parent);
    QWhatsThis::add( l3, i18n("Here you can set the command to issue when "
            "the break begins") );
    d->start = new QLineEdit(parent);
    l3->setBuddy(d->start);
    connect( d->start, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDCOPStartChanged(const QString&) ) );

    QLabel *l4 = new QLabel(i18n("DCOP At Break End:"), parent);
    QWhatsThis::add( l3, i18n("Here you can set the command to issue when "
            "the break ends") );
    d->end = new QLineEdit(parent);
    l4->setBuddy(d->end);
    connect( d->end, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDCOPStopChanged(const QString&) ) );

    QLabel *l5 = new QLabel(i18n("Activated:"), parent);
    QWhatsThis::add( l5, i18n("Here you can activate or deactivate the "
            "command" ) );
    d->active = new QCheckBox(parent);
    l4->setBuddy(d->active);
    connect( d->active, SIGNAL(clicked()), SLOT( slotCheckActive() ) );

    d->startButton = new QPushButton( i18n("Test"), parent);
    connect( d->startButton, SIGNAL(clicked()), SLOT( slotTestStart()));

    d->endButton = new QPushButton( i18n("Test"), parent);
    connect( d->endButton, SIGNAL(clicked()), SLOT( slotTestStop()));

    gbox->addWidget(l2,0,0);
    gbox->addWidget(d->desc,0,1);
    gbox->addWidget(l3,2,0);
    gbox->addWidget(d->start,2,1);
    gbox->addWidget(d->startButton,2,2);
    gbox->addWidget(l4,3,0);
    gbox->addWidget(d->end,3,1);
    gbox->addWidget(d->endButton,3,2);
    gbox->addWidget(l5,1,0);
    gbox->addWidget(d->active,1,1);

    layout->addLayout(gbox);
    d->desc->setEnabled(false);
    d->active->setEnabled(false);
    d->start->setEnabled(false);
    d->end->setEnabled(false);
    d->startButton->setEnabled(false);
    d->endButton->setEnabled(false);

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
    kdDebug() << "Entering slotTableClicked" << endl;

    //TODO: Someone please check this....
    d->current = dynamic_cast<QCheckListItem*>(item);
    if (!d->current)
        return;

    d->desc->setText( d->current->text(0));
    d->active->setChecked( d->current->isOn() );
    d->start->setText( d->current->text(1));
    d->end->setText( d->current->text(2));

    d->desc->setEnabled(true);
    d->active->setEnabled(true);
    d->start->setEnabled(true);
    d->end->setEnabled(true);
    d->startButton->setEnabled(true);
    d->endButton->setEnabled(true);

}

void SetupDCOP::slotCheckActive()
{
    d->active->isChecked() ? d->current->setState( QCheckListItem::On ):
                             d->current->setState( QCheckListItem::Off );
}

void SetupDCOP::slotDCOPStartChanged(const QString &text)
{
    d->current->setText(1,text);
}

void SetupDCOP::slotDCOPStopChanged(const QString &text)
{
    d->current->setText(2,text);
}

void SetupDCOP::slotDescChanged(const QString &text)
{
    d->current->setText(0,text);
}

void SetupDCOP::slotTestStart()
{
    /* FIXME
    Data is not correctly passed to the DCOP call.
    Syntax without data: "kopete" "KopeteIface" "setAway()"
    Syntax with data: "kopete" "KopeteIface" "setAway(QString)" "someStringContainingData"
    */

    kdDebug() << "execute" << d->current->text(1) << endl;
    QCString app=d->current->text(1).section(' ',0,0).utf8();
    QCString obj=d->current->text(1).section(' ',1,1).utf8();
    QCString fun=d->current->text(1).section(' ',2,2).utf8();
    QCString data=d->current->text(1).section(' ',3,3).utf8();

    if ( data.isEmpty() && fun.right(2) != "()" )
      fun += "()";

    // kapp->dcopClient()->attach();
    kdDebug() << "app " << app << " obj " << obj
              << " fun " << fun << " data " << data << endl;
    if (!kapp->dcopClient()->send(app,obj,fun, data))
        kdDebug() << "Command exectution failed" << endl;
}

void SetupDCOP::slotTestStop()
{
    kdDebug() << "execute" << d->current->text(2) << endl;
}

#include "setupdcop.moc"
