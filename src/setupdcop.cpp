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
#include <qpopupmenu.h>

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
#include "rsiglobals.h"

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

    QLabel *l1 = new QLabel(i18n("Here you can tell RSIBreak which"
            "applications should stop when a break is starting\nand be "
            "restarted when the break is over"), parent);
    layout->addWidget(l1);

    d->table = new QListView( parent,0, QListView::Single);
    d->table->addColumn( i18n("Application") );
    d->table->addColumn( i18n("Start") );
    d->table->addColumn( i18n("End") );
    d->table->setAllColumnsShowFocus( true );
    d->table->setColumnWidthMode(1,QListView::Manual);
    d->table->setColumnWidthMode(2,QListView::Manual);

    layout->addWidget(d->table);

    connect( d->table,
             SIGNAL( mouseButtonPressed(int, QListViewItem*,const QPoint&, int)),
             SLOT( slotTableClicked(int, QListViewItem*,const QPoint&)));

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

    // Some examples...
    if ( !d->table->firstChild() )
    {
        QCheckListItem* item = new QCheckListItem(d->table,"KArm",
                QCheckListItem::CheckBox);
        item->setText(1,"karm KarmDCOPIface stoptimerfor replacewithtask");
        item->setText(2,"karm KarmDCOPIface starttimerfor replacewithtask");
        d->table->insertItem(item);

        QCheckListItem* item3 = new QCheckListItem(d->table,"Kopete",
                QCheckListItem::CheckBox);
        item3->setText(1,"kopete KopeteIface setAway()");
        item3->setText(2,"kopete KopeteIface setAvailable()");
        d->table->insertItem(item3);
    }

    // now that the table contains data, set the width and stick to it.
    d->table->adjustColumn(1);
    d->table->adjustColumn(2);

}

SetupDCOP::~SetupDCOP()
{
    kdDebug() << "Entering ~SetupDCOP" << endl;
    delete d;
}

void SetupDCOP::applySettings()
{
    if ( !d->table->firstChild() )
        return;

    KConfig* config = kapp->config();
    config->deleteGroup("DCOP");
    config->sync();
    config->setGroup("DCOP");

    QListViewItemIterator it( d->table );
    for ( ;it.current(); ++it )
    {
        QCheckListItem *item = dynamic_cast<QCheckListItem*>(it.current());
        QStringList data;
        data << item->text(1) << item->text(2);
        item->isOn() ? data << "On" : data << "Off";
        kdDebug() << "Writing" << item->text(0) << data << endl;
        config->writeEntry(item->text(0), data);

    }
    config->sync();
}

void SetupDCOP::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();

    QMap<QString,QString> map;
    map=config->entryMap("DCOP");

    QMap<QString, QString>::const_iterator i;
    for (i = map.constBegin(); i != map.constEnd(); ++i)
    {
        kdDebug() << i.key() << ": " << i.data() << endl;
        QStringList list = QStringList::split(",",i.data());

        QCheckListItem* item = new QCheckListItem(d->table,i.key(),
                QCheckListItem::CheckBox);
        item->setText(1,list[0]);
        item->setText(2,list[1]);
        list[2] == "On" ? item->setState(QCheckListItem::On) :
                          item->setState(QCheckListItem::Off);

        d->table->insertItem(item);

    }
}

void SetupDCOP::slotTableClicked( int button, QListViewItem * item,
                                  const QPoint & pos)
{
    kdDebug() << "Entering slotTableClicked " << button << endl;

    //TODO: Someone please check this....
    d->current = dynamic_cast<QCheckListItem*>(item);

    if (button == Qt::LeftButton)
    {
        if (!d->current)
            slotAddNewItem();
        updateEditArea();
    }
    else if (button == Qt::RightButton )
    {
        kdDebug() << "conext please...." << endl;
        QPopupMenu *menu = new QPopupMenu();
        if (d->current)
            menu->insertItem(i18n("Remove"), 0);
        menu->insertItem(i18n("Add"), 1);

        int i = menu->exec(pos);

        if (i == 0)
        {
            d->table->takeItem( d->current );

            d->desc->clear();
            d->desc->setEnabled(false);
            d->active->setChecked(false);
            d->active->setEnabled(false);
            d->start->clear();
            d->start->setEnabled(false);
            d->end->clear();
            d->end->setEnabled(false);
            d->startButton->setEnabled(false);
            d->endButton->setEnabled(false);
        }
        else if (i == 1)
            slotAddNewItem();
    }
}

void SetupDCOP::slotAddNewItem()
{
    QCheckListItem* item = new QCheckListItem(d->table,i18n("New"),
                                              QCheckListItem::CheckBox);
    item->setText(1,i18n("Start command"));
    item->setText(2,i18n("End command"));
    d->table->insertItem(item);
    d->table->setSelected(item, true);
    d->table->ensureItemVisible(item);
    d->current=item;
}

void SetupDCOP::updateEditArea()
{
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
    RSIGlobals::instance()->executeDCOP(d->current->text(1));
}

void SetupDCOP::slotTestStop()
{
    RSIGlobals::instance()->executeDCOP(d->current->text(2));
}

#include "setupdcop.moc"
