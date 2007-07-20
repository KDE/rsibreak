/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   Copyright (C) 2005-2006 Tom Albers <tomalbers@kde.nl>

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

#include "rsirelaxpopup.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <q3vbox.h>
#include <qtooltip.h>
#include <QMouseEvent>
#include <QProgressBar>

#include <kdebug.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>

RSIRelaxPopup::RSIRelaxPopup( QWidget *parent, const char *name )
: KPassivePopup( parent )
{
    Q3VBox *vbox = new Q3VBox( this );
    // TODO: 
    //vbox->setSpacing( KDialog::spacingHint() );

    m_message = new QLabel( vbox );

    Q3HBox *hbox = new Q3HBox( vbox );
    hbox->setSpacing( 5 );

    m_progress = new QProgressBar( hbox );
    m_progress->setFormat();
    m_progress->setRange( 0, 0 );

    m_lockbutton = new QPushButton( SmallIcon( "lock" ), QString(), hbox );
    connect( m_lockbutton, SIGNAL( clicked() ), SIGNAL( lock() ) );
    QToolTip::add( m_lockbutton, i18n( "Lock the session") );
    m_skipbutton = new QPushButton( SmallIcon( "cancel" ), QString(), hbox );
    connect( m_skipbutton, SIGNAL( clicked() ), SIGNAL ( skip() ) );
    QToolTip::add( m_skipbutton, i18n( "Skip this break") );

    setTimeout( 0 ); // no auto close
    setView( vbox );
    readSettings();
}

RSIRelaxPopup::~RSIRelaxPopup()
{
}

void RSIRelaxPopup::relax( int n, bool bigBreakNext )
{
    if (!m_usePopup)
        return;

    /*
      Counts how many times a request for relax resets
      due to detected activity.
    */
    static int resetcount = 0;

    /*
        If n increases compared to the last call,
        we want a new request for a relax moment.
    */
    if ( n >= m_progress->value() )
    {
        m_progress->setRange(0, n );
        resetcount += 1;
        if( n > m_progress->value() )
          flash();
        else if ( resetcount % 4 == 0 ) // flash regulary when the user keeps working
          flash();
    }

    if ( n > 0 )
    {
        QString text = i18n("Please relax for 1 second",
                            "Please relax for %n seconds",
                            n );

        if ( bigBreakNext )
          text.append('\n'+i18n("Note: next break is a big break") );

        m_message->setText( text );

        m_progress->setValue( n );

        // ProcessEvents prevents jumping of the dialog and only call show()
        // once to prevent resizing when the label changes.
        kapp->processEvents();
        if (resetcount == 1)
            show();
    }
    else
    {
        setVisible( false );
        resetcount = 0;
    }
}

void RSIRelaxPopup::flash()
{
    if( m_useFlash )
    {
      QTimer::singleShot( 500, this, SLOT( unflash() ) );
      setPaletteForegroundColor( KGlobalSettings::highlightedTextColor() );
      setPaletteBackgroundColor( KGlobalSettings::highlightColor() );
    }
}

void RSIRelaxPopup::unflash()
{
    unsetPalette();
}

void RSIRelaxPopup::mouseReleaseEvent( QMouseEvent * )
{
    /* eat this! */
}

void RSIRelaxPopup::slotReadConfig()
{
    readSettings();
}

void RSIRelaxPopup::readSettings()
{
    KConfigGroup config = KGlobal::config()->group("Popup Settings");
    m_usePopup=config.readEntry("UsePopup", true);
    m_useFlash=config.readEntry("UseFlash", true);
}

void RSIRelaxPopup::setVisible( bool b )
{
    if( !b ) hide();
}

void RSIRelaxPopup::setSkipButtonHidden ( bool b )
{
    m_skipbutton->setHidden( b );
}

#include "rsirelaxpopup.moc"
