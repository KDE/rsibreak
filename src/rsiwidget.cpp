/*
    SPDX-FileCopyrightText: 2005-2006, 2009-2010 Tom Albers <toma@kde.org>
    SPDX-FileCopyrightText: 2006 Bram Schoenmakers <bramschoenmakers@kde.nl>
    SPDX-FileCopyrightText: 2010 Juan Luis Baptiste <juan.baptiste@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "rsiwidget.h"
#include "grayeffect.h"
#include "plasmaeffect.h"
#include "popupeffect.h"
#include "rsidock.h"
#include "rsiglobals.h"
#include "rsirelaxpopup.h"
#include "rsitimer.h"
#include "rsiwidgetadaptor.h"
#include "slideshoweffect.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QTimer>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KNotification>
#include <KSharedConfig>
#include <QDBusInterface>
#include <QTemporaryFile>

#include <KFormat>
#include <math.h>
#include <time.h>

RSIObject::RSIObject(QWidget *parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_effect(nullptr)
    , m_useImages(false)
    , m_usePlasma(false)
    , m_usePlasmaRO(false)
{
    // Keep these 2 lines _above_ the messagebox, so the text actually is right.
    m_tray = new RSIDock(this);
    m_tray->setIconByName("rsibreak0");

    new RsiwidgetAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/rsibreak", this);

    m_relaxpopup = new RSIRelaxPopup(nullptr);
    connect(m_relaxpopup, &RSIRelaxPopup::lock, this, &RSIObject::slotLock);

    connect(m_tray, &RSIDock::configChanged, RSIGlobals::instance(), &RSIGlobals::slotReadConfig);
    connect(m_tray, &RSIDock::configChanged, this, &RSIObject::readConfig);
    connect(m_tray, &RSIDock::configChanged, m_relaxpopup, &RSIRelaxPopup::slotReadConfig);
    connect(m_tray, &RSIDock::suspend, m_relaxpopup, &RSIRelaxPopup::setSuspended);

    readConfig();

    setIcon(0);

    QTimer::singleShot(2000, this, &RSIObject::slotWelcome);
}

RSIObject::~RSIObject()
{
    delete m_effect;
    delete RSIGlobals::instance();
    delete m_timer;
}

void RSIObject::slotWelcome()
{
    if (KMessageBox::shouldBeShownContinue("dont_show_welcome_again_for_001")) {
        KMessageBox::information(nullptr,
                                 i18n("<p>Welcome to RSIBreak</p>\n<p>"
                                      "In your tray you can now see RSIBreak.</p>\n")
                                     + i18n("<p>When you right-click on that you will see a menu, from which "
                                            "you can go to the configuration for example.</p>\n<p>When you want to "
                                            "know when the next break is, hover over the icon.</p>\n<p>Use RSIBreak "
                                            "wisely.</p>"),
                                 i18n("Welcome"),
                                 "dont_show_welcome_again_for_001");
    }
}

void RSIObject::minimize()
{
    m_effect->deactivate();
}

void RSIObject::maximize()
{
    m_effect->activate();
}

void RSIObject::slotLock()
{
    m_effect->deactivate();
    m_timer->slotLock();

    QDBusInterface lock("org.freedesktop.ScreenSaver", "/ScreenSaver", "org.freedesktop.ScreenSaver");
    lock.call("Lock");
}

void RSIObject::setCounters(int timeleft)
{
    if (timeleft > 0) {
        m_effect->setLabel(KFormat().formatSpelloutDuration(timeleft * 1000));
    } else if (m_timer->isSuspended()) {
        m_effect->setLabel(i18n("Suspended"));
    } else {
        m_effect->setLabel(QString());
    }
}

void RSIObject::updateIdleAvg(double idleAvg)
{
    if (idleAvg == 0.0)
        setIcon(0);
    else if (idleAvg > 0 && idleAvg < 30)
        setIcon(1);
    else if (idleAvg >= 30 && idleAvg < 60)
        setIcon(2);
    else if (idleAvg >= 60 && idleAvg < 90)
        setIcon(3);
    else
        setIcon(4);
}

void RSIObject::setIcon(int level)
{
    QString newIcon = "rsibreak" + (m_timer->isSuspended() ? QString("x") : QString::number(level));

    if (newIcon != m_currentIcon) {
        m_tray->setIconByName(newIcon);
        m_tray->setToolTipIconByName(newIcon);
        m_currentIcon = newIcon;
    }
}

// ------------------- Popup for skipping break ------------- //

void RSIObject::tinyBreakSkipped()
{
    m_notificator.onShortTimerReset();
}

void RSIObject::bigBreakSkipped()
{
    m_notificator.onTimersReset();
}

//--------------------------- CONFIG ----------------------------//

void RSIObject::configureTimer()
{
    if (m_timer != nullptr) {
        m_timer->updateConfig();
        return;
    }
    m_timer = new RSITimer(this);

    connect(m_timer, &RSITimer::breakNow, this, &RSIObject::maximize, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::updateWidget, this, &RSIObject::setCounters, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::updateToolTip, m_tray, &RSIDock::setCounters, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::updateIdleAvg, this, &RSIObject::updateIdleAvg, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::minimize, this, &RSIObject::minimize, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::relax, m_relaxpopup, &RSIRelaxPopup::relax, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::tinyBreakSkipped, this, &RSIObject::tinyBreakSkipped, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::bigBreakSkipped, this, &RSIObject::bigBreakSkipped, Qt::QueuedConnection);
    connect(m_timer, &RSITimer::startLongBreak, &m_notificator, &Notificator::onStartLongBreak);
    connect(m_timer, &RSITimer::endLongBreak, &m_notificator, &Notificator::onEndLongBreak);
    connect(m_timer, &RSITimer::startShortBreak, &m_notificator, &Notificator::onStartShortBreak);
    connect(m_timer, &RSITimer::endShortBreak, &m_notificator, &Notificator::onEndShortBreak);

    connect(m_tray, &RSIDock::dialogEntered, m_timer, &RSITimer::slotStop);
    connect(m_tray, &RSIDock::dialogLeft, m_timer, &RSITimer::slotStart);
    connect(m_tray, &RSIDock::suspend, m_timer, &RSITimer::slotSuspended);

    connect(m_relaxpopup, &RSIRelaxPopup::skip, m_timer, &RSITimer::skipBreak);
    connect(m_relaxpopup, &RSIRelaxPopup::postpone, m_timer, &RSITimer::postponeBreak);
}

void RSIObject::readConfig()
{
    KConfigGroup config = KSharedConfig::openConfig()->group("General Settings");

    m_relaxpopup->setSkipButtonHidden(config.readEntry("HideMinimizeButton", false));
    m_relaxpopup->setLockButtonHidden(config.readEntry("HideLockButton", false));
    m_relaxpopup->setPostponeButtonHidden(config.readEntry("HidePostponeButton", false));

    m_usePlasma = config.readEntry("UsePlasma", false);
    m_usePlasmaRO = config.readEntry("UsePlasmaReadOnly", false);

    m_useImages = config.readEntry("ShowImages", false);
    int slideInterval = config.readEntry("SlideInterval", 10);
    bool recursive = config.readEntry("SearchRecursiveCheck", false);
    bool showSmallImages = config.readEntry("ShowSmallImagesCheck", true);
    const bool expandImageToFullScreen = config.readEntry("ExpandImageToFullScreen", true);
    QString path = config.readEntry("ImageFolder");

    configureTimer();

    int effect = config.readEntry("Effect", 0);

    delete m_effect;
    switch (effect) {
    case Plasma: {
        m_effect = new PlasmaEffect(nullptr);
        m_effect->setReadOnly(m_usePlasmaRO);
        break;
    }
    case SlideShow: {
        SlideEffect *slide = new SlideEffect(nullptr);
        slide->reset(path, recursive, showSmallImages, expandImageToFullScreen, slideInterval);
        if (slide->hasImages())
            m_effect = slide;
        else {
            delete slide;
            m_effect = new GrayEffect(nullptr);
        }
        break;
    }
    case Popup: {
        PopupEffect *effect = new PopupEffect(nullptr);
        m_effect = effect;
        break;
    }
    case SimpleGray:
    default: {
        GrayEffect *effect = new GrayEffect(nullptr);
        effect->setLevel(config.readEntry("Graylevel", 80));
        m_effect = effect;
        break;
    }
    }
    connect(m_effect, &BreakBase::skip, m_timer, &RSITimer::skipBreak);
    connect(m_effect, &BreakBase::lock, this, &RSIObject::slotLock);
    connect(m_effect, &BreakBase::postpone, m_timer, &RSITimer::postponeBreak);

    m_effect->showMinimize(!config.readEntry("HideMinimizeButton", false));
    m_effect->showLock(!config.readEntry("HideLockButton", false));
    m_effect->showPostpone(!config.readEntry("HidePostponeButton", false));
    m_effect->disableShortcut(config.readEntry("DisableAccel", false));
}

void RSIObject::resume()
{
    m_tray->doResume();
}

void RSIObject::suspend()
{
    m_tray->doSuspend();
}
