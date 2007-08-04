/* 
   Copyright (C) 2007 Tom Albers <tomalbers@kde.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "boxdialog.h"
#include "rsitimer_dpms.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>

#include <KDialog>
#include <KLocale>

    // Yes, this is where you want to hack something cool ;-) Toma

BoxDialog::BoxDialog( QWidget *parent, Qt::WFlags flags)
  : QWidget( parent, flags), m_showMinimize( true ), m_disableShort( false )
{
    hide();
    m_dialog = new KDialog( this, Qt::Popup );

    // for the counter
    m_label = new QLabel(this);
    m_label->setAlignment( Qt::AlignHCenter );

    loadDialog();
}

BoxDialog::~BoxDialog()
{
}

void BoxDialog::showDialog()
{
  if ( m_dialog && !m_dialog->isHidden() )
    return;

  m_dialog->exec();
}

void BoxDialog::loadDialog()
{
  if ( m_showMinimize )
  {
    m_dialog->setButtons( KDialog::User1 | KDialog::User2 );
    m_dialog->setButtonText( KDialog::User2, i18n("Skip"));
    m_dialog->setEscapeButton( KDialog::User2 );
  }
  else
    m_dialog->setButtons( KDialog::User1 );
  m_dialog->setButtonText( KDialog::User1, i18n("Lock"));
  if ( m_disableShort || !m_showMinimize )
    m_dialog->setEscapeButton( KDialog::User1 );
  m_dialog->setMainWidget( m_label );

  QRect rect = QApplication::desktop()->availableGeometry(
                    QApplication::desktop()->screenNumber( m_dialog ) );
  m_dialog->move( rect.center().x() - m_dialog->width() / 2,
                  rect.center().y() - m_dialog->height() / 2 );

  connect( m_dialog, SIGNAL( user1Clicked() ), this, SIGNAL( lock() ) );
  connect( m_dialog, SIGNAL( user2Clicked() ), this, SIGNAL( skip() ) );
}

void BoxDialog::showMinimize( bool ok )
{
  kDebug() << k_funcinfo << ok;
  m_showMinimize = ok;
  loadDialog();
}

void BoxDialog::disableShortcut( bool ok )
{
  kDebug() << k_funcinfo << ok;
  m_disableShort = ok;
  loadDialog();
}

void BoxDialog::setLabel( const QString& time )
{
  m_label->setText( i18n("Remaining time:\n%1", time) );
}


#include "boxdialog.moc"
