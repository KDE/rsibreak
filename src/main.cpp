/*
    SPDX-FileCopyrightText: 2005-2007, 2009-2010 Tom Albers <toma@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KConfigGroup>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QSessionManager>
#include <kaboutdata.h>
#include <kmessagebox.h>
#include <kstartupinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "rsiwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    KLocalizedString::setApplicationDomain("rsibreak");

    KAboutData aboutData("rsibreak",
                         i18n("RSIBreak"),
                         "0.13.0",
                         i18n("Try to prevent Repetitive Strain Injury by "
                              "reminding a user to rest."),
                         KAboutLicense::GPL,
                         i18n("(c) 2005-2015, The RSIBreak developers"));

    aboutData.addAuthor(i18n("Albert Astals Cid"), i18n("Maintainer"), "aacid@kde.org");

    aboutData.addAuthor(i18n("Tom Albers"), i18n("Former author"), "toma@kde.org", "http://www.omat.nl");

    aboutData.addAuthor(i18n("Bram Schoenmakers"), i18n("Former author"), "bramschoenmakers@kde.nl");

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("rsibreak")));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
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
        KConfigGroup config = KSharedConfig::openConfig()->group("General");
        const bool autostart = config.readEntry("AutoStart", false);
        if (!autostart)
            return 0;
    }

    KCrash::initialize();

    new RSIObject();
    return app.exec();
}
