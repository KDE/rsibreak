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
#include <qpushbutton.h>
#include <qtimer.h>
#include <qvbox.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kprogress.h>
#include <kapplication.h>
#include <kconfig.h>

#include "rsipopup.h"

RSIPopup::RSIPopup( QWidget *parent, const char *name )
: KPassivePopup( parent, name ), m_resetcount( 0 )
{
    kdDebug() << "Entering RSIPopup::RSIPopup" << endl;

    QVBox *vbox = new QVBox( this );
    vbox->setSpacing( KDialog::spacingHint() );
    
    m_message = new QLabel( vbox );
    
    QHBox *hbox = new QHBox( vbox );
    hbox->setSpacing( 5 );
    
    m_progress = new KProgress( hbox );
    m_progress->setPercentageVisible( false );
    m_progress->setTotalSteps( 0 );
    
    m_lockbutton = new QPushButton( SmallIcon( "lock" ), QString::null, hbox );
    connect( m_lockbutton, SIGNAL( clicked() ), SIGNAL( lock() ) );
    
    m_flashtimer = new QTimer( this );
    connect( m_flashtimer, SIGNAL( timeout() ), SLOT( unflash() ) );
    
    setView( vbox );
    readSettings();
}

RSIPopup::~RSIPopup()
{
    kdDebug() << "Entering RSIPopup::~RSIPopup()" << endl;
}

void RSIPopup::relax( int n )
{
    kdDebug() << "Entering RSIPopup::relax()" << endl;

    if (!m_usePopup)
        return;
    
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
    
    if ( m_useFlash )
    {
        m_flashtimer->start( 500, true );
        setPaletteForegroundColor( QColor( 255, 255, 255 ) );
        setPaletteBackgroundColor( QColor( 0, 0, 120 ) );
    }
}

void RSIPopup::unflash()
{
    kdDebug() << "Entering RSIPopup::unflash()" << endl;
    unsetPalette();
}

void RSIPopup::mouseReleaseEvent( QMouseEvent * )
{
    kdDebug() << "Entering RSIPopup::mousePressEvent()" << endl;

    /* eat this! */
}

void RSIPopup::slotReadConfig()
{
    kdDebug() << "Entering RSIPopup::slotReadConfig" << endl;
    readSettings();
}

void RSIPopup::readSettings()
{
    kdDebug() << "Entering readSettings" << endl;
    KConfig* config = kapp->config();
    config->setGroup("Popup Settings");
    m_usePopup=config->readBoolEntry("UsePopup", true);
    m_useFlash=config->readBoolEntry("UseFlash", true);
}
#include "rsipopup.moc"
