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
#include <QSessionManager>
#include <KDBusService>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDebug>
#include <Kdelibs4ConfigMigrator>

#include "rsiwidget.h"

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed( false );
    
    Kdelibs4ConfigMigrator migrate(QLatin1String("rsibreak"));
    migrate.setConfigFiles(QStringList() << QLatin1String("rsibreakrc") << QLatin1String("rsibreak.notifyrc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("rsibreak");

    KAboutData aboutData( "rsibreak",
                          i18n( "RSIBreak" ),
                          "0.12",
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

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("autostart"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    auto disableSessionManagement = [](QSessionManager &sm) {
        sm.setRestartHint(QSessionManager::RestartNever);
    };
    QObject::connect(&app, &QGuiApplication::commitDataRequest, disableSessionManagement);
    QObject::connect(&app, &QGuiApplication::saveStateRequest, disableSessionManagement);

    if (parser.isSet("autostart")) {
        KConfigGroup config = KSharedConfig::openConfig()->group( "General" );
        const bool autostart = config.readEntry( "AutoStart", false );
        if (!autostart)
            return 0;
    }

    new RSIObject();
    return app.exec();
}
