/* This file is part of the KDE project
   Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>

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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "rsiwidget.h"

int main( int argc, char *argv[] )
{
    KAboutData aboutData( "rsibreak",
                          I18N_NOOP("RSIBreak"),
                          "0.0.1",
                          I18N_NOOP("Try to prevent Repetitive Strain Injury by "
                                  "reminding a user to rest."),
                          KAboutData::License_GPL,
                          "(c) 2005, T.M. Albers" );
    aboutData.addAuthor( "Tom Albers", I18N_NOOP("Maintainer"),
                         "tomalbers@kde.nl", "http://www.omat.nl");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication a;

    RSIWidget *rsi = new RSIWidget(  );

    a.setMainWidget( rsi );
    return a.exec();
}
