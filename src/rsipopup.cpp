/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#include <qlabel.h>
#include <qtimer.h>
#include <qvbox.h>

#include <kdebug.h>
#include <klocale.h>
#include <kprogress.h>

#include "rsipopup.h"

RSIPopup::RSIPopup( QWidget *parent, const char *name )
: KPassivePopup( parent, name ), m_resetcount( 0 )
{
  kdDebug() << "Entering RSIPopup::RSIPopup" << endl;

  QVBox *box = new QVBox( this );
  box->setSpacing( KDialog::spacingHint() );

  m_message = new QLabel( box );

  m_progress = new KProgress( box );
  m_progress->setPercentageVisible( false );
  m_progress->setTotalSteps( 0 );

  m_flashtimer = new QTimer( this );
  connect( m_flashtimer, SIGNAL( timeout() ), SLOT( unflash() ) );

  setView( box );
}

RSIPopup::~RSIPopup()
{
  kdDebug() << "Entering RSIPopup::~RSIPopup()" << endl;
}

void RSIPopup::relax( int n )
{
  kdDebug() << "Entering RSIPopup::relax()" << endl;

  /*
    If n increases compared to the last call,
    we want a new request for a relax moment.
  */
  if ( n >= m_progress->progress() ) {
    m_progress->setTotalSteps( n );
    m_resetcount += 1;
    if( n > m_progress->progress() )
      flash();
    else if ( m_resetcount % 4 == 0 ) // flash regulary when the user keeps working
      flash();
  }

  if ( n > 0 )
  {
    m_message->setText(i18n("Please relax for 1 second",
                       "Please relax for %n seconds",
                       n ));

    m_progress->setProgress( n );
    show();
  }
  else
  {
    hide();
    m_resetcount = 0;
  }
}

void RSIPopup::flash()
{
  kdDebug() << "Entering RSIPopup::flash()" << endl;
  m_flashtimer->start( 500, true );
  setPaletteForegroundColor( QColor( 255, 255, 255 ) );
  setPaletteBackgroundColor( QColor( 0, 0, 120 ) );
}

void RSIPopup::unflash()
{
  kdDebug() << "Entering RSIPopup::unflash()" << endl;
  unsetPalette();
}

#include "rsipopup.moc"
