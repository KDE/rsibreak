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
#include <qpixmap.h>
#include <qvbox.h>

#include <kdebug.h>
#include <klocale.h>

#include <math.h>

#include "rsitooltip.h"

RSIToolTip::RSIToolTip( QWidget *parent, const char *name )
  : KPassivePopup( parent, name ), m_suspended( false )
{
  kdDebug() << "RSIToolTip::RSIToolTip() entered" << endl;
  setTimeout( 10 * 1000 );

  QHBox *hbox = new QHBox( this );
  hbox->setSpacing( 10 );

  mIcon = new QLabel( hbox );
  mIcon->resize( 32, 32 );

  QVBox *vbox = new QVBox( hbox );
  new QLabel( "<qt><strong>RSIBreak</strong></qt>", vbox );
  mTimeLeft = new QLabel( vbox );
  mInterval = new QLabel( vbox );

  setView( hbox );
}

RSIToolTip::~RSIToolTip()
{
  kdDebug() << "RSIToolTip::~RSIToolTip() entered" << endl;
}

void RSIToolTip::setCounters( const QTime &time, const int currentBreak )
{
    int s = (int)ceil(QTime::currentTime().msecsTo( time )/1000);

    if( m_suspended )
    {
      mTimeLeft->setText( i18n("Suspended" ) );
      mInterval->setText( QString::null );
    }
    else if (s > 0)
    {
        int minutes = (int)floor(s/60);
        int seconds  = s-(minutes*60);
        QString mString = i18n("One minute","%n minutes",minutes);
        QString sString = i18n("One second","%n seconds",seconds);
        QString finalString;

        if (minutes > 0 && seconds > 0)
        {
            mTimeLeft->setText( i18n("First argument: minutes, second: seconds "
                               "both as you defined earlier",
                               "%1 and %2 remaining").arg(mString, sString) );
        }
        else if ( minutes == 0 && seconds > 0 )
        {
            mTimeLeft->setText( i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining").arg(sString) );
        }
        else if ( minutes >0 && seconds == 0 )
        {
            mTimeLeft->setText( i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining").arg(mString) );
        }

        if (currentBreak - 1 == 0)
            mInterval->setText( i18n("Next break is a big break") );
        else
            mInterval->setText( "\n" + i18n("Big break after next break",
                             "Big break after %n breaks", currentBreak - 1) );
    }
}

void RSIToolTip::setPixmap( const QPixmap &pix )
{
  kdDebug() << "RSIToolTip::setPixmap() entered" << endl;

  mIcon->setPixmap( pix );
}

void RSIToolTip::setSuspended( bool b )
{
  m_suspended = b;
}

#include "rsitooltip.moc"
