/* This file is part of the KDE project
   Copyright (C) 2005-2007 Tom Albers <tomalbers@kde.nl>
   Copyright (C) 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#include "graywidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QPainter>
#include <QTimer>
#include <QLabel>

#include <kpixmapeffect.h>
#include <KDialog>
#include <KLocale>
#include <KAction>

GrayWidget::GrayWidget( QWidget *parent )
  : QWidget( parent, Qt::Popup), m_currentY( 0 ), m_showMinimize( true ),
        m_disableShort( false )
{
    // full screen
    setAttribute( Qt::WA_NoSystemBackground );
    QRect rect = QApplication::desktop()->screenGeometry(
                        QApplication::desktop()->primaryScreen() );
    setGeometry( rect );

    // for the counter
    m_label = new QLabel(this);
    m_label->setAlignment( Qt::AlignHCenter );

    // prepare the dialog in the middle.
    // Yes, this is where you want to hack something cool ;-) Toma
    m_dialog = new KDialog( this, Qt::Popup );
    loadDialog();
}

GrayWidget::~GrayWidget()
{
}

void GrayWidget::reset()
{
  m_currentY = 0;
  m_dialog->hide();
}

void GrayWidget::showMinimize( bool ok )
{
  kDebug() << k_funcinfo << ok << endl;
  m_showMinimize = ok;
  loadDialog();
}

void GrayWidget::disableShortcut( bool ok )
{
  kDebug() << k_funcinfo << ok <<endl;
  m_disableShort = ok;
  loadDialog();
}


void GrayWidget::setLabel( const QString& time )
{
  m_label->setText( i18n("Remaining time:\n%1", time) );
}

// This slot and the paint event is partly copied from KDE's logout screen.
// from various authors found in:
// /KDE/4/kdebase/workspace/ksmserver/shutdowndlg.cpp
void GrayWidget::slotGrayEffect()
{
    if ( m_currentY >= height() )
    {
        showDialog();
        return;
    }
    repaint();
    m_currentY += 15;
}

void GrayWidget::paintEvent( QPaintEvent* )
{
    if ( m_currentY >= height() )
      return;

    static QPixmap complete( width(), height() );
    QPixmap below(width(),(height()-15));

    if ( m_currentY == 0 )
    {
      complete = QPixmap::grabWindow( QX11Info::appRootWindow(), 0, 0, width(), height() );
      below = complete.copy(0, 15, width(), height()-15);
    }

    // this part we want to process...
    QPixmap change(width(),15);
    change = complete.copy(0, m_currentY, width(),15);
    change = KPixmapEffect::fade( change, 0.4, Qt::black );
    change = KPixmapEffect::toGray( change, true );

    QPainter painter( this );
    painter.drawPixmap( 0, m_currentY, change );

    if ( m_currentY == 0 )
      painter.drawPixmap( 0, 15, below );

    QTimer::singleShot(10,this,SLOT(slotGrayEffect()));
}

void GrayWidget::showDialog()
{
  if ( m_dialog && !m_dialog->isHidden() )
    return;

  m_dialog->exec();
  hide();
}

void GrayWidget::loadDialog()
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

#include "graywidget.moc"
