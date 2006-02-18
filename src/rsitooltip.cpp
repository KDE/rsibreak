/* This file is part of the KDE project
   Copyright (C) 2005-2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2006 Tom Albers <tomalbers@kde.nl>

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
  vbox->setSpacing( 5 );
  new QLabel( "<qt><strong>RSIBreak</strong></qt>", vbox );
  mTinyLeft = new QLabel( vbox );
  mBigLeft = new QLabel( vbox );

  setView( hbox );
}

RSIToolTip::~RSIToolTip()
{
  kdDebug() << "RSIToolTip::~RSIToolTip() entered" << endl;
}

void RSIToolTip::setCounters( int tiny_left, int big_left )
{
    if( m_suspended )
    {
      mTinyLeft->setText( i18n("Suspended" ) );
      mBigLeft->setText( QString::null );
    }
    else
    {
        int minutes, seconds;
        QString mString, sString;

        // Only add the line for the tiny break when there is not
        // a big break planned at the same time.
        if (tiny_left != big_left)
        {
            minutes = (int)floor(tiny_left/60);
            seconds  = tiny_left-(minutes*60);
            mString = i18n("One minute","%n minutes", minutes);
            sString = i18n("One second","%n seconds", seconds);

            if (minutes > 0 && seconds > 0)
                mTinyLeft->setText( i18n("First argument: minutes, second: seconds "
                                "both as you defined earlier",
                                "%1 and %2 remaining until next tiny break").arg(mString, sString) );

            else if ( minutes == 0 && seconds > 0 )
                mTinyLeft->setText( i18n("Argument: seconds part or minutes part as "
                                "defined earlier",
                                "%1 remaining until next tiny break").arg(sString) );

            else if ( minutes >0 && seconds == 0 )
                mTinyLeft->setText( i18n("Argument: seconds part or minutes part as "
                                "defined earlier",
                                "%1 remaining until next tiny break").arg(mString) );
            else // minutes = 0 and seconds = 0, remove the old text.
                mTinyLeft->setText( QString::null );
        }
        else // tiny_left eq. big_left, remove this line.
            mTinyLeft->setText( QString::null );

        // do the same for the big break
        minutes = (int)floor(big_left/60);
        seconds = big_left-(minutes*60);
        mString = i18n("One minute","%n minutes", minutes);
        sString = i18n("One second","%n seconds", seconds);

        if (minutes > 0 && seconds > 0)
            mBigLeft->setText( i18n("First argument: minutes, second: seconds "
                               "both as you defined earlier",
                               "%1 and %2 remaining until next big break").arg(mString, sString) );

        else if ( minutes == 0 && seconds > 0 )
            mBigLeft->setText( i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining until next big break").arg(sString) );

        else if ( minutes >0 && seconds == 0 )
            mBigLeft->setText( i18n("Argument: seconds part or minutes part as "
                               "defined earlier",
                               "%1 remaining until next big break").arg(mString) );
        else // minutes = 0 and seconds = 0, remove the old text.
            mBigLeft->setText( QString::null );
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
