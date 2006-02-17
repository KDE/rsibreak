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

#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <stdio.h>

#include "rsiwidget.h"

/**
 * @class RSIApplication
 * @author Tom Albers <tomalbers@kde.nl>
 * This class is a simple inheritance from KUniqueApplication
 * the reason that it is reimplemented is that when RSIBreak
 * is launched a second time it would in the orinal implementation
 * make the RSIWidget->show(). which we do not want. This
 * class misses that call.
 */
class RSIApplication : public KUniqueApplication {
    public:
        /**
         * Constructor
         */
        RSIApplication() {}
        /**
         * Destructor
         */
        ~RSIApplication() {}

        /**
         * Similar to KUniqueApplication::newInstance, only without
         * the call to raise the widget when a second instance is started.
         */
        int newInstance() { return 0;};
};

int main( int argc, char *argv[] )
{
    KAboutData aboutData( "rsibreak",
                          I18N_NOOP("RSIBreak"),
                          "0.5.0",
                          I18N_NOOP("Try to prevent Repetitive Strain Injury by "
                                  "reminding a user to rest."),
                          KAboutData::License_GPL,
                          "(c) 2005, The RSIBreak developers" );

    aboutData.addAuthor( "Tom Albers", I18N_NOOP("Maintainer and Author"),
                         "tomalbers@kde.nl", "http://www.omat.nl");

    aboutData.addAuthor( "Bram Schoenmakers", I18N_NOOP("Author"),
                         "bramschoenmakers@kde.nl" );

    aboutData.addCredit( "Lee Olson", I18N_NOOP("Logo and Icons"),
                         "clearbeast@gmail.com");

    aboutData.addCredit( "Achim Bohnet",
                         I18N_NOOP("Packaging and help with related issues"),
                         "ach@mpe.mpg.de");

    KCmdLineArgs::init( argc, argv, &aboutData );

    if (!KUniqueApplication::start()) {
        fprintf(stderr, "RSIBreak is already running!\n");
        exit(0);
    }

    RSIApplication a;

    RSIWidget rsi;

    a.setMainWidget( &rsi );
    return a.exec();
}
