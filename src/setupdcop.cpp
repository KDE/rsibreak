/* ============================================================
 * Copyright (C) 2005-2006 by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2006 by Bram Schoenmakers <bramschoenmakers@kde.nl>
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
#include <kiconloader.h>

// Local includes.

#include "setupdcop.h"
#include "rsiglobals.h"

RSIDcopEdit::RSIDcopEdit( QWidget *parent, const char *name )
: QLineEdit( parent, name )
{
}

void RSIDcopEdit::dropEvent( QDropEvent *e )
{
    clear();
    QLineEdit::dropEvent( e );
}

class SetupDCOPPriv
{
public:
    QListView       *table;
    RSIDcopEdit     *start;
    RSIDcopEdit     *end;
    QLineEdit       *desc;
    QCheckBox       *active;
    QCheckBox       *onlyAtBigBreak;
    QCheckListItem  *current;
    QPushButton     *startButton;
    QPushButton     *endButton;
    QPushButton     *pickButton;
    QLabel          *l2;
    QLabel          *l3;
    QLabel          *l4;
    QLabel          *l5;
    QLabel          *l6;
    QLabel          *l7;
    QPopupMenu      *menu;
};

SetupDCOP::SetupDCOP(QWidget* parent )
           : QWidget(parent)
{
    d = new SetupDCOPPriv;

    KIconLoader il;

    d->menu = new QPopupMenu();

    d->menu->insertItem( il.loadIconSet("remove", KIcon::Small), i18n("Remove"), this, SLOT( slotRemoveItem() ), 0);
    d->menu->insertItem( il.loadIconSet("add", KIcon::Small), i18n("Add"), this, SLOT( slotAddNewItem() ), 1);

    QVBoxLayout *layout = new QVBoxLayout( parent );
    layout->setSpacing( KDialog::spacingHint() );
    layout->setAlignment( AlignTop );

    QLabel *l1 = new QLabel(i18n("Here you can tell RSIBreak which "
            "applications should stop when a break is starting and be "
            "restarted when the break is over"), parent);
    l1->setAlignment(Qt::WordBreak);
    layout->addWidget(l1);

    d->table = new QListView( parent,0, QListView::Single);
    d->table->addColumn( i18n("Application")  );
    d->table->setResizeMode( QListView::AllColumns );
    layout->addWidget(d->table);

    connect( d->table,
             SIGNAL( mouseButtonPressed(int, QListViewItem*,const QPoint&, int)),
             SLOT( slotTableClicked(int, QListViewItem*,const QPoint&)));

    //--- settings ---//
    // TODO: put it into a box

    QGridLayout *gbox = new QGridLayout( 4, 3 );

    d->l2 = new QLabel(i18n("Name:"), parent);
    QWhatsThis::add( d->l2, i18n("What is the name of the application") );
    d->desc = new QLineEdit(parent);
    d->l2->setBuddy(d->desc);
    connect( d->desc, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDescChanged(const QString&) ) );

    d->l3 = new QLabel(i18n("DCOP At Break Start:"), parent);
    QWhatsThis::add( d->l3, i18n("Here you can set the command to issue when "
            "the break begins") );
    d->start = new RSIDcopEdit(parent);
    d->l3->setBuddy(d->start);
    connect( d->start, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDCOPStartChanged(const QString&) ) );

    d->l4 = new QLabel(i18n("DCOP At Break End:"), parent);
    QWhatsThis::add( d->l4, i18n("Here you can set the command to issue when "
            "the break ends") );
    d->end = new RSIDcopEdit(parent);
    d->l4->setBuddy(d->end);
    connect( d->end, SIGNAL(textChanged(const QString&) ),
             SLOT(slotDCOPStopChanged(const QString&) ) );

    d->l5 = new QLabel(i18n("Activated:"), parent);
    QWhatsThis::add( d->l5, i18n("Here you can activate or deactivate the "
            "command" ) );
    d->active = new QCheckBox(parent);
    d->l5->setBuddy(d->active);
    connect( d->active, SIGNAL(clicked()), SLOT( slotCheckActive() ) );

    d->l6 = new QLabel(i18n("Only at Long Breaks:"), parent);
    QWhatsThis::add( d->l6, i18n("Here you can indicate if the command should"
            "only be executed at Long Breaks or also on Short Breaks" ) );
    d->onlyAtBigBreak = new QCheckBox(parent);
    d->l6->setBuddy(d->onlyAtBigBreak);
    connect( d->onlyAtBigBreak,SIGNAL(clicked()),SLOT( slotCheckOnlyAtBigBreak()));

    d->startButton = new QPushButton( i18n("Test"), parent);
    connect( d->startButton, SIGNAL(clicked()), SLOT( slotTestStart()));

    d->endButton = new QPushButton( i18n("Test"), parent);
    connect( d->endButton, SIGNAL(clicked()), SLOT( slotTestStop()));

    d->pickButton = new QPushButton( i18n("Start KDCOP..."), parent );
    connect( d->pickButton, SIGNAL( clicked() ), SLOT( slotPicked() ) );

    d->l7 = new QLabel( i18n("Use KDCOP to drag and drop DCOP commands to the fields above:"), parent );
    QWhatsThis::add( d->l7, i18n("KDCOP is a little application which allows to see which DCOP commands are available for all running applications. You can simply drag and drop commands to the command fields above.") );
    d->l7->setBuddy( d->pickButton );

    gbox->addWidget(d->l2,0,0, Qt::AlignRight);
    gbox->addMultiCellWidget(d->desc,0,0,1,3);
    gbox->addWidget(d->l3,2,0, Qt::AlignRight);
    gbox->addMultiCellWidget(d->start,2,2,1,2);
    gbox->addWidget(d->startButton,2,3);
    gbox->addWidget(d->l4,3,0, Qt::AlignRight);
    gbox->addMultiCellWidget(d->end,3,3,1,2);
    gbox->addWidget(d->endButton,3,3);
    gbox->addWidget(d->l5,1,0, Qt::AlignRight);
    gbox->addWidget(d->active,1,1);
    gbox->addWidget(d->l6,1,2, Qt::AlignRight);
    gbox->addWidget(d->onlyAtBigBreak,1,3);
    gbox->addMultiCellWidget(d->l7, 4,4, 0, 2, Qt::AlignRight );
    gbox->addWidget(d->pickButton, 4, 3 );

    layout->addLayout(gbox);
    d->desc->setEnabled(false);
    d->active->setEnabled(false);
    d->onlyAtBigBreak->setEnabled(false);
    d->start->setEnabled(false);
    d->end->setEnabled(false);
    d->startButton->setEnabled(false);
    d->endButton->setEnabled(false);
    d->pickButton->setEnabled(false);
    d->l2->setEnabled(false);
    d->l3->setEnabled(false);
    d->l4->setEnabled(false);
    d->l5->setEnabled(false);
    d->l6->setEnabled(false);
    d->l7->setEnabled(false);
    readSettings();

    // An example...
    if ( !d->table->firstChild() )
    {
        QCheckListItem* item = new QCheckListItem(d->table,"Kopete",
                QCheckListItem::CheckBox);
        item->setText(1,"kopete KopeteIface setAway()");
        item->setText(2,"kopete KopeteIface setAvailable()");
        item->setText(3,"true");
        d->table->insertItem(item);
    }
}

SetupDCOP::~SetupDCOP()
{
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
        data << item->text(3);
        config->writeEntry(item->text(0), data);

    }
    config->sync();
}

void SetupDCOP::readSettings()
{
    KConfig* config = kapp->config();

    QMap<QString,QString> map;
    map=config->entryMap("DCOP");

    QMap<QString, QString>::const_iterator i;
    for (i = map.constBegin(); i != map.constEnd(); ++i)
    {
        QStringList list = QStringList::split(",",i.data());

        QCheckListItem* item = new QCheckListItem(d->table,i.key(),
                QCheckListItem::CheckBox);
        item->setText(1,list[0]);
        item->setText(2,list[1]);
        list[2] == "On" ? item->setState(QCheckListItem::On) :
                          item->setState(QCheckListItem::Off);
        item->setText(3,list[3]);
        d->table->insertItem(item);

    }
}

void SetupDCOP::slotTableClicked( int button, QListViewItem * item,
                                  const QPoint & pos)
{
    d->current = static_cast<QCheckListItem*>(item);

    if (button == Qt::LeftButton)
    {
        if (!d->current)
            slotAddNewItem();
        updateEditArea();
    }
    else if (button == Qt::RightButton )
    {
        d->menu->exec(pos);
    }
}

void SetupDCOP::slotAddNewItem()
{
    QCheckListItem* item = new QCheckListItem(d->table,i18n("New"),
                                              QCheckListItem::CheckBox);
    item->setText(1,i18n("Start command"));
    item->setText(2,i18n("End command"));
    item->setText(3,"true");
    d->table->insertItem(item);
    d->table->setSelected(item, true);
    d->table->ensureItemVisible(item);
    d->current=item;
}

void SetupDCOP::slotRemoveItem()
{
    d->table->takeItem( d->current );

    d->desc->clear();
    d->desc->setEnabled(false);
    d->active->setChecked(false);
    d->active->setEnabled(false);
    d->onlyAtBigBreak->setChecked(false);
    d->onlyAtBigBreak->setEnabled(false);
    d->start->clear();
    d->start->setEnabled(false);
    d->end->clear();
    d->end->setEnabled(false);
    d->startButton->setEnabled(false);
    d->endButton->setEnabled(false);
    d->pickButton->setEnabled(false);
    d->l2->setEnabled(false);
    d->l3->setEnabled(false);
    d->l4->setEnabled(false);
    d->l5->setEnabled(false);
    d->l6->setEnabled(false);
    d->l7->setEnabled(false);
}

void SetupDCOP::updateEditArea()
{
    d->desc->setText( d->current->text(0));
    d->active->setChecked( d->current->isOn() );
    d->start->setText( d->current->text(1));
    d->end->setText( d->current->text(2));

    d->desc->setEnabled(true);
    d->active->setEnabled(true);
    d->onlyAtBigBreak->setEnabled(true);
    d->current->text(3) == "true" ? d->onlyAtBigBreak->setChecked(true) :
                                    d->onlyAtBigBreak->setChecked(false);
    d->l2->setEnabled( true );
    d->l5->setEnabled( true );
    slotCheckActive();
}

void SetupDCOP::slotCheckActive()
{
    bool b = d->active->isChecked();
    b ? d->current->setState( QCheckListItem::On ):
        d->current->setState( QCheckListItem::Off );
    d->onlyAtBigBreak->setEnabled( b );
    d->start->setEnabled( b );
    d->end->setEnabled( b );
    d->startButton->setEnabled( b );
    d->endButton->setEnabled( b );
    d->pickButton->setEnabled( b );
    d->l3->setEnabled( b );
    d->l4->setEnabled( b );
    d->l6->setEnabled( b );
    d->l7->setEnabled( b );
}

void SetupDCOP::slotCheckOnlyAtBigBreak()
{
    d->onlyAtBigBreak->isChecked() ? d->current->setText(3, "true" ):
                                     d->current->setText(3, "false" );
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

void SetupDCOP::slotPicked()
{
    KApplication::kdeinitExec( "kdcop", QStringList(), 0, 0, "kdcop" );
}

#include "setupdcop.moc"
