/*
   Copyright (C) 2005-2007,2009-2010 Tom Albers <toma@kde.org>

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

#include <kmessagebox.h>
#include <kstartupinfo.h>
#include <KLocalizedString>
#include <kaboutdata.h>
#include <stdio.h>
#include <stdlib.h>
#include <QApplication>
#include <QCommandLineParser>
#include <KDBusService>

#include "rsiwidget.h"

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    
    KAboutData aboutData( "rsibreak",
                          i18n( "RSIBreak" ),
                          "0.11",
                          i18n( "Try to prevent Repetitive Strain Injury by "
                                 "reminding a user to rest." ),
                          KAboutLicense::GPL,
                          i18n( "(c) 2005-2015, The RSIBreak developers" ) );

    aboutData.addAuthor( i18n( "Albert Astals Cid" ), i18n( "Maintainer" ),
                         "aacid@kde.org" );

    aboutData.addAuthor( i18n( "Tom Albers" ), i18n( "Former author" ),
                         "toma@kde.org", "http://www.omat.nl" );

    aboutData.addAuthor( i18n( "Bram Schoenmakers" ), i18n( "Former author" ),
                         "bramschoenmakers@kde.nl" );

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    //PORTING SCRIPT: adapt aboutdata variable if necessary
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service;

    // TODO unique app
    // TODO migrate settings
    // TODO i18n catalog
    // TODO
//     a.disableSessionManagement();
    app.setQuitOnLastWindowClosed( false );

    new RSIObject();
    return app.exec();
}
